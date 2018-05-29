#pragma once

struct KeyButtonState
{
public:
    void Clear(); // Clears all flags
public:
    bool m_justPressed = false;
    bool m_justReleased = false;
    bool m_isDown = false;
};