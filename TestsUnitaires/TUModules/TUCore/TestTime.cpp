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
#include <UTests.h>
#include <Core/Exception.h>
#include <Core/Standard.h>
#include <Core/RT/Thread.h>
#include <Core/DateAndTime.h>
#include <Core/StopWatch.h>
#include <ctime>

using namespace Core;

void testDateAndTime()
{
    const int64 value = 879654;
    const int64 hou = 12;
    const int64 min = 41;
    const int64 sec = 45;
    const int64 msec = 452;
    const int32 sleepDuration = 584;
    const int32 sleepEpsilon = 100;
    const int32 year = 2009;
    const EMonthName monthName = MARCH;
    const int32 monthDay = 12;
    const EDayName dayName = THURSDAY;
    const int32 yearDay = 70;
    const int32 isdst = -1;

    //TimeDuration

    //constructeur
    TimeDuration td_1;
    UTEST_EQU((int32) td_1.getTicks(), 0);

    TimeDuration td_2(value);
    UTEST_EQU((int32) td_2.getTicks(), (int32) value);

    TimeDuration td_3(hou, min, sec, msec);
    UTEST_EQU((int32) td_3.hours(), (int32) hou);
    UTEST_EQU((int32) td_3.minutes(), (int32) min);
    UTEST_EQU((int32) td_3.seconds(), (int32) sec);

    //total
    UTEST_EQU((int32) td_3.totalSeconds(), (int32) (hou * 60 * 60 + min * 60 + sec));
    UTEST_EQU((int32) td_3.totalMilliseconds(), (int32) ((hou * 60 * 60 + min * 60 + sec) * 1000));
    UTEST_EQU((int32) td_3.totalMicroseconds(), (int32) ((((hou * 60 * 60 + min * 60 + sec) * 1000) * 1000) + msec));

    //comparaison
    UTEST_EQU(td_1<td_2 ,true)
    UTEST_EQU(td_1>td_2 ,false)

    //operateur
    TimeDuration td_4 (-td_3);
    UTEST_EQU((int32) td_4.getTicks(), -1 * (int32) td_3.getTicks());
    TimeDuration td_6 = td_4 + td_3;
    UTEST_EQU((int32) td_6.getTicks(), (int32) td_4.getTicks() + (int32) td_3.getTicks());
    TimeDuration td_7 = td_4;
    td_7 += td_3;
    UTEST_EQU((int32) td_7.getTicks(), (int32) td_6.getTicks());

    td_6 = td_4 - td_3;
    UTEST_EQU((int32) td_6.getTicks(), (int32) td_4.getTicks() - (int32) td_3.getTicks());
    td_7 = td_4;
    td_7 -= td_3;
    UTEST_EQU((int32) td_7.getTicks(), (int32) td_6.getTicks());

    td_6 = td_4 * value;
    UTEST_EQU((int32) td_6.getTicks(), (int32) (td_4.getTicks() * value));
    td_7 = td_4;
    td_7 *= value;
    UTEST_EQU((int32) td_7.getTicks(), (int32) td_6.getTicks());

    td_6 = td_4 / value;
    UTEST_EQU((int32) td_6.getTicks(), (int32) (td_4.getTicks() / value));
    td_7 = td_4;
    td_7 /= value;
    UTEST_EQU((int32) td_7.getTicks(), (int32) td_6.getTicks());

    //clock
    Clock clock;

    TimeValue tv_l_1;
    TimeValue tv_l_2;
    TimeValue tv_u_1;
    TimeValue tv_u_2;
    TimeValue tv_a_1;
    TimeValue tv_a_2;

    tv_l_1 = clock.localTime();
    tv_u_1 = clock.universalTime();
    td_1 = clock.applicationTime();
    Thread::quickSleep(sleepDuration, 1);
    tv_l_2 = clock.localTime();
    tv_u_2 = clock.universalTime();
    td_2 = clock.applicationTime();

    UTEST_EQUE((int32) ((tv_l_2 - tv_l_1).totalMilliseconds()), sleepDuration, sleepEpsilon);
    UTEST_EQUE((int32) ((tv_u_2 - tv_u_1).totalMilliseconds()), sleepDuration, sleepEpsilon);
    UTEST_EQUE((int32) ((td_2 - td_1).totalMilliseconds()), sleepDuration, sleepEpsilon);

    //TimeValue
    TimeValue tv_3(tv_l_1);
    UTEST_EQU(tv_l_1 == tv_l_2, false)
    UTEST_EQU(tv_l_1 == tv_3, true)
    UTEST_EQU(tv_l_1 != tv_l_2, true)
    UTEST_EQU(tv_l_1 != tv_3, false)
    UTEST_EQU(tv_l_1 < tv_l_2, true)
    UTEST_EQU(tv_l_2 < tv_l_1, false)
    UTEST_EQU(tv_l_1> tv_l_2, false)
    UTEST_EQU(tv_l_2> tv_l_1, true)

    TimeValue tv_4;
    const TimeValue tv_5;
    TimeDuration td_5(0, 0, 0, msec * 1000);
    tv_3 = tv_l_1 + td_5;
    UTEST_EQU((int32)((tv_3-tv_l_1).totalMilliseconds()), (int32)(td_5.totalMilliseconds()))
    tv_4 = tv_l_1;
    tv_4 += td_5;
    UTEST_EQU((int32)((tv_3-tv_5).totalMilliseconds()), (int32)((tv_4-tv_5).totalMilliseconds()))

    tv_3 = tv_l_1 - td_5;
    UTEST_EQU((int32)((tv_3-tv_l_1).totalMilliseconds()), (int32)(-1*td_5.totalMilliseconds()))
    tv_4 = tv_l_1;
    tv_4 -= td_5;
    UTEST_EQU((int32)((tv_3-tv_5).totalMilliseconds()), (int32)((tv_4-tv_5).totalMilliseconds()))

    //Date
    Date d_1 (year, monthName, monthDay);
    UTEST_EQU(d_1.year() == year, true)
    UTEST_EQU(d_1.month() == monthName, true)
    UTEST_EQU(d_1.monthDay() == monthDay, true)

    Date d_2 (year+1, monthName, monthDay);
    Date d_3(year, monthName, monthDay);
    UTEST_EQU(d_1 < d_2, true)
    UTEST_EQU(d_1> d_2, false)
    UTEST_EQU(d_1 != d_2, true)
    UTEST_EQU(d_1 == d_3, true)

    //TimeValueComponents
    TimeValueComponents tvc_1;

    tvc_1.sec = sec;
    tvc_1.min = min;
    tvc_1.hour = hou;
    tvc_1.monthDay = monthDay;
    tvc_1.month = monthName;
    tvc_1.year = year - 1900;
    tvc_1.day = dayName;
    tvc_1.yearDay = yearDay;
    tvc_1.isdst = isdst;

    TimeValue tv_6(tvc_1);
    TimeValueComponents tvc_2 = getTimeComponents(tv_6);
    UTEST_EQU(tvc_1.sec == tvc_2.sec, true)
    UTEST_EQU(tvc_1.min == tvc_2.min, true)
    UTEST_EQU(tvc_1.hour == tvc_2.hour, true)
    UTEST_EQU(tvc_1.monthDay == tvc_2.monthDay, true)
    UTEST_EQU(tvc_1.month == tvc_2.month, true)
    UTEST_EQU(tvc_1.year == tvc_2.year, true)
    UTEST_EQU(tvc_1.day == tvc_2.day, true)
    UTEST_EQU(tvc_1.yearDay == tvc_2.yearDay, true)
    UTEST_EQU(tvc_1.isdst == tvc_2.isdst, true)

    TimeValue tv_7 (Date (year, monthName, monthDay), hours(hou) + minutes(min) + seconds(sec));
    TimeValueComponents tvc_3 = getTimeComponents(tv_7);
    UTEST_EQU(tvc_3.sec == tvc_2.sec, true)
    UTEST_EQU(tvc_3.min == tvc_2.min, true)
    UTEST_EQU(tvc_3.hour == tvc_2.hour, true)
    UTEST_EQU(tvc_3.monthDay == tvc_2.monthDay, true)
    UTEST_EQU(tvc_3.month == tvc_2.month, true)
    UTEST_EQU(tvc_3.year == tvc_2.year, true)
    UTEST_EQU(tvc_3.day == tvc_2.day, true)
    UTEST_EQU(tvc_3.yearDay == tvc_2.yearDay, true)
    UTEST_EQU(tvc_3.isdst == tvc_2.isdst, true)

    //FILETIME
#ifdef _WIN32
    FILETIME ft;
    ft.dwHighDateTime = 29715317;
    ft.dwLowDateTime = 3865122988UL;
    TimeValue tv_8 (ft);
    tvc_3 = getTimeComponents(tv_8);
    UTEST_EQU(tvc_3.year == 2005-1900, true)
    UTEST_EQU(tvc_3.month == JUNE, true)
    UTEST_EQU(tvc_3.monthDay == 07, true)
    UTEST_EQU(tvc_3.hour == 15, true)
    UTEST_EQU(tvc_3.min == 30, true)
    UTEST_EQU(tvc_3.sec == 57, true)
#endif

    //time_t
    std::time_t t = 1118158776;
    TimeValue tv_9 (t);
    tvc_3 = getTimeComponents(tv_9);
    UTEST_EQU(tvc_3.year == 2005-1900, true)
    UTEST_EQU(tvc_3.month == JUNE, true)
    UTEST_EQU(tvc_3.monthDay == 07, true)
    UTEST_EQU(tvc_3.hour == 15, true)
    UTEST_EQU(tvc_3.min == 39, true)
    UTEST_EQU(tvc_3.sec == 36, true)
}

void testStopWatch()
{
    const int32 sleepDuration = 584;
    const int32 sleepEpsilon = 100;

    StopWatch sw;
    TimeDuration td;

    sw.reset();
    Thread::sleep(sleepDuration);
    td = sw.elapsed();
    UTEST_EQUE((int32) (td.totalMilliseconds()), sleepDuration, sleepEpsilon);
}

void testTime()
{
    UTEST_PACKAGE("Time");

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"DateAndTime");
    testDateAndTime();
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"StopWatch");
    testStopWatch();
    UTEST_END_TEST;

}
