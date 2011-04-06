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
#include <Core/Logger.h>
#include <Renderer/Common/Tools.h>
#include <Renderer/SM2/RenderToolsSM2.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
int64 estimatedMemoryConsommationForView(int32 width, int32 height, EMSAALevel msaa)
{
    return width * height * 4 * (5 + 2 * (1 << int32(msaa)));
}
//-----------------------------------------------------------------------------
int32 findAcceptableResolution(const Core::List<Resolution> & resolutions, int32 iRes, EMSAALevel maxMSAA, int64 totalVRAM, float ratio, EMSAALevel & msaa)
{
    bool isResSafe = false;

    if(iRes >= resolutions.size())
        iRes = -1;

    while(iRes >= 0 && !isResSafe)
    {
        const Resolution & curRes = resolutions[iRes];
        msaa = maxMSAA;

        while(int32(msaa) >= 0 && !isResSafe)
        {
            int64 estimatedViewMem = estimatedMemoryConsommationForView(curRes.width, curRes.height, msaa);
            float viewMemRatio = float(estimatedViewMem) / float(totalVRAM);
            isResSafe = (viewMemRatio < (1.0f - ratio));

            if(!isResSafe)
                msaa = EMSAALevel(int32(msaa) - 1);
        }

        if(!isResSafe)
            iRes --;
    }

    return iRes;
}
//-----------------------------------------------------------------------------
void autoConfig(const Ptr<Gfx::IDriver> & pDriver, int32 iAdapter, RendererSettings & settings, const ConfigProfile & profile)
{
    Core::List<Resolution> allRes;
    Core::List<Resolution> goodRes;

    Gfx::DisplayMode initialMode = pDriver->getAdapterDisplayMode(iAdapter);
    Resolution initialRes(initialMode.width, initialMode.height);

    filterResolutions(pDriver->getDisplayFormats(iAdapter), initialMode, allRes, goodRes, false);
    filterResolutions(goodRes, profile.minWidth, profile.minHeight);
    filterResolutions(allRes, profile.minWidth, profile.minHeight);

    const Gfx::GraphicCapabilities & caps = pDriver->getGraphicCaps(iAdapter);
    int64 totalVRAM = caps.videoMem;
    bool isResSafe = false;
    EMSAALevel msaa = profile.msaaLevel;

    int32 iRes = std::find(goodRes.begin(), goodRes.end(), initialRes) - goodRes.begin();
    int32 iResOK = findAcceptableResolution(goodRes, iRes, profile.msaaLevel, totalVRAM, profile.textureVRAMRatio, msaa);

    if(iResOK != -1)
    {
        settings.width = goodRes[iResOK].width;
        settings.height = goodRes[iResOK].height;
        settings.msaaLevel = msaa;
    }
    else
    {
        iRes = std::find(allRes.begin(), allRes.end(), initialRes) - allRes.begin();
        iResOK = findAcceptableResolution(allRes, iRes, profile.msaaLevel, totalVRAM, profile.textureVRAMRatio, msaa);

        if(iResOK != -1)
        {
            WAR << L"Autoselected resolution do not preserve aspect ratio\n";
            settings.width = allRes[iResOK].width;
            settings.height = allRes[iResOK].height;
            settings.msaaLevel = msaa;
        }
        else
        {
            if(goodRes.size() > 0)
            {
                WAR << L"No suitable resolution found, selecting lowest resolution with correct aspect ratio\n";
                settings.width = goodRes[0].width;
                settings.height = goodRes[0].height;
                settings.msaaLevel = MSAA_NONE;
            }
            else if(allRes.size() > 0)
            {
                WAR << L"No suitable resolution found, selecting lowest resolution (bad aspect ratio is possible)\n";
                settings.width = allRes[0].width;
                settings.height = allRes[0].height;
                settings.msaaLevel = MSAA_NONE;
            }
            else
            {
                WAR << L"No suitable resolution found, selecting initial resolution (crash is highly probable)\n";
                settings.width = initialMode.width;
                settings.height = initialMode.height;
                settings.msaaLevel = MSAA_NONE;
            }
        }
    }

    INF << L"Autoconfig resolution is " << Core::toString(settings.width) << L"x" 
        << Core::toString(settings.height) << L" with " << Renderer::toString(settings.msaaLevel) << L"\n";
}
//-----------------------------------------------------------------------------
}
