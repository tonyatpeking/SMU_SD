
// Time.hpp
//
#pragma once

namespace TimeUtils
{
struct SysTime
{
    int year;
    int month;
    int dayOfWeek;
    int day;
    int hour;
    int minute;
    int second;
    int milliseconds;
};

double GetCurrentTimeSeconds();

SysTime GetDateTime();



}

