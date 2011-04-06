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
#include <Core/Standard.h>
#include <d3d9.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
String getErrorMessage(int32 id)
{
    String result;
    #define CASE_ERROR(err) { case err: result = String(#err); break; };

    switch(id)
    {
    CASE_ERROR(D3D_OK);
    CASE_ERROR(D3DOK_NOAUTOGEN);
    CASE_ERROR(D3DERR_CONFLICTINGRENDERSTATE);
    CASE_ERROR(D3DERR_CONFLICTINGTEXTUREFILTER);
    CASE_ERROR(D3DERR_CONFLICTINGTEXTUREPALETTE);
    CASE_ERROR(D3DERR_DEVICELOST);
    CASE_ERROR(D3DERR_DEVICENOTRESET);
    CASE_ERROR(D3DERR_DRIVERINTERNALERROR);
    CASE_ERROR(D3DERR_DRIVERINVALIDCALL);
    CASE_ERROR(D3DERR_INVALIDCALL);
    CASE_ERROR(D3DERR_INVALIDDEVICE);
    CASE_ERROR(D3DERR_MOREDATA);
    CASE_ERROR(D3DERR_NOTAVAILABLE);
    CASE_ERROR(D3DERR_NOTFOUND);
    CASE_ERROR(D3DERR_OUTOFVIDEOMEMORY);
    CASE_ERROR(D3DERR_TOOMANYOPERATIONS);
    CASE_ERROR(D3DERR_UNSUPPORTEDALPHAARG);
    CASE_ERROR(D3DERR_UNSUPPORTEDALPHAOPERATION);
    CASE_ERROR(D3DERR_UNSUPPORTEDCOLORARG);
    CASE_ERROR(D3DERR_UNSUPPORTEDCOLOROPERATION);
    CASE_ERROR(D3DERR_UNSUPPORTEDFACTORVALUE);
    CASE_ERROR(D3DERR_UNSUPPORTEDTEXTUREFILTER);
    CASE_ERROR(D3DERR_WASSTILLDRAWING);
    CASE_ERROR(D3DERR_WRONGTEXTUREFORMAT);
    CASE_ERROR(E_FAIL);
    CASE_ERROR(E_INVALIDARG);
    CASE_ERROR(E_NOINTERFACE);
    CASE_ERROR(E_NOTIMPL);
    CASE_ERROR(E_OUTOFMEMORY);
    default:
        result = L"unknown";
        break;
    }
    return result;
#undef CASE_ERROR
}
//-----------------------------------------------------------------------------
}
