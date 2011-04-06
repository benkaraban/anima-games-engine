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
#ifdef _WIN32
#include <winsafe.h>
#endif

#include <Core/Logger.h>
#include <Core/Debug/Assert.h>
#include <Core/System/Files.h>
#include "UTests.h"

namespace UTest
{ 
//-----------------------------------------------------------------------------
void UTManager::setError()
{
    _isCurrentTestOK = false;
    _allTestsOK = false;
}
//-----------------------------------------------------------------------------
void UTManager::changeDir()
{
}
//-----------------------------------------------------------------------------
void UTManager::startExecution(int argc, char *argv[])
{
    try
    {
        String execDir(LM_WORKING_DIRECTORY);
        Core::System::setCurrentDir(execDir);
        
        Core::Assert::activateAssertBreak(true);
        
        for(int32 iArg=1; iArg < argc; iArg++)
        {
            String8 arg8(argv[iArg]);
            String  arg(arg8);
            if(arg == L"-noassertbreak")
                Core::Assert::activateAssertBreak(false);
            else
            {
                String strLine;
                strLine << L"WARNING : unknwon argument " << arg << L"\n";
                UTest::UTManager::getInstance().print(strLine);
            }
        }
    }
    catch(Core::SystemException & exception)
    {
        String strLine;
        strLine << L"ERROR : " << exception.toString() << L"\n";
        UTest::UTManager::getInstance().print(strLine);
    }
}
//-----------------------------------------------------------------------------
void UTManager::endExecution()
{
    try
    {
        String execDir(LM_WORKING_DIRECTORY);
        execDir << L"/Data/TUData/Tmp";
        Core::System::setCurrentDir(execDir);
            
        Core::List<String> files;
        Core::List<String> dirs;
        Core::System::getDirContent(L".", files, dirs);
        for(Core::List<String>::const_iterator iFile = files.begin(); iFile != files.end(); ++iFile)
            Core::System::deleteFile(*iFile);
    
        for(Core::List<String>::const_iterator iDir = dirs.begin(); iDir != dirs.end(); ++iDir)
        {
            if(iDir->length() > 0 && (*iDir)[0] != L'.')
            {
                Core::System::deleteFile(*iDir);
            }
        }

        if(!_allTestsOK)
            system("pause");
    }
    catch(Core::SystemException & exception)
    {
        String strLine;
        strLine << L"ERROR : " << exception.toString() << L"\n";
        UTest::UTManager::getInstance().print(strLine);
    }
}
//-----------------------------------------------------------------------------
void UTManager::startTest(const Core::String & testName)
{
    if(_isInTest)
    {
        String strLine;
        strLine << L"TEST_ERROR : unable to start unit test '" << testName
                << L"', unit test '" << _currentTestName << L"' is already runing.\n";
        UTest::UTManager::getInstance().print(strLine);
    }
    else
    {
        _currentTestName = testName;
        _isCurrentTestOK = true;
        _isInTest = true;
    }
}
//-----------------------------------------------------------------------------
void UTManager::endTest()
{
    if(!_isInTest)
    {
        String strLine;
        strLine << L"TEST_ERROR : unable to end unit test, no one is running right now. " 
                << L"Las run was '" << _currentTestName << L"'\n";
        UTest::UTManager::getInstance().print(strLine);
    }
    else
    {
        Core::String resultString(_currentTestName);
        resultString += L"...";
        resultString.resize(RESULT_COLUMN_NUMBER, L' ');
        if(_isCurrentTestOK)
        {
            resultString << L"[  OK  ]\n"; 
        }
        else
        {
            resultString << L"[FAILED]\n"; 
            _allTestsOK = false;
        }
        
        UTest::UTManager::getInstance().print(resultString);
        _isInTest = false;
    }    
}
//-----------------------------------------------------------------------------
void UTManager::print(const Core::String & message)
{
    std::wcout << message.c_str() << std::flush;
#ifdef _MSC_VER
    OutputDebugStringW(message.c_str());
#endif
}
//-----------------------------------------------------------------------------
void UTManager::error(const Core::String & message)
{
    print(message);
    setError();
}
//-----------------------------------------------------------------------------
UTManager & UTManager::getInstance()
{
    static UTManager sInstance;
    return sInstance;
}
//-----------------------------------------------------------------------------
UTManager::UTManager() : 
    _isInTest(false), 
    _currentTestName("NO_TEST"), 
    _isCurrentTestOK(false),
    _allTestsOK(true)
{
}
//-----------------------------------------------------------------------------
}
