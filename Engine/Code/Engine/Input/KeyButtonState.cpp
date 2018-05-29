#include "Engine/Input/KeyButtonState.hpp"

void KeyButtonState::Clear()
{
    m_justPressed = false;
    m_justReleased = false;
    m_isDown = false;
}
