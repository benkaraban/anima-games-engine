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

#include <Core/String.h>
#include <Core/List.h>
#include "Tools.h"

namespace Core
{
//-----------------------------------------------------------------------------
LM_ENUM_3(EStrState, STR_TEXT, STR_META, STR_NUMBER);
LM_ENUM_4(ECharType, CHAR_ALPHA, CHAR_NUMERIC, CHAR_META, CHAR_END);
//-----------------------------------------------------------------------------
String strFormat(const String & str, const String & s1)
{
    List<String> args;
    args.push_back(s1);
    return strFormat(str, args);
}
//-----------------------------------------------------------------------------
String strFormat(const String & str, const String & s1, const String & s2)
{
    List<String> args;
    args.push_back(s1);
    args.push_back(s2);
    return strFormat(str, args);
}
//-----------------------------------------------------------------------------
String strFormat(const String & str, const String & s1, const String & s2, const String & s3)
{
    List<String> args;
    args.push_back(s1);
    args.push_back(s2);
    args.push_back(s3);
    return strFormat(str, args);
}
//-----------------------------------------------------------------------------
String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4)
{
    List<String> args;
    args.push_back(s1);
    args.push_back(s2);
    args.push_back(s3);
    args.push_back(s4);
    return strFormat(str, args);
}
//-----------------------------------------------------------------------------
String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5)
{
    List<String> args;
    args.push_back(s1);
    args.push_back(s2);
    args.push_back(s3);
    args.push_back(s4);
    args.push_back(s5);
    return strFormat(str, args);
}
//-----------------------------------------------------------------------------
String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5, const String & s6)
{
    List<String> args;
    args.push_back(s1);
    args.push_back(s2);
    args.push_back(s3);
    args.push_back(s4);
    args.push_back(s5);
    args.push_back(s6);
    return strFormat(str, args);
}
//-----------------------------------------------------------------------------
String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5, const String & s6, const String & s7)
{
    List<String> args;
    args.push_back(s1);
    args.push_back(s2);
    args.push_back(s3);
    args.push_back(s4);
    args.push_back(s5);
    args.push_back(s6);
    args.push_back(s7);
    return strFormat(str, args);
}
//-----------------------------------------------------------------------------
String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5, const String & s6, const String & s7, const String & s8)
{
    List<String> args;
    args.push_back(s1);
    args.push_back(s2);
    args.push_back(s3);
    args.push_back(s4);
    args.push_back(s5);
    args.push_back(s6);
    args.push_back(s7);
    args.push_back(s8);
    return strFormat(str, args);
}
//-----------------------------------------------------------------------------
String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5, const String & s6, const String & s7, const String & s8, const String & s9)
{
    List<String> args;
    args.push_back(s1);
    args.push_back(s2);
    args.push_back(s3);
    args.push_back(s4);
    args.push_back(s5);
    args.push_back(s6);
    args.push_back(s7);
    args.push_back(s8);
    args.push_back(s9);
    return strFormat(str, args);
}
//-----------------------------------------------------------------------------
String strFormat(const String & str, const List<String> & args)
{
    EStrState state = STR_TEXT;
    String number;
    String result;
    result.reserve(str.size());

    int32 ii = 0;

    while(ii <= str.size())
    {
        wchar_t c = (ii < str.size()) ? str[ii] : L'\0';
        ECharType ctype;

        if(ii >= str.size())
            ctype = CHAR_END;
        else if(c == L'%')
            ctype = CHAR_META;
        else if(c >= L'0' && c <= L'9')
            ctype = CHAR_NUMERIC;
        else
            ctype = CHAR_ALPHA;

        switch(state)
        {
        case STR_TEXT:
        {
            switch(ctype)
            {
            case CHAR_END:
                break;
            case CHAR_ALPHA:
            case CHAR_NUMERIC:
                result.push_back(c);
                break;
            case CHAR_META:
                state = STR_META;
                break;
            }
            break;
        }
        case STR_META:
        {
            switch(ctype)
            {
            case CHAR_END:
            case CHAR_ALPHA:
                LM_ASSERT(false);
                result << L"<lone % error>";
                result.push_back(c);
                state = STR_TEXT;
                break;
            case CHAR_NUMERIC:
                number.clear();
                number.push_back(c);
                state = STR_NUMBER;
                break;
            case CHAR_META:
                result.push_back(c);
                state = STR_TEXT;
                break;
            }
            break;
        }
        case STR_NUMBER:
        {
            switch(ctype)
            {
            case CHAR_NUMERIC:
                number.push_back(c);
                break;
            case CHAR_END:
            case CHAR_ALPHA:
            case CHAR_META:
            {
                int32 iArg = Core::toInteger(number) - 1;
                if(iArg < 0 || iArg >= args.size())
                {
                    LM_ASSERT(false);
                    result << L"<% index error>";
                }
                else
                    result << args[iArg];
                
                if(ctype == CHAR_ALPHA)
                {
                    result.push_back(c);
                    state = STR_TEXT;
                }
                else
                    state = STR_META;

                break;
            }
            } // switch(ctype)
            break;
        } // case STR_NUMBER
        } // switch(state)

        ii ++;

    } // while

    return result;
}
//-----------------------------------------------------------------------------
void tokenize(const String & source, List<String> & tokens, const String & separators)
{
    tokens.clear();
    int32 iStart = 0;
    int32 iToken = source.find_first_of(separators);

    while(iToken != String::npos)
    {
        if(iToken == 0)
            tokens.push_back(String::EMPTY);
        else
            tokens.push_back(source.slice(iStart, iToken -1));
        iStart = iToken + 1;
        iToken = source.find_first_of(separators, iStart);
    }

    if (iStart<source.size())
        tokens.push_back(source.slice(iStart));
}
//-----------------------------------------------------------------------------
void replaceChar(String & source, wchar_t findChar, wchar_t replaceChar)
{
    int32 iChar = source.find_first_of(findChar);

    while(iChar != String::npos)
    {
        source[iChar] = replaceChar;
        iChar = source.find_first_of(findChar, iChar + 1);
    }
}
//-----------------------------------------------------------------------------
void trimLeft(const String & source, String & target, const String & trimChars)
{
    int32 iStart = source.find_first_not_of(trimChars);
    if(iStart == String::npos)
        target.clear();
    else   
        target = source.slice(iStart);
}
//-----------------------------------------------------------------------------
void trimRight(const String & source, String & target, const String & trimChars)
{
    int32 iEnd = source.find_last_not_of(trimChars);
    if(iEnd == String::npos)
        target.clear();
    else   
        target = source.slice(0, iEnd);
}
//-----------------------------------------------------------------------------
void trim(const String & source, String & target, const String & trimChars)
{
    target.clear();

    int32 iStart = 0;
    int32 iTrim = source.find_first_of(trimChars);

    while(iTrim != String::npos)
    {
        target << source.slice(iStart, iTrim);
        iTrim = source.find_first_not_of(trimChars, iTrim);
        if(iTrim != String::npos)
        {
            iStart = iTrim;
            iTrim = source.find_first_of(trimChars, iStart);
        }
        else
        {
            iStart = int32(String::npos);
        }
    }

    if(iStart != String::npos)
        target << source.slice(iStart);
}
//-----------------------------------------------------------------------------
void trimAll(const String & source, String & target, const String & trimChars)
{
    String tmp;
    trimLeft(source, target, trimChars);
    trimRight(target, tmp, trimChars);
    trim(tmp, target, trimChars);
}
//-----------------------------------------------------------------------------
void removeControlChars(const String & source, String & target)
{
    target.clear();
    for(int32 ii=0; ii < source.size(); ii++)
        if(int32(source[ii]) >= 0x20)
            target.push_back(source[ii]);
}
//-----------------------------------------------------------------------------
bool isValidMail(const String & mail)
{
    int32 iAro = mail.find_first_of(L"@");

    if(iAro == 0 || iAro == String::npos || iAro >= mail.size() - 1)
        return false;
    
    int32 iDot = mail.find_last_of(L".");
    int32 iAro2 = mail.find_first_of(L"@", iAro + 1);

    if(iDot < iAro || iDot >= mail.size() - 1)
        return false;

    if(iAro2 != String::npos)
        return false;

    return true;
}
//-----------------------------------------------------------------------------
} // namespace Core
