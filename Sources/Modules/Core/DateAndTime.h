/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * Jérémie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef CORE_DATEANDTIME_H_
#define CORE_DATEANDTIME_H_

#include <Core/Types.h>
#include <Core/String.h>
#include <Core/Enum.h>

#ifdef _WIN32
#include <winsafe.h>
#else
#include <sys/time.h>
#endif

namespace Core
{
/**
 *  La classe Clock permet d'obtenir les informations sur la date et l'heure
 *  Sur Linux l'heure dispose d'une précision à la microseconde
 *  Sur windows la précision est de l'ordre de la milliseconde - ou de la dizaine de milliseconde, pour pouvoir
 *  disposer d'une meilleure définition, on ajoute à l'heure la valeur du QueryPerformanceCounter (uniquement sous windows)
 *
 * \code
 * Core::TimeValue tv = Core::Clock::universalTime();       //donne la date et l'heure en temps universel (UTC)
 * Core::TimeValue tv = Core::Clock::localTime();           //donne la date et l'heure en temps local
 * Core::TimeDuration td = Core::Clock::applicationTime();  //donne le temps écoulé depuis le lancement de l'application
 * \endcode
 */

namespace Detail
{
class Ltime;
class WindowsClock;

class Ltime_duration;
class Ldate;
} //namespace Detail


class LM_API_COR Clock;
class LM_API_COR TimeValue;


LM_ENUM_2 (ETimeDurationPrintMode,
        TD_PRINT_HOURS,
        TD_PRINT_COUNTER)
//-----------------------------------------------------------------------------
class LM_API_COR TimeDuration
{
    friend class Detail::Ltime_duration;
    friend class TimeValue;

    typedef int64 Ticks_t;
public:
    TimeDuration():_ticks(0)
    {}

    TimeDuration(Ticks_t val):_ticks(val)
    {}

    TimeDuration(const int32 h, const int32 m, const int32 s, const int32 ms);//ms=>microseconde

    ~TimeDuration()
    {}

    bool operator<(const TimeDuration& other) const
    {
#ifdef _WIN32
        if (_ticks == other._ticks)
        return _queryCounter < other._queryCounter;
        else
        return _ticks < other._ticks;
#else
        return _ticks < other._ticks;
#endif
    }

    bool operator>(const TimeDuration& other) const
    {
#ifdef _WIN32
        if (_ticks == other._ticks)
        return _queryCounter> other._queryCounter;
        else
        return _ticks> other._ticks;
#else
        return _ticks> other._ticks;
#endif
    }

    TimeDuration operator-() const
    {
#ifdef _WIN32
        TimeDuration td;
        td._ticks = _ticks * -1;
        td._queryCounter = _queryCounter * -1;
        return td;
#else
        return TimeDuration(_ticks * (-1));
#endif
    }

    TimeDuration operator-(const TimeDuration& other) const
    {
#ifdef _WIN32
        TimeDuration td;
        td._ticks = _ticks - other._ticks;
        td._queryCounter = _queryCounter - other._queryCounter;
        return td;
#else
        return TimeDuration(_ticks - other._ticks);
#endif
    }

    TimeDuration operator+(const TimeDuration& other) const
    {
#ifdef _WIN32
        TimeDuration td;
        td._ticks = _ticks + other._ticks;
        td._queryCounter = _queryCounter + other._queryCounter;
        return td;
#else
        return TimeDuration(_ticks + other._ticks);
#endif
    }

    TimeDuration operator-=(const TimeDuration& other)
    {
#ifdef _WIN32
        TimeDuration td;
        td._ticks = _ticks - other._ticks;
        td._queryCounter = _queryCounter - other._queryCounter;
        *this = td;
        return td;
#else
        _ticks = _ticks - other._ticks;
        return TimeDuration(_ticks);
#endif
    }

    TimeDuration operator+=(const TimeDuration& other)
    {
#ifdef _WIN32
        TimeDuration td;
        td._ticks = _ticks + other._ticks;
        td._queryCounter = _queryCounter + other._queryCounter;
        *this = td;
        return td;
#else
        _ticks = _ticks + other._ticks;
        return TimeDuration(_ticks);
#endif
    }

    TimeDuration operator/(Ticks_t divisor) const
    {
#ifdef _WIN32
        TimeDuration td;
        td._ticks = _ticks / divisor;
        td._queryCounter = _queryCounter / divisor;
        return td;
#else
        return TimeDuration(_ticks / divisor);
#endif
    }

    TimeDuration operator/=(Ticks_t divisor)
    {
#ifdef _WIN32
        TimeDuration td;
        td._ticks = _ticks / divisor;
        td._queryCounter = _queryCounter / divisor;
        *this = td;
        return td;
#else
        _ticks = _ticks / divisor;
        return TimeDuration(_ticks);
#endif
    }

    TimeDuration operator*(Ticks_t multiplicator) const
    {
#ifdef _WIN32
        TimeDuration td;
        td._ticks = _ticks * multiplicator;
        td._queryCounter = _queryCounter * multiplicator;
        return td;
#else
        return TimeDuration(_ticks * multiplicator);
#endif
    }

    TimeDuration operator*=(Ticks_t multiplicator)
    {
#ifdef _WIN32
        TimeDuration td;
        td._ticks = _ticks * multiplicator;
        td._queryCounter = _queryCounter * multiplicator;
        *this = td;
        return td;
#else
        _ticks = _ticks * multiplicator;
        return TimeDuration(_ticks);
#endif
    }

    Ticks_t getTicks() const
    {
        return _ticks;
    }

    String toString(bool humanReadable = true, ETimeDurationPrintMode printMode = TD_PRINT_HOURS) const;

    int32 hours() const;

    int32 minutes() const;

    int32 seconds() const;

    double ftotalSeconds() const;

    Ticks_t totalSeconds() const;

    Ticks_t totalMilliseconds() const;

    Ticks_t totalMicroseconds() const;

protected:
    TimeDuration (const Detail::Ltime_duration& td);

protected:
    Ticks_t _ticks;
#ifdef _WIN32
    int64 _queryCounter; //pour windows on rajoute la valeur du performanceCounter pour disposer d'une precision de l'ordre de la microseconde
#endif
};
//-----------------------------------------------------------------------------
inline TimeDuration hours(const int32 h)
{   return TimeDuration(h,0,0,0);}
//-----------------------------------------------------------------------------
inline TimeDuration minutes(const int32 m)
{   return TimeDuration(0,m,0,0);}
//-----------------------------------------------------------------------------
inline TimeDuration seconds(const int32 s)
{   return TimeDuration(0,0,s,0);}
//-----------------------------------------------------------------------------
inline TimeDuration microseconds(const int32 ms)
{   return TimeDuration(0,0,0,ms);}
//-----------------------------------------------------------------------------
LM_ENUM_12 (EMonthName,
        JANUARY,
        FEBRUARY,
        MARCH,
        APRIL,
        MAY,
        JUNE,
        JULY,
        AUGUST,
        SEPTEMBER,
        OCTOBER,
        NOVEMBER,
        DECEMBER)

LM_ENUM_7 (EDayName,
        SUNDAY,
        MONDAY,
        TUESDAY,
        WEDNESDAY,
        THURSDAY,
        FRIDAY,
        SATURDAY)
//-----------------------------------------------------------------------------
class LM_API_COR Date
{
    friend class Detail::Ldate;

public:
    Date(int32 year, EMonthName month, int32 monthDay);
    ~Date();

    int32 year() const;
    EMonthName month() const;
    int32 monthDay() const;

    bool operator<(const Date& other) const
    {   return _day < other._day;}
    bool operator>(const Date& other) const
    {   return _day> other._day;}
    bool operator==(const Date& other) const
    {   return _day == other._day;}
    bool operator!=(const Date& other) const
    {   return _day != other._day;}

protected:
    int64 _day;
};
//on reprend le principe de la structure tm (Time structure) du fichier ctime en utilisant les enum
struct LM_API_COR TimeValueComponents
{
    int32 sec; //en general  [0-59] (0-61 to accommodate for leap seconds in certain systems)
    int32 min; //minutes dans l'heure [0-59]
    int32 hour; //L'heure en cours [0-23]
    int32 monthDay; //le jour du mois [1-31]
    EMonthName month; //le nom du mois en cours
    int32 year; //année depuis 1900
    EDayName day; //le nom du jour de la semaine
    int32 yearDay; //jour depuis le 01 janvier [0-365]
    int32 isdst; //is greater than zero if Daylight Saving Time is in effect, zero if Daylight Saving Time is not in effect, and less than zero if the information is not available
};

//-----------------------------------------------------------------------------
LM_ENUM_3 (ETimeValuePrintMode,
        TV_PRINT_DATETIME,
        TV_PRINT_HOURS,
        TV_PRINT_COUNTER)
class LM_API_COR TimeValue
{
    friend class Clock;
    friend class Detail::Ltime;
    friend class Detail::WindowsClock;

public:
    TimeValue();
    TimeValue(const TimeValueComponents& tvc);
    TimeValue(const Date& d, const TimeDuration& td);
    TimeValue(const time_t& t);
#ifdef _WIN32
    TimeValue(const FILETIME& ft);
#endif
    virtual ~TimeValue();
    String toString (ETimeValuePrintMode printMode = TV_PRINT_DATETIME) const;
    //
    bool operator== (const TimeValue& other) const;
    bool operator!= (const TimeValue& other) const;
    bool operator< (const TimeValue& other) const;
    bool operator<=(const TimeValue& other) const;
    bool operator> (const TimeValue& other) const;
    bool operator>=(const TimeValue& other) const;
    //
    TimeDuration operator- (const TimeValue& other) const;
    //
    TimeValue operator+ (const TimeDuration& td) const;
    TimeValue operator+= (const TimeDuration& td);
    TimeValue operator- (const TimeDuration& td) const;
    TimeValue operator-= (const TimeDuration& td);

protected:
    TimeValue(const Detail::Ltime& t);

public:
    int64 _time;
#ifdef _WIN32
    int64 _queryCounter; //pour windows on rajoute la valeur du performanceCounter pour disposer d'une precision de l'ordre de la microseconde
#endif
};
//-----------------------------------------------------------------------------
LM_API_COR TimeValueComponents getTimeComponents(const TimeValue& tv);
//-----------------------------------------------------------------------------
class LM_API_COR Clock
{
public:
    Clock(){}
    static TimeValue localTime();
    static TimeValue universalTime();
    static TimeDuration applicationTime();

protected:
    static TimeValue UniversalStartTime;
    static TimeValue LocalStartTime;
};

inline String toString (const TimeDuration& td, bool humanReadable = true, ETimeDurationPrintMode printMode = TD_PRINT_HOURS)
{
    return td.toString(humanReadable, printMode);
}

inline String toString (const TimeValue& tv, ETimeValuePrintMode printMode = TV_PRINT_DATETIME)
{
    return tv.toString(printMode);
}

}
//namespace Core
#endif /*CORE_DATEANDTIME_H_*/
