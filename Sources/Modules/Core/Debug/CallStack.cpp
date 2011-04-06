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
#include <Core/Debug/CallStack.h>
#include <Core/Debug/StackWalker.h>
#include <Core/RT/Mutex.h>
#include <Core/Logger.h>

 #include <string.h>


namespace Core
{
#if defined _MSC_VER

class StackWalkerAG : public StackWalker
{
public:
    static const int32 MAX_LENGTH = 64 << 10;

    StackWalkerAG(int32 ignoreCount) : _count(0), _ignoreCount(ignoreCount), _mainReached(false)
    {
        reset();
    };

    void reset()
    {
        strcpy_s(_result, MAX_LENGTH, "");
        _count = 0;
        _mainReached = false;
    };

    const char * getCallStack() const { return _result; };

protected:
    virtual void OnOutput(LPCSTR szText)
    {
        if(!_mainReached)
        {
            strncat_s(_result, MAX_LENGTH, szText, strlen(szText));
        }
    }

    virtual void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName)
    {
        // ignored
    }

    virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion)
    {
        // ignored
    }

    virtual void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry)
    {
        if(_count >= _ignoreCount)
        {
            StackWalker::OnCallstackEntry(eType, entry);
        }

        if(strcmp(entry.name, "main") == 0 || strcmp(entry.name, "WinMain") == 0)
            _mainReached = true;

        _count ++;
    }

    virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
    {
        // ignored
    }

    char      _result[MAX_LENGTH + 1];
    int       _count;
    int       _ignoreCount;
    bool      _mainReached;
};
#endif/*#if defined _MSC_VER*/

void getCallStackLowLevel(char * pString, int32 maxCount)
{
#if defined(_DEBUG) && defined(_MSC_VER)
    static Core::Mutex sMutex;
    {
        Core::ScopedLock lock(sMutex);
        {
            static StackWalkerAG sw(2);
            sw.reset();
            sw.ShowCallstack();
            strncpy_s(pString, maxCount, sw.getCallStack(), _TRUNCATE);
        }
    }
#else
    strncpy(pString, "<no callstack>", maxCount);
#endif
}

void getCallStack(String & cs)
{
    static const int32 MAX_LENGTH = 64 << 10;
    char buffer[MAX_LENGTH];
    getCallStackLowLevel(buffer, MAX_LENGTH);
    cs = String(String8(buffer));
}

void dumpSystemExceptionCallStack(void * pException)
{
#if defined _MSC_VER
    static const int32 MAX_LENGTH = 64 << 10;
    char buffer[MAX_LENGTH];
    getCallStackLowLevel(buffer, MAX_LENGTH);

    StackWalkerAG sw(0);
    sw.ShowCallstack(GetCurrentThread(), (CONTEXT*)pException);
    printf(sw.getCallStack());
    ERR << String(String8(sw.getCallStack())) << L"\n";
#else
    ERR << L"<no callstack>\n";
#endif
}

}
