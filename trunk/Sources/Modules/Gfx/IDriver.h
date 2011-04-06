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
#ifndef IDRIVER_H_
#define IDRIVER_H_

#include <Core/Standard.h>
#include <Core/List.h>

#include <Gfx/GfxType.h>
#include <Gfx/IDevice.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
class LM_API_GFX AdapterInformation
{
public:
    AdapterInformation(const String & name = L"", const String & information = L"");
    virtual ~AdapterInformation();
    const String & getName() const;
    const String & getInformation() const;

protected:
    String _name;
    String _information;
};
//-----------------------------------------------------------------------------
const int32 DEFAULT_ADAPTER = 0;
//-----------------------------------------------------------------------------
const int32 MULTITHREADED_DEVICE_FLAG = 0x00000001;
//-----------------------------------------------------------------------------
LM_ENUM_2(EVertexProcessing, HARDWARE_PROCESSING, SOFTWARE_PROCESSING);
//-----------------------------------------------------------------------------
class LM_API_GFX IDriver
{
public:
    virtual ~IDriver();

    ////////////////////////////////
    //  info materiel
    ////////////////////////////////
    int32 getAdapterCount() const
    {
        return _infos.size();
    };
    const AdapterInformation & getAdapterInfos(const int32 id) const
    {
        return _infos[id];
    };
    const GraphicCapabilities & getGraphicCaps(const int32 id) const
    {
        return _caps[id];
    };

    virtual const Core::List<DisplayMode> & getDisplayFormats(int32 adapterId) const = 0;

    virtual void createDevice(int32 adapterId, EVertexProcessing vertexProcess, const SwapChainDesc& scDesc,
                              ISwapChainPtr& pSC, IDevicePtr& pDevice, int32 flags = 0) = 0;

    virtual DisplayMode getAdapterDisplayMode(int32 adapterId) = 0;

protected:
    IDriver();

    Core::List<GraphicCapabilities> _caps;
    Core::List<AdapterInformation> _infos;
};
typedef Ptr<IDriver> IDriverPtr;
//-----------------------------------------------------------------------------

}//namespace Gfx

namespace Core
{
inline String toString(const Gfx::AdapterInformation& ai)
{
    String str;

    str = ai.getName();
    str +=L"\n" ;
    str+= ai.getInformation();

    return str;
}

} //namespace Core

#endif /* IDRIVER_H_ */
