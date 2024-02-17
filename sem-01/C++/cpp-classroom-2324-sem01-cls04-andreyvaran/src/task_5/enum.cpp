#include <iostream>
#include <string>
#include "enum.hpp"

const char *GetDayOfWeekName(WeekDay w) {
    switch (w) {
        case WeekDay::Sunday:
            return "Sunday";
        case WeekDay::Monday:
            return "Monday";
        case WeekDay::Tuesday:
            return "Tuesday";
        case WeekDay::Wednesday:
            return "Wednesday";
        case WeekDay::Thursday:
            return "Thursday";
        case WeekDay::Friday:
            return "Friday";
        case WeekDay::Saturday:
            return "Saturday";
        case WeekDay::Tempday:
            break;
    }
    return "Invalid";
}

bool IsWeekend(WeekDay w) {
    return w == WeekDay::Saturday || w == WeekDay::Sunday;
}

WeekDay GetDayOfWeek(size_t year, size_t month, size_t day) {
    if (month < 3) {
        month += 12;
        year--;
    }
    size_t K = year % 100;
    size_t J = year / 100;
    size_t dayOfWeek = (day + 13 * (month + 1) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;

    switch (dayOfWeek) {
        case 0:
            return WeekDay::Saturday;
        case 1:
            return WeekDay::Sunday;
        case 2:
            return WeekDay::Monday;
        case 3:
            return WeekDay::Tuesday;
        case 4:
            return WeekDay::Wednesday;
        case 5:
            return WeekDay::Thursday;
        case 6:
            return WeekDay::Friday;
    }

    return WeekDay::Sunday;
}

