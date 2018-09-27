#pragma once


class IConsoleObserver
{
public:
    virtual void OnConsolePrint( const string& str ) = 0;

};
