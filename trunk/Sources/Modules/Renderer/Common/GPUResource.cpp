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
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
GPUResourceLib::GPUResourceLib() : _deviceLost(false)
{
}
//-----------------------------------------------------------------------------
void GPUResourceLib::registerResource(IGPUResource * pResource)
{
    _resources.push_back(pResource);
    if(!_deviceLost)
        pResource->onDeviceReset();
}
//-----------------------------------------------------------------------------
void GPUResourceLib::unregisterResource(IGPUResource * pResource)
{
    Core::List<IGPUResource *>::iterator iRes = std::find(_resources.begin(), _resources.end(), pResource);
    if(iRes != _resources.end())
        _resources.erase(iRes);
    if(!_deviceLost)
        pResource->onDeviceLost();
}
//-----------------------------------------------------------------------------
void GPUResourceLib::onDeviceLost()
{
    _deviceLost = true;
    for(int32 ii=0; ii < _resources.size(); ii++)
        _resources[ii]->onDeviceLost();
}
//-----------------------------------------------------------------------------
void GPUResourceLib::onDeviceReset()
{
    _deviceLost = false;
    for(int32 ii=0; ii < _resources.size(); ii++)
        _resources[ii]->onDeviceReset();
}
//-----------------------------------------------------------------------------
void GPUResourceLib::onTextureLevelChange(ETextureLevel level)
{
    for(int32 ii=0; ii < _resources.size(); ii++)
        _resources[ii]->onTextureLevelChange(level);
}
//-----------------------------------------------------------------------------
void GPUResourceLib::onFullscreenChange(bool fullscreen)
{
    for(int32 ii=0; ii < _resources.size(); ii++)
        _resources[ii]->onFullscreenChange(fullscreen);
}
//-----------------------------------------------------------------------------
}
