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
#include <intrin.h>
#else
#endif

#include <Core/Strings/Tools.h>
#include "Files.h"

namespace Core
{
namespace System
{
#ifdef _WIN32
//-----------------------------------------------------------------------------
void errorMessageBox(const String & message, void * pHandle)
{
    MessageBoxW((HWND)pHandle, message.c_str(), NULL, MB_OK | MB_ICONERROR);
}
//-----------------------------------------------------------------------------
bool retryCancelMessageBox(const String & message, void * pHandle)
{
    return MessageBoxW((HWND)pHandle, message.c_str(), NULL, MB_RETRYCANCEL | MB_ICONERROR) == IDRETRY;
}
//-----------------------------------------------------------------------------
String getClipboardContent()
{
    String result;

    if(OpenClipboard(NULL))
    {
        if(IsClipboardFormatAvailable(CF_UNICODETEXT))
        {
            HGLOBAL hclip = GetClipboardData(CF_UNICODETEXT);
            if(hclip != NULL)
            {
                const wchar_t * pText = reinterpret_cast<const wchar_t *>(GlobalLock(hclip));
                if(pText != NULL)
                {
                    result = pText;
                    GlobalUnlock(hclip);
                }
            }
        }
        else if(IsClipboardFormatAvailable(CF_TEXT))
        {
            HGLOBAL hclip = GetClipboardData(CF_TEXT);
            if(hclip != NULL)
            {
                const char * pText = reinterpret_cast<const char *>(GlobalLock(hclip));
                if(pText != NULL)
                {
                    result = String(String8(pText));
                    GlobalUnlock(hclip);
                }
            }
        }
        CloseClipboard();
    }

    return result;
}
//-----------------------------------------------------------------------------
void setClipboardContent(const String & content)
{
    if(OpenClipboard(NULL))
    {
        EmptyClipboard();

        int32 size = (content.size() + 1) * sizeof(wchar_t);
        HGLOBAL hclip = GlobalAlloc(GMEM_MOVEABLE, size); 

        if(hclip != NULL)
        {
            void * pData = GlobalLock(hclip);
            
            if(pData != NULL)
            {
                memcpy(pData, content.data(), size);
                GlobalUnlock(hclip);
            }
            
            SetClipboardData(CF_UNICODETEXT, hclip);
        }
        CloseClipboard();
    }
}
//-----------------------------------------------------------------------------
String getKeyCodeName(int32 keyVCode)
{
    wchar_t buffer[20 + 1];
    int32 scanCode = MapVirtualKeyW(keyVCode, 0);
    // because MapVirtualKey strips the extended bit for some keys
    switch (keyVCode)
    {
        case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
        case VK_PRIOR: case VK_NEXT: // page up and page down
        case VK_END: case VK_HOME:
        case VK_INSERT: case VK_DELETE:
        case VK_DIVIDE: // numpad slash
        case VK_NUMLOCK:
        case VK_RCONTROL: case VK_RMENU:
        {
            scanCode |= 0x100; // set extended bit
            break;
        }
    }

    scanCode = scanCode << 16;
    int32 length = GetKeyNameTextW(scanCode, buffer, 20);
    if(length > 0)
        return String(buffer);

    return L"";
}

int32 scanCodeToVK(int32 scanCode)
{
    int32 vk = 0;

    while(vk < 256)
    {
        if(MapVirtualKeyW(vk, 0) == scanCode)
            return vk;

        vk++;
    }

    return -1;
}
//-----------------------------------------------------------------------------
ELanguage getSystemSupportedLanguage()
{
    ELanguage language = Core::ENGLISH;
    LANGID languageID = ::GetSystemDefaultLangID();
    if( languageID == 0x080c //French (fr), Belgium (BE)
    ||  languageID == 0x0C0C //French (fr), Canada (CA)
    ||  languageID == 0x040c //French (fr), France (FR)
    ||  languageID == 0x140C //French (fr), Luxembourg (LU)
    ||  languageID == 0x180C //French (fr), Monaco (MC)
    ||  languageID == 0x100C)//French (fr), Switzerland (CH)
    {
        language = Core::FRENCH;
    }
    else
    {
        language = Core::ENGLISH;
    }
    return language;
}
//-----------------------------------------------------------------------------
String getOSName()
{
    String result(L"Windows ");
    OSVERSIONINFOEXW infos;
    ZeroMemory(&infos, sizeof(infos));
    infos.dwOSVersionInfoSize = sizeof(infos);
    
    //http://msdn.microsoft.com/en-us/library/ms724833%28v=VS.85%29.aspx
    if(GetVersionExW((OSVERSIONINFOW*)&infos))
    {
        if(infos.dwMajorVersion == 6 && infos.dwMinorVersion == 1)
        {
            if(infos.wProductType == VER_NT_WORKSTATION)
                result << L"7";
            else
                result << L"Server 2008 R2";
        }
        else if(infos.dwMajorVersion == 6 && infos.dwMinorVersion == 0)
        {
            if(infos.wProductType == VER_NT_WORKSTATION)
                result << L"Vista";
            else
                result << L"Server 2008";
        }
        else if(infos.dwMajorVersion == 5 && infos.dwMinorVersion == 2)
        {
            if(GetSystemMetrics(SM_SERVERR2) != 0)
                result << L"Server 2003 R2";
            else
                result << L"Server 2003";
        }
        else if(infos.dwMajorVersion == 5 && infos.dwMinorVersion == 1)
        {
            result << L"XP";
        }
        else if(infos.dwMajorVersion == 5 && infos.dwMinorVersion == 0)
        {
            result << L"2000";
        }
        else
        {
            result << L"???";
        }

        String servicePack(String8(infos.szCSDVersion));

        if(!servicePack.empty())
            result << L" " << servicePack;

        result << L" build "
            << Core::toString(int32(infos.dwMajorVersion)) << L"."
            << Core::toString(int32(infos.dwMinorVersion)) << L"."
            << Core::toString(int32(infos.dwBuildNumber)) << L" SP "
            << Core::toString(int32(infos.wServicePackMajor)) << L"."
            << Core::toString(int32(infos.wServicePackMinor));
    }
    else
    {
        result << L"!version unavailable!";
    }

    return result;
}
//-----------------------------------------------------------------------------
int32 getCPUCount()
{
    SYSTEM_INFO infos;
    ZeroMemory(&infos, sizeof(infos));
    GetSystemInfo(&infos);
    return int32(infos.dwNumberOfProcessors);
}
//-----------------------------------------------------------------------------
int64 getTotalPhysicalRAM()
{
    MEMORYSTATUSEX status;
    ZeroMemory(&status, sizeof(status));
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return int64(status.ullTotalPhys);
}
//-----------------------------------------------------------------------------
int64 getTotalFileSwapRAM()
{
    MEMORYSTATUSEX status;
    ZeroMemory(&status, sizeof(status));
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return int64(status.ullTotalPageFile);
}
//-----------------------------------------------------------------------------
int64 getTotalVirtualRAM()
{
    MEMORYSTATUSEX status;
    ZeroMemory(&status, sizeof(status));
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return int64(status.ullTotalVirtual);
}
//-----------------------------------------------------------------------------
String getCPUName()
{
// Get extended ids.
    int CPUInfo[4] = {-1};
    __cpuid(CPUInfo, 0x80000000);
    unsigned int nExIds = CPUInfo[0];

    // Get the information associated with each extended ID.
    char CPUBrandString[0x40] = { 0 };
    for( unsigned int i=0x80000000; i<=nExIds; ++i)
    {
        __cpuid(CPUInfo, i);

        // Interpret CPU brand string and cache information.
        if  (i == 0x80000002)
        {
            memcpy( CPUBrandString,
            CPUInfo,
            sizeof(CPUInfo));
        }
        else if( i == 0x80000003 )
        {
            memcpy( CPUBrandString + 16,
            CPUInfo,
            sizeof(CPUInfo));
        }
        else if( i == 0x80000004 )
        {
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
    }
    return String(String8(CPUBrandString));
}
//-----------------------------------------------------------------------------
bool launchDefaultInternetNavigator(const String & httpAddress)
{
    LM_ASSERT(httpAddress.startsWith(L"http://"));

    HINSTANCE returnCode = ShellExecuteW(NULL, L"open", httpAddress.c_str(), NULL, NULL, SW_SHOWNORMAL);

    return (((int32)returnCode) >= 32);
}
//-----------------------------------------------------------------------------
uint64 setThreadCPUAffinity(uint64 mask)
{
    return 0;
}
//-----------------------------------------------------------------------------
#else
//-----------------------------------------------------------------------------
void errorMessageBox(const String & message)
{
}
//-----------------------------------------------------------------------------
String getClipboardContent(void * hWnd)
{
    return L"<no clipboard>";
}
//-----------------------------------------------------------------------------
void setClipboardContent(void * hWnd, const String & content)
{
}
//-----------------------------------------------------------------------------
int32 getCPUCount()
{
    //En cas d'erreur la méthode retourne -1...
    //Or on risque difficilement d'avoir moins d'un coeur...
    return std::max(1, int32(sysconf(_SC_NPROCESSORS_ONLN)));
}
//-----------------------------------------------------------------------------
#endif
} // System
} // Core
