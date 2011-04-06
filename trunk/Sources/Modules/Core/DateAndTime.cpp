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


#include <Core/DateAndTime.h>

#include <boost/date_time/local_time/local_time.hpp>
#include <math.h>
#include <ctime>

typedef boost::posix_time::time_duration ptime_duration;
typedef Core::Detail::Ltime Ltime;
typedef Core::Detail::Ltime_duration Ltime_duration;
typedef Core::Detail::Ldate Ldate;

namespace Core
{
//-----------------------------------------------------------------------------
//le  namespace Detail contient les classe interne necessaire à la classe Clock et TimeValue
namespace Detail
{
//-----------------------------------------------------------------------------
//La classe Ltime fait l'interface de la classe boost::posix_time::ptime
//pour permettre d'acceder au membre time_
class Ltime: public boost::posix_time::ptime
{
public:
    Ltime(const TimeValue& t) :
        boost::posix_time::ptime(boost::posix_time::int64_time_rep(t._time))
    {
    }
    Ltime(const ptime& t) :
        boost::posix_time::ptime(t)
    {
    }
    inline int64 getTime() const
    {
        return time_.get_rep().as_number();
    }
};

//-----------------------------------------------------------------------------
//La classe Ltime_duration fait l'interface de la classe boost::posix_time::time_duration
//pour permettre d'acceder au membre ticks_
class Ltime_duration: public boost::posix_time::time_duration
{
    typedef int64 TicksValue_t;
public:
    Ltime_duration(const TicksValue_t& val)
    {
        ticks_ = val;
    }
    Ltime_duration(const time_duration& td) :
        boost::posix_time::time_duration(td)
    {
    }
    Ltime_duration(const TimeDuration& td)
    {
        ticks_ = td.getTicks();
    }
    Ltime_duration(const int32 h, const int32 m, const int32 s, const int32 ms) :
        boost::posix_time::time_duration(h, m, s, ms)
    {
    }
};
//-----------------------------------------------------------------------------
class Ldate: public boost::gregorian::date
{
public:
    Ldate(const boost::gregorian::date& d) :
        boost::gregorian::date(d)
    {
        ;
    }
    Ldate(const Date& D) :
        boost::gregorian::date(boost::gregorian::date::date_int_type(D._day))
    {
    }
    inline int64 getDay() const
    {
        return days_;
    }
};
#ifdef _WIN32

#define LM_THREAD_AFFINITY_MASK 1
union int64DualType
{
    LARGE_INTEGER largeInt;
    __int64 int64;
};
//-----------------------------------------------------------------------------
//boost microsec_clock dispose d'une précision de l'ordre de la milisecondes sous windows
//on utilise la classe Detail::WindowsClock pour avoir une précision de l'ordre de la microseconde
class WindowsClock
{
public:
    static TimeValue localTime();
    static TimeValue universalTime();

    WindowsClock();
    virtual ~WindowsClock();

protected:
    static TimeValue _localStartTimeStamp;
    static TimeValue _universalStartTimeStamp;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
WindowsClock::WindowsClock()
{
    int64DualType counter;
    DWORD_PTR oldMask = SetThreadAffinityMask(GetCurrentThread(), LM_THREAD_AFFINITY_MASK);
    QueryPerformanceCounter(&counter.largeInt);
    SetThreadAffinityMask(GetCurrentThread(), oldMask);

    Ltime l_ptime_boost = boost::posix_time::microsec_clock::local_time();

    _localStartTimeStamp = l_ptime_boost;
    _localStartTimeStamp._queryCounter = counter.int64;

    l_ptime_boost = boost::posix_time::microsec_clock::universal_time();
    _universalStartTimeStamp = l_ptime_boost;
    _universalStartTimeStamp._queryCounter = counter.int64;
}

//-----------------------------------------------------------------------------
WindowsClock::~WindowsClock()
{
}

//-----------------------------------------------------------------------------
TimeValue WindowsClock::localTime()
{
    int64DualType counter;
    DWORD_PTR oldMask = SetThreadAffinityMask(GetCurrentThread(), LM_THREAD_AFFINITY_MASK);
    QueryPerformanceCounter(&counter.largeInt);
    SetThreadAffinityMask(GetCurrentThread(), oldMask);

    Ltime l_ptime_boost = boost::posix_time::microsec_clock::local_time();

    TimeValue local = l_ptime_boost;
    local._queryCounter = counter.int64;

    return local;
}

//-----------------------------------------------------------------------------
TimeValue WindowsClock::universalTime()
{
    int64DualType counter;
    DWORD_PTR oldMask = SetThreadAffinityMask(GetCurrentThread(), LM_THREAD_AFFINITY_MASK);
    QueryPerformanceCounter(&counter.largeInt);
    SetThreadAffinityMask(GetCurrentThread(), oldMask);

    Ltime l_ptime_boost = boost::posix_time::microsec_clock::universal_time();

    TimeValue universal = l_ptime_boost;
    universal._queryCounter = counter.int64;

    return universal;
}

Core::TimeValue WindowsClock::_localStartTimeStamp;
Core::TimeValue WindowsClock::_universalStartTimeStamp;
static WindowsClock winClock;

#endif// _WIN32

} //namespace Detail

TimeValue Clock::UniversalStartTime = Clock::universalTime();
TimeValue Clock::LocalStartTime = Clock::localTime();

//-----------------------------------------------------------------------------
TimeDuration::TimeDuration(const Ltime_duration& td)
{
    _ticks = td.ticks();
#ifdef _WIN32
    _queryCounter = 0;
#endif
}

//-----------------------------------------------------------------------------
TimeDuration::TimeDuration(const int32 h, const int32 m, const int32 s, const int32 ms)
{
    *this = Ltime_duration(h, m, s, ms);
#ifdef _WIN32
    _queryCounter = 0;
#endif
}

//-----------------------------------------------------------------------------
String TimeDuration::toString(bool humanReadable, ETimeDurationPrintMode printMode) const
{
    String result;

    switch (printMode)
    {
    case TD_PRINT_HOURS:
        if (humanReadable)
        {
            String8 str8;
            str8 = to_simple_string(static_cast<Ltime_duration> (*this)).c_str();
            result = String(str8);
        }
        else
        {
            result = Core::toString(_ticks, 8);
        }
        break;

    case TD_PRINT_COUNTER:
        if (humanReadable)
        {
            //TODO voir si on passe en microseconde
#ifdef _WIN32
            result = Core::toString(_queryCounter, 10);
#endif
        }
        else
        {
#ifdef _WIN32
            result = Core::toString(_queryCounter, 10);
#endif
        }
        break;
    }

    return result;
}

//-----------------------------------------------------------------------------
int32 TimeDuration::hours() const
{
    return static_cast<Ltime_duration> (*this).hours();
}

//-----------------------------------------------------------------------------
int32 TimeDuration::minutes() const
{
    return static_cast<Ltime_duration> (*this).minutes();
}

//-----------------------------------------------------------------------------
int32 TimeDuration::seconds() const
{
    return static_cast<Ltime_duration> (*this).seconds();
}

//-----------------------------------------------------------------------------
TimeDuration::Ticks_t TimeDuration::totalSeconds() const
{
    return static_cast<Ltime_duration> (*this).total_seconds();
}

//-----------------------------------------------------------------------------
TimeDuration::Ticks_t TimeDuration::totalMilliseconds() const
{
    return static_cast<Ltime_duration> (*this).total_milliseconds();
}

//-----------------------------------------------------------------------------
TimeDuration::Ticks_t TimeDuration::totalMicroseconds() const
{
    return static_cast<Ltime_duration> (*this).total_microseconds();
}

//-----------------------------------------------------------------------------
double TimeDuration::ftotalSeconds() const
{
    double totalMicroSeconds = (double)totalMicroseconds();
    return totalMicroSeconds / 1000000.0;
}

//-----------------------------------------------------------------------------
Date::Date(int32 year, EMonthName month, int32 monthDay)
{
    boost::gregorian::date::year_type y = year;
    boost::gregorian::date::month_type m = static_cast<int32> (month) + 1;
    boost::gregorian::date::day_type d = monthDay;

    boost::gregorian::date boostDate(y, m, d);
    Ldate ld(boostDate);
    _day = ld.getDay();
}

//-----------------------------------------------------------------------------
Date::~Date()
{
}

//-----------------------------------------------------------------------------
int32 Date::year() const
{
    Ldate ld(*this);

    return ld.year();
}

//-----------------------------------------------------------------------------
EMonthName Date::month() const
{
    Ldate ld(*this);
    return static_cast<EMonthName> (ld.month() - 1);
}

//-----------------------------------------------------------------------------
int32 Date::monthDay() const
{
    Ldate ld(*this);
    return ld.day();
}

//-----------------------------------------------------------------------------
TimeValue::TimeValue(const TimeValueComponents& tvc)
{
    tm pt_tm;

    pt_tm.tm_sec = tvc.sec;
    pt_tm.tm_min = tvc.min;
    pt_tm.tm_hour = tvc.hour;
    pt_tm.tm_mday = tvc.monthDay;
    pt_tm.tm_mon = static_cast<int> (tvc.month);
    pt_tm.tm_year = tvc.year;
    pt_tm.tm_wday = static_cast<int> (tvc.day);
    pt_tm.tm_yday = tvc.yearDay;
    pt_tm.tm_isdst = tvc.isdst;

    boost::posix_time::ptime pt = boost::posix_time::ptime_from_tm(pt_tm);

    Ltime lt(pt);
    _time = lt.getTime();

#ifdef _WIN32
    _queryCounter = 0;
#endif
}

//-----------------------------------------------------------------------------
TimeValue::TimeValue(const Date& d, const TimeDuration& td)
{
    Ldate ld(d);
    Ltime_duration ltd(td);

    boost::posix_time::ptime pt(ld, ltd);

    Ltime lt(pt);
    _time = lt.getTime();

#ifdef _WIN32
    _queryCounter = 0;
#endif
}

//-----------------------------------------------------------------------------
TimeValue::TimeValue()
{
    _time = 0;
#ifdef _WIN32
    _queryCounter = 0;
#endif
}

//-----------------------------------------------------------------------------
TimeValue::~TimeValue()
{
}

//-----------------------------------------------------------------------------
TimeValue::TimeValue(const Detail::Ltime& t)
{
    _time = t.getTime();
#ifdef _WIN32
    _queryCounter = 0;
#endif
}

//-----------------------------------------------------------------------------
TimeValue::TimeValue(const time_t& t)
{
    Ltime lt = boost::posix_time::from_time_t(t);
    _time = lt.getTime();

#ifdef _WIN32
    _queryCounter = 0;
#endif
}

//-----------------------------------------------------------------------------
#ifdef _WIN32
TimeValue::TimeValue(const FILETIME& ft)
{
    Ltime lt  = boost::posix_time::from_ftime<boost::posix_time::ptime>(ft);
    _time = lt.getTime();
    _queryCounter = 0;
}
#endif

//-----------------------------------------------------------------------------
bool TimeValue::operator==(const TimeValue& other) const
{
#ifdef _WIN32
    return (_queryCounter == other._queryCounter && static_cast<Ltime>(*this) == static_cast<Ltime>(other));
#else
    return static_cast<Ltime>(*this) == static_cast<Ltime>(other);
#endif
}

//-----------------------------------------------------------------------------
bool TimeValue::operator!=(const TimeValue& other) const
{
#ifdef _WIN32
    return (_queryCounter != other._queryCounter || static_cast<Ltime>(*this) != static_cast<Ltime>(other));
#else
    return !(static_cast<Ltime>(*this) == static_cast<Ltime>(other));
#endif
}

//-----------------------------------------------------------------------------
bool TimeValue::operator<(const TimeValue& other) const
{
#ifdef _WIN32
    if (_time == other._time)
    return _queryCounter<other._queryCounter;
    else
    return static_cast<Ltime>(*this) < static_cast<Ltime>(other);
#else
    return static_cast<Ltime>(*this) < static_cast<Ltime>(other);
#endif
}

//-----------------------------------------------------------------------------
bool TimeValue::operator<=(const TimeValue& other) const
{
#ifdef _WIN32
    if (_time == other._time)
    return _queryCounter <= other._queryCounter;
    else
    return static_cast<Ltime>(*this) <= static_cast<Ltime>(other);
#else
    return static_cast<Ltime>(*this) <= static_cast<Ltime>(other);
#endif
}

//-----------------------------------------------------------------------------
bool TimeValue::operator>(const TimeValue& other) const
{
#ifdef _WIN32
    if (_time == other._time)
    return _queryCounter > other._queryCounter;
    else
    return static_cast<Ltime>(*this) > static_cast<Ltime>(other);
#else
    return static_cast<Ltime>(*this) > static_cast<Ltime>(other);
#endif
}

//-----------------------------------------------------------------------------
bool TimeValue::operator>=(const TimeValue& other) const
{
#ifdef _WIN32
    if (_time == other._time)
    return _queryCounter >= other._queryCounter;
    else
    return static_cast<Ltime>(*this) >= static_cast<Ltime>(other);
#else
    return static_cast<Ltime>(*this) >= static_cast<Ltime>(other);
#endif
}

//-----------------------------------------------------------------------------
TimeDuration TimeValue::operator-(const TimeValue& other) const
{
#ifdef _WIN32
    TimeDuration td = static_cast<Ltime_duration> (static_cast<Ltime>(*this) - static_cast<Ltime>(other));
    td._queryCounter = _queryCounter - other._queryCounter;
    return td;
#else
    return static_cast<Ltime_duration> (static_cast<Ltime>(*this) - static_cast<Ltime>(other));
#endif
}

//-----------------------------------------------------------------------------
TimeValue TimeValue::operator+(const TimeDuration& td) const
{
#ifdef _WIN32
    TimeValue tv = TimeValue(static_cast<Ltime>(*this) + static_cast<Ltime_duration>(td));
    tv._queryCounter = _queryCounter + td._queryCounter;
    return tv;
#else
    return TimeValue(static_cast<Ltime>(*this) + static_cast<Ltime_duration>(td));
#endif
}

//-----------------------------------------------------------------------------
TimeValue TimeValue::operator+=(const TimeDuration& td)
{
#ifdef _WIN32
    TimeValue tv = TimeValue(static_cast<Ltime>(*this) + static_cast<Ltime_duration>(td));
    tv._queryCounter = _queryCounter + td._queryCounter;
    *this = tv;
    return tv;
#else
    TimeValue tv = *this + td;
    *this = tv;
    return tv;
#endif
}

//-----------------------------------------------------------------------------
TimeValue TimeValue::operator-(const TimeDuration& td) const
{
#ifdef _WIN32
    TimeValue tv = TimeValue(static_cast<Ltime>(*this) - static_cast<Ltime_duration>(td));
    tv._queryCounter = _queryCounter - td._queryCounter;
    return tv;
#else
    return TimeValue(static_cast<Ltime>(*this) - static_cast<Ltime_duration>(td));
#endif
}

//-----------------------------------------------------------------------------
TimeValue TimeValue::operator-=(const TimeDuration& td)
{
#ifdef _WIN32
    TimeValue tv = TimeValue(static_cast<Ltime>(*this) - static_cast<Ltime_duration>(td));
    tv._queryCounter = _queryCounter - td._queryCounter;
    *this = tv;
    return tv;
#else
    TimeValue tv = *this - td;
    *this = tv;
    return tv;
#endif
}

//-----------------------------------------------------------------------------
String TimeValue::toString(ETimeValuePrintMode printMode) const
{
    String result;
    String8 str8;
    switch(printMode)
    {
#ifdef _WIN32
        case TV_PRINT_DATETIME:
        str8 = boost::posix_time::to_iso_extended_string(Ltime(*this)).c_str();
        result = String (str8);
        break;

        case TV_PRINT_HOURS:
        str8 = boost::posix_time::to_iso_extended_string(Ltime(*this)).c_str();
        result = String (str8);
        result = result.slice(11);
        break;

        case TV_PRINT_COUNTER:
        result = Core::toString(_queryCounter);
#else
        case TV_PRINT_DATETIME:
        str8 = boost::posix_time::to_iso_extended_string(Ltime(*this)).c_str();
        result = String (str8);
        break;

        case TV_PRINT_HOURS:
        str8 = boost::posix_time::to_iso_extended_string(Ltime(*this)).c_str();
        result = String (str8);
        result = result.slice(11);
        break;

        case TV_PRINT_COUNTER:
        result = L"noCounter";
#endif
        break;
    }

    return result;
}
//-----------------------------------------------------------------------------
TimeValueComponents getTimeComponents(const TimeValue& tv)
{
    Ltime lt (tv);
    TimeValueComponents tvc;
    tm pt_tm = to_tm(lt);

    tvc.sec = pt_tm.tm_sec;
    tvc.min = pt_tm.tm_min;
    tvc.hour = pt_tm.tm_hour;
    tvc.monthDay = pt_tm.tm_mday;
    tvc.month = static_cast<EMonthName>(pt_tm.tm_mon);
    tvc.year = pt_tm.tm_year;
    tvc.day = static_cast<EDayName>(pt_tm.tm_wday);
    tvc.yearDay = pt_tm.tm_yday;
    tvc.isdst = pt_tm.tm_isdst;

    return tvc;
}
//-----------------------------------------------------------------------------
TimeValue Clock::localTime()
{
#ifdef _WIN32
    //boost microsec_clock dispose d'une précision de l'ordre de la milisecondes sous windows
    //on utilise la classe Detail::WindowsClock pour avoir une précision de l'ordre de la microseconde
    return Detail::WindowsClock::localTime();
#else
    return Detail::Ltime (boost::posix_time::microsec_clock::local_time());
#endif
}

//-----------------------------------------------------------------------------
TimeValue Clock::universalTime()
{
#ifdef _WIN32
    //boost microsec_clock dispose d'une précision de l'ordre de la milisecondes sous windows
    //on utilise la classe Detail::WindowsClock pour avoir une précision de l'ordre de la microseconde
    return Detail::WindowsClock::universalTime();
#else
    return Ltime (boost::posix_time::microsec_clock::universal_time());
#endif
}

//-----------------------------------------------------------------------------
TimeDuration Clock::applicationTime()
{
    return universalTime() - UniversalStartTime;
}

} //namespace Core
