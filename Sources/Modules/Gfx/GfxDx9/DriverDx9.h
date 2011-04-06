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
#ifndef DRIVERDX9_H_
#define DRIVERDX9_H_

#include <winsafe.h>
#include <Gfx/GfxDx9/DeviceDx9.h>
#include <Gfx/IDriver.h>

namespace Gfx
{
class LM_API_GX9 DriverDx9: public IDriver
{
public:
    DriverDx9();

    virtual ~DriverDx9();
    virtual const Core::List<DisplayMode> & getDisplayFormats(int32 adapterId) const;

    virtual void createDevice(int32 adapterId, EVertexProcessing vertexProcess, const SwapChainDesc& scDesc,
                              ISwapChainPtr& pSC, IDevicePtr& pDevice, int32 flags = 0);

    virtual DisplayMode getAdapterDisplayMode(int32 adapterId);

protected:

    void initDisplayModeList();
    void initAdapters();

protected:
    bool checkMSAA(int32 id, D3DMULTISAMPLE_TYPE t, int & maxQuality);

    LPDIRECT3D9 _pD3D;
    Core::List<D3DCAPS9> _capsD3D;
    Core::List<Core::List<DisplayMode> > _displayModes;
};
typedef Core::Ptr<DriverDx9> DriverDx9Ptr;
} // namespace Gfx

namespace Core
{
inline String toString(const D3DCAPS9 & c)
{
    return String::EMPTY;
};
};

#endif /* DRIVERDX9_H_ */
