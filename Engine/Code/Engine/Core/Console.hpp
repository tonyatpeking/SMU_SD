#pragma once
#include <string>
#include <vector>

#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Input/InputObserver.hpp"

class Renderer;
class InputSystem;
class BitmapFont;

class Console : public InputObserver
{

public:

    Console( Renderer* renderer, InputSystem* inputSys );
    ~Console() {};

    void Render();
    void Update( float deltaSeconds );

    // Default console
    static Console* DefaultConsole();
    static void SetDefaultConsole( Console* console );

    // Subscriptions
    void NotifyKeyPressed( unsigned char keyCode ) override;
    void NotifyKeyReleased( unsigned char keyCode ) override;
    void NotifyCharInput( unsigned char asciiCode ) override;
    void NotifyKeyPressedWithRepeat( unsigned char keyCode ) override;
    // Methods

    // Accessors
    bool IsActive() const { return m_isActive; };
    bool HasInput() const { return !m_inputText.empty(); };

    // Mutators
    void SetInputSystem( InputSystem* inputSys );
    void SetRenderer( Renderer* renderer ) { m_renderer = renderer; };
    void SetFont( BitmapFont* font ) { m_font = font; };
    void SetActive( bool isActive );
    void SetScreenBounds( const AABB2& screenBounds );
    void SetOutputFilePath( const String& filePath ) { m_outputFilePath = filePath; };
    bool OutputToFile();
    void SetSizeRatio( float sizeRatio );
    void ToggleActive();
    void ScrollTextUp( int lines );
    void ScrollTextDown( int lines );
    void ScrollToStart();
    void ScrollToEnd();
    void SetScorllPosition( int lineNum );
    void InputWithEnter( String text );
    void Clear();
    void ClearInput();
    void OnEnterPressed();  // called when 'Enter' is pressed
    void UsePython( bool use );

    // the text will be split at each newline '\n'
    void Print( const String& text, const Rgba& color );
    void Print( const String& text );
    void Printfv( const Rgba& color, const char* format, va_list args );
    void Printf( const Rgba& color, const char* format, ... );
    void Printf( const char* format, ... );

private:

    // Drawing
    void DrawInputText( Vec2& out_nextPos ) const; // Cannot span multiple lines
    void DrawOutputText( const Vec2& startPos ) const; // Can span multiple lines
    void DrawOneLine( const String& text, const Rgba& color, const Vec2& startPos ) const; //Cannot span multiple lines, text will be truncated
    void DrawOneOutputString( const String& text, const Rgba& color, const Vec2& startPos, Vec2& out_nextPos, bool& out_overflow ) const; // String can span multiple lines

    // Utilities
    float GetStringDrawHeight( const String& text ) const; // How high will the text box be taking into account multi line span
    int GetCharsPerLine() const; // How many chars will fit in one line
    int GetOutputMaxLinesShown() const; // How many vertical chars can be shown
    float GetTextboxWidth() const;
    float GetTextboxHeight() const;
    void UpdateBounds(); // Automatically called in SetScreenBounds and SetSizeRatio

    // Cursor
    void TranslateCursorPosition( int offset );
    void SetCursorPosition( int position );
    void DrawCursor();
    bool IsCursorBlinkOn();

    // The default console
    static Console* s_defaultConsole;

    // Console parameters
    AABB2 m_screenBounds = AABB2::ZEROS_ONES;
    float m_sizeHeightRatio = 1.f; // Ratio in terms of screen height, ranges from 0~1
    float m_paddingToWidthRatio = 0.01f;
    float m_padding = 0.f;
    AABB2 m_bounds = AABB2::ZEROS_ONES;
    AABB2 m_textBounds = AABB2::ZEROS_ONES;
    float m_fontHeightRatioOfScreenHeight = 0.015f;
    float m_fontHeight = 0.f;
    float m_lineSpacingRatio = 0.3f; // Ratio in terms of font height, 0 is no spacing
    float m_lineSpacing = 0.f;
    float m_cursorBlinkInterval = 1.f;
    String m_inputPrompt = ">>> ";
    String m_outputFilePath;
    Rgba m_backgroundColor = Rgba( 0, 0, 0, 150 );
    Rgba m_defaultTextColor = Rgba::LIGHT_GRAY;

    // Dependent Engine Systems
    Renderer* m_renderer;
    InputSystem* m_inputSystem;

    // Console state
    bool m_isActive = false;
    bool m_usePython = false;
    int m_currentOutputLine = -1;
    float m_age = 0.f;
    int m_cursorPosition = 0;
    int m_linesToScroll = 3;

    // Text
    Strings m_previousInputText;
    Strings m_allOutputText;
    std::vector<Rgba> m_allOutputColors;
    String m_inputText;
    BitmapFont* m_font = nullptr;

};