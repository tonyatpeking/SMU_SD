#pragma once


class IConsoleObserver
{
public:
    virtual void OnConsolePrint( const String& str ) = 0;

};
