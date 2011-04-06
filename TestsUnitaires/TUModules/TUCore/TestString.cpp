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
﻿
#include <UTests.h>
#include <Core/String.h>
#include <Core/Strings/Tools.h>
#include <Core/Exception.h>
#include <Core/List.h>

using Core::String;
using Core::String8;

static void testStringInit()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Initialisation");
    String   stringVide;
    String8  string8Vide;
    String   stringValue(L"123");
    String8  string8Value("123");
    
    UTEST_EQU(stringVide, String(""));
    UTEST_EQU(string8Vide, String8(""));
    UTEST_NOT_EQU(stringVide, stringValue);
    UTEST_NOT_EQU(string8Vide, string8Value);

    UTEST_END_TEST;   
}

static void testStringUTF8()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Conversion UTF8-UTF16");
    
    String   stringValue(L"Visual C++ doesn't fucking support unicode source code literals");
    String8  string8Value("Visual C++ doesn't fucking support unicode source code literals");    
    String   stringConv1(string8Value);
    String8  string8Conv1(stringValue);
        
    UTEST_EQU(stringConv1, stringValue);
    UTEST_EQU(string8Conv1, string8Value);
        
    UTEST_END_TEST;   
}

static void testStringCat()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Concatenation de strings");
    
    String   stringValue(L"Visual C++ doesn't fucking support unicode source code literals");
    String   stringConcat; 
    
    stringConcat << L"Visual C++ doesn't" << L" fucking support unicode" << L" source code literals";
           
    UTEST_EQU(stringConcat, stringValue);
        
    UTEST_END_TEST;   
}

static void testStringUpDown()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Conversion majuscules/minuscules");

    String   stringValue(L"Visual C++ doesn't fucking");
    String   expectedUpr(L"VISUAL C++ DOESN'T FUCKING");
    String   expectedLow(L"visual c++ doesn't fucking");
    
    stringValue.toUpper();    
    UTEST_EQU(stringValue, expectedUpr);
    stringValue.toLower();    
    UTEST_EQU(stringValue, expectedLow);
        
    UTEST_END_TEST;   
}

static void testStringStartEnds()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"StartsWith/endsWith");

    String testString(L"my extreme");
    UTEST_EQU(testString.startsWith(L""), true);
    UTEST_EQU(testString.startsWith(L"m"), true);
    UTEST_EQU(testString.startsWith(L"my ex"), true);
    UTEST_EQU(testString.startsWith(L"my extreme"), true);
    UTEST_EQU(testString.startsWith(L"y"), false);
    UTEST_EQU(testString.startsWith(L"my exq"), false);
    UTEST_EQU(testString.startsWith(L"my extremz"), false);
    UTEST_EQU(testString.startsWith(L"my extreme "), false);
        
    UTEST_EQU(testString.endsWith(L""), true);
    UTEST_EQU(testString.endsWith(L"e"), true);
    UTEST_EQU(testString.endsWith(L"extreme"), true);
    UTEST_EQU(testString.endsWith(L"my extreme"), true);
    UTEST_EQU(testString.endsWith(L"f"), false);
    UTEST_EQU(testString.endsWith(L"zxtreme"), false);
    UTEST_EQU(testString.endsWith(L"yy extreme"), false);
    UTEST_EQU(testString.endsWith(L"ayy extreme"), false);
    
    UTEST_END_TEST;   
}

static void testSlice()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Slice");
    String testString(L"extreme");
    UTEST_EQU(testString.slice(0), String(L"extreme"));
    UTEST_EQU(testString.slice(1), String(L"xtreme"));
    UTEST_EQU(testString.slice(3), String(L"reme"));
    UTEST_EQU(testString.slice(6), String(L"e"));
    UTEST_EQU(testString.slice(7), String(L""));

    UTEST_EQU(testString.slice(0, 2), String(L"ext"));
    UTEST_EQU(testString.slice(1, 3), String(L"xtr"));
    UTEST_EQU(testString.slice(3, 4), String(L"re"));
    UTEST_EQU(testString.slice(5, 5), String(L"m"));
    UTEST_EQU(testString.slice(2, 1), String(L""));
    UTEST_END_TEST;   
}

static void testFormat()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Format");

    Core::List<String>  args;
    args.push_back(L"first");
    args.push_back(L"second");
    args.push_back(L"42");
    args.push_back(L"def");
    args.push_back(L"abc");

    String result(Core::strFormat(L"%2 value of %1 : %3%% and %5%4-and %3", args));

    UTEST_EQU(result, String(L"second value of first : 42% and abcdef-and 42"));
    UTEST_EQU(Core::strFormat(L"with %2 embeded %1 and %% or", L"my", L"your"), String(L"with your embeded my and % or"));
    UTEST_END_TEST;   
}

static void testStringTokenize()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Tokenize");
    
    Core::List<String> tokens;

    Core::tokenize(L"aaa:bb:c::d", tokens, L":");
    
    UTEST_EQU(tokens.size(), 5);
    if(tokens.size() == 5)
    {
        UTEST_EQU(tokens[0],L"aaa");
        UTEST_EQU(tokens[1],L"bb");
        UTEST_EQU(tokens[2],L"c");
        UTEST_EQU(tokens[3],L"");
        UTEST_EQU(tokens[4],L"d");        
    }
    
    Core::tokenize(L":a:bb:", tokens, L":");

    UTEST_EQU(tokens.size(), 4);
    if(tokens.size() == 4)
    {
        UTEST_EQU(tokens[0],L"");
        UTEST_EQU(tokens[1],L"a");
        UTEST_EQU(tokens[2],L"bb");
        UTEST_EQU(tokens[3],L"");
    }

    Core::tokenize(L"abc", tokens, L":");

    UTEST_EQU(tokens.size(), 1);
    if(tokens.size() == 1)
    {
        UTEST_EQU(tokens[0],L"abc");
    }
    
    Core::tokenize(L"aaa+bb-c*/d", tokens, L"*/-+");

    UTEST_EQU(tokens.size(), 5);
    if(tokens.size() == 5)
    {
        UTEST_EQU(tokens[0],L"aaa");
        UTEST_EQU(tokens[1],L"bb");
        UTEST_EQU(tokens[2],L"c");
        UTEST_EQU(tokens[3],L"");
        UTEST_EQU(tokens[4],L"d");        
    }
    UTEST_END_TEST;   
}

static void testReplaceChar()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"ReplaceChar");
    String result = L"-a-b--cdef-g-";
    Core::replaceChar(result, L'-', L'+');
    UTEST_EQU(result, L"+a+b++cdef+g+");
    
    result = L"abcdefg";
    Core::replaceChar(result, L'-', L'+');
    UTEST_EQU(result, L"abcdefg");
    
    result = L"---";
    Core::replaceChar(result, L'-', L'+');
    UTEST_EQU(result, L"+++");
    
    result = L"";
    Core::replaceChar(result, L'-', L'+');
    UTEST_EQU(result, L"");
    
    UTEST_END_TEST;   
}

static void testStringTrim()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Trim Left/Right");
    String source1(L"   abc d  def  ");
    String source2(L"abc d  def");
    String source3(L" abc d  def ");
    String source4(L"  ");
    String source5(L"123abc d  def12");
    String result;
    
    Core::trimLeft(source1, result, L" ");
    UTEST_EQU(result, L"abc d  def  ");
    Core::trimRight(source1, result, L" ");
    UTEST_EQU(result, L"   abc d  def");
    
    Core::trimLeft(source2, result, L" ");
    UTEST_EQU(result, L"abc d  def");
    Core::trimRight(source2, result, L" ");
    UTEST_EQU(result, L"abc d  def");
    
    Core::trimLeft(source3, result, L" ");
    UTEST_EQU(result, L"abc d  def ");
    Core::trimRight(source3, result, L" ");
    UTEST_EQU(result, L" abc d  def");
    
    Core::trimLeft(source4, result, L" ");
    UTEST_EQU(result, L"");
    Core::trimRight(source4, result, L" ");
    UTEST_EQU(result, L"");
    
    Core::trimLeft(source5, result, L"123");
    UTEST_EQU(result, L"abc d  def12");
    Core::trimRight(source5, result, L"123");
    UTEST_EQU(result, L"123abc d  def");
    
    UTEST_END_TEST;   
}

static void testStringTrimDoubles()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Trim doubles");
    
    String source1(L"a bb  ccc     dddd");
    String source2(L"  a bbb  c     d  ");
    String source3(L" abcd ");
    String result;
    
    Core::trim(source1, result, L" ");
    UTEST_EQU(result, L"a bb ccc dddd");

    Core::trim(source2, result, L" ");
    UTEST_EQU(result, L" a bbb c d ");

    Core::trim(source3, result, L" ");
    UTEST_EQU(result, L" abcd ");

    UTEST_END_TEST;   
}

void testString()
{
    UTEST_PACKAGE("String");

    testStringInit();
    testStringUTF8();
    testStringCat();
    testStringUpDown();
    testStringStartEnds();
    testSlice();
    testFormat();
    testStringTokenize();
    testReplaceChar();
    testStringTrim();
    testStringTrimDoubles();
}
