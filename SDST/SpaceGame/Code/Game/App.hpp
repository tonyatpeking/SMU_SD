#pragma once

class Game;

class App
{
public:
    App();
    ~App();

    void RunFrame();
    void OnQuitRequested();
    bool IsQuitting() const;

private:
    const char* m_appName;
    bool m_isQuitting = false;
};