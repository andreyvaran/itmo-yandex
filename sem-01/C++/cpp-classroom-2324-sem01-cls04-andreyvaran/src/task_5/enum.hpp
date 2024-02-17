#pragma once
#include <iostream>


enum class WeekDay {
    Tempday,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};
const char *GetDayOfWeekName(WeekDay w);

bool IsWeekend(WeekDay w);

WeekDay GetDayOfWeek(size_t year, size_t month, size_t day);
