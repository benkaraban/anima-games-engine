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
#include "DocumentTexture.h"

#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

#include <EPI/ImportInfo/ImportTextureInfo.h>
#include <EPI/Document/Properties/PtyTexture.moc.h>

#include <EPI/Document/GuiDescription.h>

#include <Universe/World.h>
namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DocumentTexture::DocumentTexture(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr, const Ptr<ImportTextureInfo> & pTextureInfo)
:   DocumentBase(guiGoc, DOC_TEXTURE, pDocRdr),
    _textureName(pTextureInfo->textureName)
{
    Ptr<GuiDescription> pGuiDesc(new GuiDescription());
        Core::List<EViewportLayoutMode> viewLayoutModes;
            viewLayoutModes.push_back(SINGLE);
            pGuiDesc->setViewportLayoutModes(viewLayoutModes);
        Core::List<EViewportCameraNavigation> viewCameraNavigations;
            viewCameraNavigations.push_back(VIEWPORT_CAMERA_ORBITAL);
            pGuiDesc->setViewportCameraNavigations(viewCameraNavigations);
    setGuiDescription(pGuiDesc);

    Ptr<Universe::World> pWorld = getWorldDocument();
        Renderer::MainLightSettings sun;
        sun.sunAngleToZenith    = 0;
        sun.sunAngleToNorth     = 0;
        sun.ambient             = Core::Vector4f(0.f, 0.f, 0.f, 0.f);
        sun.diffuse0            = Core::Vector4f(1.f, 1.f, 1.f, 1.f);
        sun.specular            = Core::Vector4f(0.f, 0.f, 0.f, 1.f);
        sun.mulIntensity                = 1.0;
        sun.staticShadowOpacity         = 0;
        sun.dynamicShadowOpacity        = 0;
        sun.dynamicShadowMiddle         = 0;
        sun.dynamicShadowMiddleBlend    = 0;
        sun.dynamicShadowEnd            = 0;
        sun.dynamicShadowEndBlend       = 0;
        sun.bloomThreshold      = 0;
        sun.bloomFactor         = 0;

        pWorld->setMainLightSettings(sun);
        pWorld->setFogColor(Core::Vector4f(0.f, 0.f, 0.f, 0.f));
        pWorld->setFogRange(10000.f, 10000.f);
        pWorld->setFogSkyOpacity(0);

    Ptr<Property> pPropertyTexture (new PtyTexture(getWorldDocument(), pTextureInfo, L"Texture"));
    
    getPropertyDocumentContent()->addChild(pPropertyTexture);
    selectProperty(pPropertyTexture);
    setGenerationProperty(pPropertyTexture);
}
//-----------------------------------------------------------------------------
DocumentTexture::~DocumentTexture()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentTexture::getDefaultProperty() const
{
    return getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
}  // namespace EPI
