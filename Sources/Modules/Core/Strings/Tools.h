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
#ifndef CORE_STRINGS_TOOLS_H_
#define CORE_STRINGS_TOOLS_H_

#include <Core/List.h>

namespace Core
{
LM_API_COR String strFormat(const String & str, const String & s1);
LM_API_COR String strFormat(const String & str, const String & s1, const String & s2);
LM_API_COR String strFormat(const String & str, const String & s1, const String & s2, const String & s3);
LM_API_COR String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4);
LM_API_COR String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5);
LM_API_COR String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5, const String & s6);
LM_API_COR String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5, const String & s6, const String & s7);
LM_API_COR String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5, const String & s6, const String & s7, const String & s8);
LM_API_COR String strFormat(const String & str, const String & s1, const String & s2, const String & s3, const String & s4, const String & s5, const String & s6, const String & s7, const String & s8, const String & s9);

LM_API_COR String strFormat(const String & str, const List<String> & args);

LM_API_COR void tokenize(const String & source, List<String> & tokens, const String & separators);
LM_API_COR void replaceChar(String & source, wchar_t findChar, wchar_t replaceChar);

/**
 * Retire les caractères trimChars qui sont à gauche de la chaîne.
 * \code
 * trimLeft(L"__   abc", target, L" _");
 * LM_ASSERT(target == L"abc")
 * \endcode
 */
LM_API_COR void trimLeft(const String & source, String & target, const String & trimChars);
LM_API_COR void trimRight(const String & source, String & target, const String & trimChars);

/**
 * Retire les caractères trimChars qui sont en double dans la chaîne.
 * \code
 * trimLeft(L"a__ b  _c   ", target, L" _");
 * LM_ASSERT(target == L"a b c ")
 * \endcode
 */
LM_API_COR void trim(const String & source, String & target, const String & trimChars);

/**
 * Retire les caractères trimChars qui sont à droite et à gauche de la chaîne.
 * Retire les caractères trimChars qui sont en double dans la chaîne.
 */
LM_API_COR void trimAll(const String & source, String & target, const String & trimChars);

LM_API_COR void removeControlChars(const String & source, String & target);

LM_API_COR bool isValidMail(const String & mail);
}

#endif /*CORE_STRINGS_TOOLS_H_*/
