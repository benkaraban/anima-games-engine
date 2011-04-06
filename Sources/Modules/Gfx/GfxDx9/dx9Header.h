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
#ifndef DX9HEADER_H_
#define DX9HEADER_H_

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

//TODO GCC->VC2008
//ajout ligne
#include <Guiddef.h>
//fin TODO GCC->VC2008
#include <d3d9.h>
#include <dxerr9.h>
#include <D3d9types.h>
#include <D3DX9Shader.h>
#include <Gfx/GfxDx9/D3DUtils.h>
#include <Core/Logger.h>

#ifdef _MSC_VER
#define DXGetErrorString9 DXGetErrorString
#define DXGetErrorDescription9 DXGetErrorDescription
#endif

#define LM_HANDLE_D3D_ERROR(hresult, msg) \
    if (FAILED(hresult)) \
    { \
        /*LM_ASSERT(false);*/ \
        static const int32 MAX_ERROR_COUNT = 100; \
        static int32 errorCount = 0; \
        if(errorCount < MAX_ERROR_COUNT) \
        { \
            String errorMsg; \
            errorMsg << msg << L" : " << String(DXGetErrorString9(hresult)); \
            ERR << LM_LOG_TAG_FUNCTION << LM_LOG_TAG_LINE << L" : " << errorMsg << L"\n"; \
            errorCount ++; \
            if(errorCount == MAX_ERROR_COUNT) \
                ERR << L"Too many errors, logging of this error is suspended\n"; \
            throw Gfx::GfxException(errorMsg); \
        } \
    }

#define LM_HANDLE_D3D_ERROR_NOMSG(hresult) \
    if (FAILED(hresult)) \
    { \
        LM_ASSERT(false); \
        static const int32 MAX_ERROR_COUNT = 100; \
        static int32 errorCount = 0; \
        if(errorCount < MAX_ERROR_COUNT) \
        { \
            String errorMsg; \
            errorMsg << String(DXGetErrorDescription9(hresult)) << L" (" << String(DXGetErrorString9(hresult)) << L")"; \
            ERR << LM_LOG_TAG_FUNCTION << LM_LOG_TAG_LINE << L" : " << errorMsg << L"\n"; \
            errorCount ++; \
            if(errorCount == MAX_ERROR_COUNT) \
                ERR << L"Too many errors, logging of this error is suspended\n"; \
            throw Gfx::GfxException(errorMsg); \
        } \
    }

#define LM_D3D_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#endif /* DX9HEADER_H_ */
