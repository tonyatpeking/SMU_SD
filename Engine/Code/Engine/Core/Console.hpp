﻿#pragma once
#include <string>
#include <vector>
#include <functional>

#include "Engine/Core/EngineCommonH.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Input/InputObserver.hpp"
#include <mutex>

class Renderer;
class InputSystem;
class BitmapFont;
class Logger;
class IConsoleObserver;
struct LogEntry;

void Print( const string& str, const Rgba& color = Rgba::WHITE );
void Printf( const char* format, ... );
void Printf( const Rgba& color, const char* format, ... );
void Printfv(  const char* format, va_list args );
void Printfv( const Rgba& color, const char* format, va_list args );

class Console : public InputObserver
{

public:

    Console( Renderer* renderer, InputSystem* inputSys );
    ~Console();

    void ShutDown();


    void Render();
    void Update( float deltaSeconds );

    // Default console
    static Console* GetDefault();
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
    Rgba GetDefaultTextColor() { return m_defaultTextColor; };

    // Mutators
    void SetInputSystem( InputSystem* inputSys );
    void SetRenderer( Renderer* renderer ) { m_renderer = renderer; };
    void SetFont( BitmapFont* font ) { m_font = font; };
    void SetActive( bool isActive );
    void SetScreenBounds( const AABB2& screenBounds );
    void SetOutputFilePath( const string& filePath ) { m_outputFilePath = filePath; };
    bool OutputToFile();
    void SetSizeRatio( float sizeRatio );
    void ToggleActive();
    void ScrollTextUp( int lines );
    void ScrollTextDown( int lines );
    void ScrollToStart();
    void ScrollToEnd();
    void SetScorllPosition( int lineNum );
    void InputWithEnter( string text );
    void Clear();
    void ClearInput();
    void OnEnterPressed();  // called when 'Enter' is pressed
    void UsePython( bool use );

    // the text will be split at each newline '\n'
    void Print( const string& text, const Rgba& color );
    void Print( const string& text );
    void Printfv( const Rgba& color, const char* format, va_list args );
    void Printfv( const char* format, va_list args );
    void Printf( const Rgba& color, const char* format, ... );
    void Printf( const char* format, ... );

    string GetInputPrompt() const;

    float GetFontHeight() { return m_fontHeight; };

    // Logging
    static void LoggerCB( LogEntry* entry, void* me );
    void HookToLogger( Logger* logger );

    // Observers
    void AddObserver( IConsoleObserver* ob );
    void RemoveObserver( IConsoleObserver* ob );

private:

    // Drawing
    void DrawInputText( Vec2& out_nextPos ) const; // Cannot span multiple lines
    void DrawOutputText( const Vec2& startPos ) const; // Can span multiple lines
    void DrawOneLine( const string& text, const Rgba& color, const Vec2& startPos ) const; //Cannot span multiple lines, text will be truncated
    void DrawOneOutputString( const string& text, const Rgba& color, const Vec2& startPos, Vec2& out_nextPos, bool& out_overflow ) const; // String can span multiple lines

    // Utilities
    float GetStringDrawHeight( const string& text ) const; // How high will the text box be taking into account multi line span
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

    // Command History
    void LoadFromCommandHistoryFile();
    void DumpToCommandHistoryFile();
    //offset 1 is previous command, 2 is the one before that, 0 is no command selected
    string GetCommandFromHistory(int offset);
    void AppendToCommandHistory( const string& str );
    void ClearCommandHistorySelector();
    // 1 is up and -1 is down, wraps
    void MoveCommandHistorySelector( int direction );

    void NotifyObservers( const string& text );


    // The default console
    static Console* s_defaultConsole;

    // Console parameters
    AABB2 m_screenBounds = AABB2::ZEROS_ONES;
    float m_sizeHeightRatio = 1.f; // Ratio in terms of screen height, ranges from 0~1
    float m_paddingToWidthRatio = 0.01f;
    float m_padding = 0.f;
    AABB2 m_bounds = AABB2::ZEROS_ONES;
    AABB2 m_textBounds = AABB2::ZEROS_ONES;
    float m_fontHeightRatioOfScreenHeight = 0.016f;
    float m_fontHeight = 0.f;
    float m_lineSpacingRatio = 0.5f; // Ratio in terms of font height, 0 is no spacing
    float m_lineSpacing = 0.f;
    float m_cursorBlinkInterval = 1.f;
    string m_outputFilePath;
    Rgba m_backgroundColor = Rgba( 0, 0, 0, 230 );
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
    vector<Rgba> m_allOutputColors;
    string m_inputText;
    BitmapFont* m_font = nullptr;

    // Thread Safety
    std::mutex m_printMutex;

    // Command History
    string m_historyFile = "Logs/command_history.txt";
    int m_maxCommandHistoryLength = 20;
    Strings m_commandHistory;
    int m_commandHistorySelectorPos = 0;

    // Observers
    vector<IConsoleObserver*> m_observers;
};