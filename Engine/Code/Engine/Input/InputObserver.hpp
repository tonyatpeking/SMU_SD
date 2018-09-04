#pragma once

#include "Engine/Core/EngineCommonH.hpp"

class InputObserver
{
public:
    virtual void NotifyKeyPressed( unsigned char keyCode ) { UNUSED( keyCode ); };
    virtual void NotifyKeyReleased( unsigned char keyCode ) { UNUSED( keyCode ); };
    virtual void NotifyCharInput( unsigned char asciiCode ) { UNUSED( asciiCode ); };
    virtual void NotifyKeyPressedWithRepeat( unsigned char keyCode ) { UNUSED( keyCode ); };
    virtual void NotifyStringInput( const String& str ) { UNUSED( str ); };
};