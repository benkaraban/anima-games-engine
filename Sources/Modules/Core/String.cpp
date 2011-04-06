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
#include "String.h"

#include <stdio.h>
#include <wchar.h>
#include <sstream>
#include <Core/Strings/ConvertUTF.h>
#include <Core/Math/MathTools.h>

#if defined _MSC_VER
#define LM_SPRINTF sprintf_s
#define LM_SWSCANF swscanf_s
#else
#define LM_SPRINTF snprintf
#define LM_SWSCANF swscanf

#include <math.h>

#define INFINITE INFINITY
#endif

namespace Core
{
//-----------------------------------------------------------------------------
LM_API_COR const String8 String8::EMPTY("");
LM_API_COR const String String::EMPTY(L"");
//-----------------------------------------------------------------------------
LM_API_COR String8::String8(const String & string)
{
    if(sizeof(wchar_t) == 4)
    {
        int32 sourceLength = string.length();
        const UTF32 * sourceStart = reinterpret_cast<const UTF32*>(string.data());
        const UTF32 * sourceEnd = sourceStart + sourceLength;

        int32 destMaxLength = 4 * sourceLength;
        resize(destMaxLength);

        UTF8 * targetStart = reinterpret_cast<UTF8*>(const_cast<char*>(data()));
        UTF8 * targetEnd = targetStart + destMaxLength;
        UTF8 * initialTargetStart = targetStart;

        ConvertUTF32toUTF8 (&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);
        resize(targetStart - initialTargetStart);
    }
    else
    {
        int32 sourceLength = string.length();
        const UTF16 * sourceStart = reinterpret_cast<const UTF16*>(string.data());
        const UTF16 * sourceEnd = sourceStart + sourceLength;

        int32 destMaxLength = 2 * sourceLength;
        resize(destMaxLength);

        UTF8 * targetStart = reinterpret_cast<UTF8*>(const_cast<char*>(data()));
        UTF8 * targetEnd = targetStart + destMaxLength;
        UTF8 * initialTargetStart = targetStart;

        ConvertUTF16toUTF8 (&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);
        resize(targetStart - initialTargetStart);
    }
}
//-----------------------------------------------------------------------------
LM_API_COR String::String(const String8 & string)
{
    if(sizeof(wchar_t) == 4)
    {
        int32 sourceLength = string.length();
        const UTF8 * sourceStart = reinterpret_cast<const UTF8*>(string.data());
        const UTF8 * sourceEnd = sourceStart + sourceLength;

        resize(sourceLength);

        UTF32 * targetStart = reinterpret_cast<UTF32*>(const_cast<wchar_t*>(data()));
        UTF32 * targetEnd = targetStart + sourceLength;
        UTF32 * initialTargetStart = targetStart;

        ConvertUTF8toUTF32 (&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);
        resize(targetStart - initialTargetStart);
    }
    else
    {
        int32 sourceLength = string.length();
        const UTF8 * sourceStart = reinterpret_cast<const UTF8*>(string.data());
        const UTF8 * sourceEnd = sourceStart + sourceLength;

        resize(sourceLength);

        UTF16 * targetStart = reinterpret_cast<UTF16*>(const_cast<wchar_t*>(data()));
        UTF16 * targetEnd = targetStart + sourceLength;
        UTF16 * initialTargetStart = targetStart;

        ConvertUTF8toUTF16 (&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);
        resize(targetStart - initialTargetStart);
    }
}
//-----------------------------------------------------------------------------
LM_API_COR void String::toUpper()
{
    for(int32 ii=0; ii < int32(size()); ii++)
        (*this)[ii] = towupper((*this)[ii]);
}
//-----------------------------------------------------------------------------
LM_API_COR void String::toLower()
{
    for(int32 ii=0; ii < int32(size()); ii++)
        (*this)[ii] = towlower((*this)[ii]);
}
//-----------------------------------------------------------------------------
LM_API_COR bool String::startsWith(const String & prefix) const
{
    int32 prefixLength = int32(prefix.length());
    bool result = (prefixLength <= int32(length()));

    int32 ii = 0;
    while(ii < prefixLength && result)
    {
        result = (prefix[ii] == (*this)[ii]);
        ii ++;
    }
    return result;
}
//-----------------------------------------------------------------------------
LM_API_COR bool String::endsWith(const String & suffix) const
{
    int32 suffixLength = suffix.length();
    int32 myLength = length();
    bool result = (suffixLength <= myLength);

    int32 ii = 1;
    while(ii <= suffixLength && result)
    {
        result = (suffix[suffixLength - ii] == (*this)[myLength - ii]);
        ii ++;
    }
    return result;
}
//-----------------------------------------------------------------------------
LM_API_COR String String::slice(int32 iStart) const
{
    return slice(iStart, length() - 1);
}
//-----------------------------------------------------------------------------
LM_API_COR String String::slice(int32 iStart, int32 iEnd) const
{
    if(iEnd < iStart)
        return String::EMPTY;
    else
        return substr(iStart, iEnd - iStart + 1);
}
//-----------------------------------------------------------------------------
const char CHAR_BUFFER_SIZE = 40;
//-----------------------------------------------------------------------------
inline String bruteConversion(const char * string)
{
    wchar_t wbuffer[CHAR_BUFFER_SIZE];
    int ii = 0;

    while(string[ii] != '\0')
    {
        wbuffer[ii] = string[ii];
        ii ++;
    }
    wbuffer[ii] = '\0';
    return String(wbuffer);
}
//-----------------------------------------------------------------------------
String toString(int32 value)
{
    char buffer[CHAR_BUFFER_SIZE];

    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%d", int(value));

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toStringByteSize(int64 value)
{
    static const int32 KB_MUL = 1024;
    static const int32 MB_MUL = KB_MUL * 1024;
    static const int32 GB_MUL = MB_MUL * 1024;
    static const int64 TB_MUL = int64(GB_MUL) * int64(1024);

    double dvalue = double(value);
    char buffer[CHAR_BUFFER_SIZE];

    if(value >= TB_MUL)
    {
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%.1f Tb", dvalue / double(TB_MUL));
    }
    else if(value >= GB_MUL)
    {
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%.1f Gb", dvalue / double(GB_MUL));
    }
    else if(value >= MB_MUL)
    {
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%.1f Mb", dvalue / double(MB_MUL));
    }
    else if(value >= KB_MUL)
    {
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%.1f Kb", dvalue / double(KB_MUL));
    }
    else
    {
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%d bytes", int32(value));
    }

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toStringByteSize(int32 value)
{
    return toStringByteSize(int64(value));
}
//-----------------------------------------------------------------------------
String toString(int64 value)
{
    char buffer[CHAR_BUFFER_SIZE];
    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%lld", value);
    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toString(uint64 value)
{
    char buffer[CHAR_BUFFER_SIZE];
    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%lld", value);
    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toString(int64 value, int32 minWidth)
{
    char printfArgument[CHAR_BUFFER_SIZE];
    LM_SPRINTF(printfArgument, CHAR_BUFFER_SIZE, "%%0%dlld", int(minWidth));

    char buffer[CHAR_BUFFER_SIZE];
    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, printfArgument, value);

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toString(uint64 value, int32 minWidth)
{
    char printfArgument[CHAR_BUFFER_SIZE];
    LM_SPRINTF(printfArgument, CHAR_BUFFER_SIZE, "%%0%dlld", int(minWidth));

    char buffer[CHAR_BUFFER_SIZE];
    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, printfArgument, value);

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toString(int32 value, int32 minWidth)
{
    char printfArgument[CHAR_BUFFER_SIZE];
    LM_SPRINTF(printfArgument, CHAR_BUFFER_SIZE, "%%0%dd", int(minWidth));

    char buffer[CHAR_BUFFER_SIZE];
    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, printfArgument, int(value));

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toStringPadSpaces(int32 value, int32 minWidth)
{
    char printfArgument[CHAR_BUFFER_SIZE];
    LM_SPRINTF(printfArgument, CHAR_BUFFER_SIZE, "%% %dd", int(minWidth));

    char buffer[CHAR_BUFFER_SIZE];
    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, printfArgument, int(value));

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toStringHex(int32 value)
{
    char buffer[CHAR_BUFFER_SIZE];
    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%X", int(value));
    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toStringHex(int32 value, int32 minWidth)
{
    char printfArgument[CHAR_BUFFER_SIZE];
    LM_SPRINTF(printfArgument, CHAR_BUFFER_SIZE, "%%0%dX", int(minWidth));

    char buffer[CHAR_BUFFER_SIZE];
    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, printfArgument, int(value));

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toString(const void * value)
{
    char buffer[CHAR_BUFFER_SIZE];
    LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%08X", int32(int64(value)));
    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toString(double value)
{
    char buffer[CHAR_BUFFER_SIZE];

    if(!isValid(value))
    {
        return L"INFINITE";
    }
    if(value >= 10000000000.0 || value <= 0.000000001)
    {
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%e", value);
    }
    else
    {
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, "%f", value);
    }

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toString(double value, int32 precision)
{
    char buffer[CHAR_BUFFER_SIZE];

    if(value >= 10000000000.0)
    {
        char printfArgument[CHAR_BUFFER_SIZE];
        LM_SPRINTF(printfArgument, CHAR_BUFFER_SIZE, "%%.%de", precision);
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, printfArgument, value);
    }
    else
    {
        char printfArgument[CHAR_BUFFER_SIZE];
        LM_SPRINTF(printfArgument, CHAR_BUFFER_SIZE, "%%.%df", precision);
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, printfArgument, value);
    }

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
String toString(double value, int32 minWidth, int32 precision)
{
    char buffer[CHAR_BUFFER_SIZE];

    if(value >= 10000000000.0)
    {
        char printfArgument[CHAR_BUFFER_SIZE];
        LM_SPRINTF(printfArgument, CHAR_BUFFER_SIZE, "%%.%de", precision);
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, printfArgument, value);
    }
    else
    {
        char printfArgument[CHAR_BUFFER_SIZE];
        LM_SPRINTF(printfArgument, CHAR_BUFFER_SIZE, "%%%d.%df", minWidth + precision + 1, precision);
        LM_SPRINTF(buffer, CHAR_BUFFER_SIZE, printfArgument, value);
    }

    return bruteConversion(buffer);
}
//-----------------------------------------------------------------------------
int32 toInteger(const String & string)
{
    int32 result;
    LM_SWSCANF(string.c_str(), L"%d", &result);
    return result;
}
//-----------------------------------------------------------------------------
int64 toInteger64(const String & string)
{
    int64 result;
    LM_SWSCANF(string.c_str(), L"%lld", &result);
    return result;
}
//-----------------------------------------------------------------------------
int32 toIntegerFromHex(const String & string)
{
    int32 result;
    LM_SWSCANF(string.c_str(), L"%x", &result);
    return result;
}
//-----------------------------------------------------------------------------
int64 toIntegerFromHex64(const String & string)
{
    int64 result;
    LM_SWSCANF(string.c_str(), L"%llx", &result);
    return result;
}
//-----------------------------------------------------------------------------
double toFloat(const String & string)
{
    if(string == L"INFINITE")
    {
        return INFINITE;
    }
    else
    {
        double result;
        LM_SWSCANF(string.c_str(), L"%lf", &result);
        return result;
    }
}
//-----------------------------------------------------------------------------
String strYesNo(bool b)
{
    if(b) return String(L"YES");
    else  return String(L"NO");
}
//-----------------------------------------------------------------------------
}
