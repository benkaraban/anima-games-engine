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
#ifndef UTESTS_H_
#define UTESTS_H_

#include <math.h>
#include <iostream>
#include <Core/String.h>
#include <Core/Math/MathTools.h>

namespace UTest
{
const int RESULT_COLUMN_NUMBER = 45;

class LM_API_TUT UTManager
{
public:
    void startExecution(int argc, char *argv[]);
    void endExecution();
        
    void startTest(const Core::String & testName);
    void endTest();
    void setError();
    void print(const Core::String & message);
    void error(const Core::String & message);
    
    static UTManager & getInstance();
    
private:
    UTManager();
    void changeDir();
    
    bool            _isInTest;
    Core::String    _currentTestName;
    bool            _isCurrentTestOK;
    bool            _allTestsOK;
};

}

#define UTEST_PACKAGE(packageName) \
    { Core::String strPackName(L"\n"); strPackName \
        << L"-------------------------------------------------------------------------------\n" \
        << L"    " << Core::String(packageName) << L"\n" \
        << L"-------------------------------------------------------------------------------\n"; \
    UTest::UTManager::getInstance().print(strPackName); }

#ifdef _DEBUG

#define UTEST_START_TEST(testName) \
    try { \
        UTest::UTManager::getInstance().startTest(testName);

#define UTEST_END_TEST \
        UTest::UTManager::getInstance().endTest(); \
    } \
    catch(Core::Exception & exception) \
    { \
        Core::String strError; \
        strError << L"TEST_ERROR : unexpected exception caught : " \
                 << exception.toString() << L"\n"; \
        UTest::UTManager::getInstance().print(strError); \
        UTest::UTManager::getInstance().setError(); \
        UTest::UTManager::getInstance().endTest(); \
    } \
    catch(...) \
    { \
        UTest::UTManager::getInstance().print(Core::String(L"TEST_ERROR : unexpected exception caught.\n")); \
        UTest::UTManager::getInstance().setError(); \
        UTest::UTManager::getInstance().endTest(); \
    }

#define UTEST_END_TEST_NO_EXCEPTION \
        UTest::UTManager::getInstance().endTest(); \
    } \
    catch(...) \
    { \
        UTest::UTManager::getInstance().print(Core::String(L"TEST_ERROR : unexpected exception caught.\n")); \
        UTest::UTManager::getInstance().setError(); \
        UTest::UTManager::getInstance().endTest(); \
    }

#define UTEST_EQU(x, y) \
    if((x) != (y)) \
    { \
        Core::String strError; \
        strError << L"TEST_ERROR : " << Core::String(#x) << L" != " << Core::String(#y) \
                 << L" (" << Core::String(#x) << L"=" << Core::toString((x)) << L", " << Core::String(#y) \
                 << L"=" << Core::toString((y)) << L") at " << Core::String(__FILE__) << L":" << Core::toString(__LINE__) \
                 << L" in " << Core::String(__FUNCTION__) << L"\n"; \
        UTest::UTManager::getInstance().error(strError); \
    } 

#define UTEST_ASSERT(condition, message) \
    if(!(condition)) \
    { \
        Core::String strError; \
        strError << L"TEST_ERROR : " << Core::String(message) \
                 << L" at " << Core::String(__FILE__) << L":" << Core::toString(__LINE__) \
                 << L" in " << Core::String(__FUNCTION__) << L"\n"; \
        UTest::UTManager::getInstance().error(strError); \
    }

#define UTEST_NOT_EQU(x, y) \
    if((x) == (y)) \
    { \
        Core::String strError; \
        strError << L"TEST_ERROR : " << Core::String(#x) << L" == " << Core::String(#y) \
                 << L" (" << Core::String(#x) << L"=" << Core::toString((x)) << L", " << Core::String(#y) \
                 << L"=" << Core::toString((y)) << L") at " << Core::String(__FILE__) << L":" << Core::toString(__LINE__) \
                 << L" in " << Core::String(__FUNCTION__) << L"\n"; \
        UTest::UTManager::getInstance().error(strError); \
    } 

#define UTEST_EQUE(x, y, epsilon) \
    if(Core::L_abs((x) - (y)) > epsilon) \
    { \
        Core::String strError; \
        strError << L"TEST_ERROR : " << Core::String(#x) << L" != " << Core::String(#y) \
                 << L" (" << Core::String(#x) << L"=" << Core::toString((x)) << L", " << Core::String(#y) \
                 << L"=" << Core::toString((y)) << L", espilon=" << Core::toString(epsilon) << L") at " << Core::String(__FILE__) \
                 << L":" << Core::toString(__LINE__) << L" in " << Core::String(__FUNCTION__) << L"\n"; \
        UTest::UTManager::getInstance().error(strError); \
    }

#else

#define UTEST_START_TEST(testName) \
    { \
        UTest::UTManager::getInstance().startTest(testName);

#define UTEST_END_TEST \
        UTest::UTManager::getInstance().endTest(); \
    }

#define UTEST_END_TEST_NO_EXCEPTION \
        UTest::UTManager::getInstance().endTest(); \
    }

#define UTEST_EQU(x, y) \
    if((x) != (y)) \
    { \
    UTest::UTManager::getInstance().error(Core::String("TEST_ERROR : "##__FILE__##":"##UTEST_STRING(__LINE__)##"\n")); \
    } 

#define UTEST_ASSERT(condition, message) \
    if(!(condition)) \
    { \
    UTest::UTManager::getInstance().error(Core::String("TEST_ERROR : "##__FILE__##":"##UTEST_STRING(__LINE__)##"\n")); \
    }

#define UTEST_NOT_EQU(x, y) \
    if((x) == (y)) \
    { \
    UTest::UTManager::getInstance().error(Core::String("TEST_ERROR : "##__FILE__##":"##UTEST_STRING(__LINE__)##"\n")); \
    } 

#define UTEST_EQUE(x, y, epsilon) \
    if(Core::L_abs((x) - (y)) > epsilon) \
    { \
    UTest::UTManager::getInstance().error(Core::String("TEST_ERROR : "##__FILE__##":"##UTEST_STRING(__LINE__)##"\n")); \
    }

#endif

#define UTEST_STRING_(x) #x

#define UTEST_STRING(x) UTEST_STRING_(x)

#define UTEST_TEST_FAILED \
    UTest::UTManager::getInstance().setError();
    
#define UTEST_CATCH_EXCEPTION(exceptionType, code) \
    try { \
        code; \
        UTest::UTManager::getInstance().error(Core::String(L"TEST_ERROR : expected exception NOT caught.\n")); \
    } \
    catch(exceptionType &) \
    {} \
    catch(Core::Exception & exception) \
    { \
        UTest::UTManager::getInstance().error(Core::String(L"TEST_ERROR : unexpected exception caught.") + exception.toString() + L"\n"); \
    } \
    catch(...) \
    { \
        UTest::UTManager::getInstance().error(Core::String(L"TEST_ERROR : unexpected exception caught.\n")); \
    }

#define UTEST_CATCH_CORE_EXCEPTION(code) \
    try { \
        code; \
        UTest::UTManager::getInstance().error(Core::String(L"TEST_ERROR : expected exception NOT caught.\n")); \
    } \
    catch(Core::Exception &) \
    {} \
    catch(...) \
    { \
        UTest::UTManager::getInstance().error(Core::String(L"TEST_ERROR : unexpected exception caught.\n")); \
    }

#define UTEST_EQUF(x, y) UTEST_EQUE(x, y, 0.0001)

#endif /*UTESTS_H_*/
