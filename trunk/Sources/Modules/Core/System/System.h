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
#ifndef CORE_SYSTEM_H_
#define CORE_SYSTEM_H_

#include <Core/Standard.h>
#include <Core/Exception.h>
#include <Core/Dictionary.h>

namespace Core
{
class SystemException : public Exception
{
public:
    SystemException(const String & message) : Exception(message)
    {};
};
namespace System
{
LM_API_COR void errorMessageBox(const String & message, void * pHandle = NULL);
LM_API_COR bool retryCancelMessageBox(const String & message, void * pHandle = NULL);

LM_API_COR String getOSName();
LM_API_COR String getCPUName();
LM_API_COR int32 getCPUCount();
LM_API_COR uint64 setThreadCPUAffinity(uint64 mask);

LM_API_COR int64 getTotalPhysicalRAM();
LM_API_COR int64 getTotalFileSwapRAM();
LM_API_COR int64 getTotalVirtualRAM();

LM_API_COR String getClipboardContent();
LM_API_COR void setClipboardContent(const String & content);

LM_API_COR String getKeyCodeName(int32 keyVCode);
LM_API_COR int32 scanCodeToVK(int32 scanCode);

LM_API_COR ELanguage getSystemSupportedLanguage();

LM_API_COR bool launchDefaultInternetNavigator(const String & httpAdress);

} // System
} // Core

#endif /*CORE_SYSTEM_H_*/
