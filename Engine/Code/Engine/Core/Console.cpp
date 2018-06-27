#include <stdarg.h>

#include "Engine/Core/PythonInterpreter.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/IO/IOUtils.hpp"
#include "Engine/Core/CommandSystem.hpp"

Console* Console::s_defaultConsole = nullptr;

Console::Console( Renderer* renderer, InputSystem* inputSys )
    : m_renderer( renderer )
    , m_inputSystem( inputSys )
{
    SetFont( m_renderer->DefaultFont() );
    SetScreenBounds( m_renderer->GetWindow()->GetLocalBounds() );
}



void Console::Render()
{
    if( m_isActive )
    {
        m_renderer->ClearDepth();
        m_renderer->UseUICamera();
        m_renderer->DrawAABB( m_bounds, m_backgroundColor );

        Vec2 out_nextPos;
        DrawInputText( out_nextPos );
        Vec2 outputStartPos = out_nextPos;
        DrawOutputText( outputStartPos );

        DrawCursor();
    }
}

void Console::Update( float deltaSeconds )
{
    m_age += deltaSeconds;
}


Console* Console::DefaultConsole()
{
    return s_defaultConsole;
}

void Console::SetDefaultConsole( Console* console )
{
    s_defaultConsole = console;
}

void Console::NotifyKeyPressed( unsigned char keyCode )
{
    UNUSED( keyCode );
}

void Console::NotifyKeyReleased( unsigned char keyCode )
{
    UNUSED( keyCode );
}

void Console::NotifyCharInput( unsigned char asciiCode )
{
    if( asciiCode == 27 ) // esc
        return;

    if( asciiCode == '`' )
        return;

    if( asciiCode == '\t' )
    {
        m_inputText.insert( m_cursorPosition, "    " );
        TranslateCursorPosition( 4 );
    }
    else if( asciiCode == '\b' )
    {
        if( !m_inputText.empty() && m_cursorPosition != 0 )
        {
            m_inputText.erase( m_cursorPosition - 1, 1 );
            TranslateCursorPosition( -1 );
        }
        return;
    }
    else if( asciiCode == '\r' )
    {
        OnEnterPressed();
        return;
    }
    else
    {
        m_inputText.insert( m_cursorPosition, 1, asciiCode );
        TranslateCursorPosition( 1 );
    }
}

void Console::NotifyKeyPressedWithRepeat( unsigned char keyCode )
{
    bool controlDown = m_inputSystem->IsKeyPressed( InputSystem::KEYBOARD_CONTROL );
    // Scrolling
    if( keyCode == InputSystem::KEYBOARD_PAGEUP && !controlDown )
        ScrollTextDown( GetOutputMaxLinesShown() );
    if( keyCode == InputSystem::KEYBOARD_PAGEDOWN && !controlDown )
        ScrollTextUp( GetOutputMaxLinesShown() );
    if( keyCode == InputSystem::KEYBOARD_HOME )
        ScrollToStart();
    if( keyCode == InputSystem::KEYBOARD_END )
        ScrollToEnd();
    if( keyCode == InputSystem::KEYBOARD_PAGEUP && controlDown )
        ScrollTextDown( m_linesToScroll );
    if( keyCode == InputSystem::KEYBOARD_PAGEDOWN && controlDown )
        ScrollTextUp( m_linesToScroll );

    // Cursor
    if( keyCode == InputSystem::KEYBOARD_LEFT_ARROW )
        TranslateCursorPosition( -1 );
    if( keyCode == InputSystem::KEYBOARD_RIGHT_ARROW )
        TranslateCursorPosition( 1 );

    // Delete
    if( keyCode == InputSystem::KEYBOARD_DELETE )
    {
        if( !m_inputText.empty() && m_cursorPosition != (int) m_inputText.size() )
        {
            m_inputText.erase( m_cursorPosition, 1 );
        }
        return;
    }
}

void Console::UpdateBounds()
{
    m_bounds = m_screenBounds;
    m_bounds.maxs.y = m_bounds.maxs.y * m_sizeHeightRatio;
    m_textBounds = m_bounds;
    m_padding = m_bounds.GetWidth() * m_paddingToWidthRatio;
    m_textBounds.AddPaddingToSides( -m_padding, -m_padding );
}

int Console::GetCharsPerLine() const
{
    float consoleWidth = GetTextboxWidth();
    float charWidth = m_fontHeight;
    return FloorToInt( consoleWidth / charWidth );
}

int Console::GetOutputMaxLinesShown() const
{
    float height = GetTextboxHeight();
    height -= m_padding;
    height -= m_fontHeight;

    int linesShown = (int) ( height / ( m_fontHeight + m_lineSpacing ) );
    return linesShown - 1;
}

float Console::GetTextboxWidth() const
{
    return m_textBounds.GetWidth();
}

float Console::GetTextboxHeight() const
{
    return m_textBounds.GetHeight();
}

void Console::TranslateCursorPosition( int offset )
{
    m_cursorPosition += offset;
    m_cursorPosition = ClampInt( m_cursorPosition, 0, (int) m_inputText.size() );
}

void Console::SetCursorPosition( int position )
{
    m_cursorPosition = position;
}

void Console::DrawCursor()
{
    if( !IsCursorBlinkOn() )
        return;

    Vec2 pos = m_textBounds.mins;
    pos.x += ( ( m_cursorPosition + m_inputPrompt.size() ) * m_fontHeight );
    pos.y -= m_fontHeight * 0.3f;
    m_renderer->DrawText2D(
        "|", pos, m_fontHeight * 1.6f, m_font, Rgba::WHITE,
        Alignment::BOTTOM_CENTER, 0.5f );
}

bool Console::IsCursorBlinkOn()
{
    if( fmodf( m_age, m_cursorBlinkInterval ) > m_cursorBlinkInterval / 2.f )
        return true;

    return false;
}



void Console::SetInputSystem( InputSystem* inputSys )
{
    m_inputSystem = inputSys;
}

void Console::SetActive( bool isActive )
{
    m_isActive = isActive;

    if( m_isActive )
        m_inputSystem->Attach( this );
    else
        m_inputSystem->Detach( this );
}

void Console::SetScreenBounds( const AABB2& screenBounds )
{
    m_screenBounds = screenBounds;
    UpdateBounds();
    m_fontHeight = screenBounds.GetHeight() * m_fontHeightRatioOfScreenHeight;
    m_lineSpacing = m_fontHeight * m_lineSpacingRatio;
}

bool Console::OutputToFile()
{
    return IOUtils::WriteToFile( m_outputFilePath, m_allOutputText );
}

void Console::SetSizeRatio( float sizeRatio )
{
    m_sizeHeightRatio = sizeRatio;
    UpdateBounds();
}

void Console::ToggleActive()
{
    if( IsActive() )
        SetActive( false );
    else
        SetActive( true );
}

void Console::ScrollTextUp( int lines )
{
    SetScorllPosition( m_currentOutputLine + lines );
}

void Console::ScrollTextDown( int lines )
{
    int lineNum = m_currentOutputLine - lines;
    lineNum = ClampInt(
        lineNum, GetOutputMaxLinesShown(), (int) m_allOutputText.size() - 1 );
    SetScorllPosition( lineNum );
}

void Console::ScrollToStart()
{
    SetScorllPosition( GetOutputMaxLinesShown() );
}

void Console::ScrollToEnd()
{
    SetScorllPosition( (int) m_allOutputText.size() - 1 );
}

void Console::SetScorllPosition( int lineNum )
{
    m_currentOutputLine = ClampInt( lineNum, 0, (int) m_allOutputText.size() - 1 );
}

void Console::Print( const String& text, const Rgba& color /*= Rgba::LIGHT_GRAY */ )
{
    Strings out_SingleLines;
    String delimiter = "\n";
    bool removeWhiteSpace = false;
    bool removeBrackets = false;
    StringUtils::ParseToTokens(
        text, out_SingleLines, delimiter, removeWhiteSpace, removeBrackets );

    for( size_t lineIdx = 0; lineIdx < out_SingleLines.size(); ++lineIdx )
    {
        m_allOutputText.push_back( out_SingleLines[lineIdx] );
        m_allOutputColors.push_back( color );
        if( m_currentOutputLine == (int) m_allOutputText.size() - 2 )
        {
            ++m_currentOutputLine;
        }
    }

}

void Console::Print( const String& text )
{
    Print( text, m_defaultTextColor );
}

void Console::Printfv( const Rgba& color, const char* format, va_list args )
{
    String str = Stringf( format, args );
    Print( str, color );
}

void Console::Printf( const Rgba& color, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    Printfv( color, format, args );
    va_end( args );
}

void Console::Printf( const char* format, ... )
{
    va_list args;
    va_start( args, format );
    Printfv( m_defaultTextColor, format, args );
    va_end( args );
}

void Console::InputWithEnter( String text )
{
    m_inputText = text;
    OnEnterPressed();
}

void Console::OnEnterPressed()
{
    SetCursorPosition( 0 );
    String text = m_inputPrompt + m_inputText;
    Print( text );
    if( m_inputText != "" )
        m_previousInputText.push_back( m_inputText );
    ScrollToEnd();

    if( m_usePython )
    {
        PythonInterpreter::GetInstance()->PushToShell( m_inputText );
        String pythonOut = PythonInterpreter::GetInstance()->ReadFromShell();
        Print( pythonOut );
    }
    else
    {
        CommandSystem::DefaultCommandSystem()->RunCommand( m_inputText );
    }

    m_inputText.clear();
}

void Console::UsePython( bool use )
{
    m_usePython = use;
}

void Console::Clear()
{
    m_cursorPosition = 0;
    m_currentOutputLine = -1;
    m_previousInputText.clear();
    m_allOutputText.clear();
    m_allOutputColors.clear();
}

void Console::ClearInput()
{
    m_cursorPosition = 0;
    m_inputText.clear();
}

void Console::DrawInputText( Vec2& out_nextPos ) const
{
    Vec2 startPos = m_textBounds.mins;
    DrawOneLine( m_inputPrompt + m_inputText, m_defaultTextColor, startPos );
    out_nextPos = startPos;
    out_nextPos.y += m_fontHeight + m_padding;
}

void Console::DrawOutputText( const Vec2& startPos ) const
{
    Vec2 stringStartPos = startPos;
    Vec2 out_nextPosition;
    for( int outputIdx = m_currentOutputLine; outputIdx >= 0; --outputIdx )
    {
        bool out_overflow = false;
        // draw blank lines to push output text to top
        if( outputIdx > (int) m_allOutputText.size() - 1 )
        {
            DrawOneOutputString(
                "", Rgba::WHITE, stringStartPos, out_nextPosition, out_overflow );
        }
        else
        {
            const String& outputText = m_allOutputText[outputIdx];
            const Rgba& color = m_allOutputColors[outputIdx];
            DrawOneOutputString(
                outputText, color, stringStartPos, out_nextPosition, out_overflow );

            if( out_overflow )
                break;
        }

        stringStartPos = out_nextPosition;

    }
}

void Console::DrawOneLine(
    const String& text, const Rgba& color, const Vec2& startPos ) const
{
    String textCopy = text;
    size_t charsPerLine = GetCharsPerLine();
    if( textCopy.size() > charsPerLine )
    {
        textCopy = textCopy.substr( 0, charsPerLine - 1 );
    }
    m_renderer->DrawText2D(
        textCopy, startPos, m_fontHeight, m_font, color, Alignment::BOTTOM_LEFT );
}

float Console::GetStringDrawHeight( const String& text ) const
{
    int lineCount = (int) text.size() / GetCharsPerLine();  // Yes we want int divide
    if( lineCount == 0 ) // blank lines take up one line
    {
        lineCount = 1;
    }
    return (float) lineCount * m_fontHeight;
}

void Console::DrawOneOutputString(
    const String& text, const Rgba& color, const Vec2& startPos, Vec2& out_nextPos,
    bool& out_overflow ) const
{
    out_overflow = false;
    int charsPerLine = GetCharsPerLine();

    Strings out_lines;
    StringUtils::ChopStringWithMaxLength( text, out_lines, charsPerLine );

    int lineCount = (int) out_lines.size();
    Vec2 lineStart = startPos;

    for( int lineIdx = lineCount - 1; lineIdx >= 0; --lineIdx )
    {
        if( lineStart.y + m_fontHeight + m_lineSpacing > m_textBounds.maxs.y )
        {
            out_overflow = true;
            break;
        }
        DrawOneLine( out_lines[lineIdx], color, lineStart );
        lineStart.y += m_fontHeight + m_lineSpacing;
    }
    out_nextPos = lineStart;


}
