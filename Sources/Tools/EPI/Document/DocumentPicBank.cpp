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
#include "DocumentPicBank.h"

#include <EPI/ImportInfo/ImportPicBankInfo.h>
#include <EPI/Document/Properties/PtyPicBank.h>

#include <EPI/Document/GuiDescription.h>

#include <Universe/World.h>

namespace EPI
{
//-----------------------------------------------------------------------------
DocumentPicBank::DocumentPicBank(GuiDocument & guiGoc, const Ptr<DocumentRenderer> & pDocRdr, const Ptr<ImportPicBankInfo> & pPicBankInfo)
:   DocumentBase(guiGoc, DOC_PIC_BANK, pDocRdr),
    _picBankName(pPicBankInfo->picBankName)
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
        pWorld->setFogColor(Core::Vector4f(0.f, 0.f, 0.f, 0.f));
        pWorld->setFogRange(10000.f, 10000.f);
        pWorld->setFogSkyOpacity(0);

    Ptr<Universe::RessourcePool> pResPool = pWorld->getRessourcesPool();
    Ptr<Assets::PictureBank> pPBK(new Assets::PictureBank());
    pResPool->getVFS()->read(pPicBankInfo->picBankName, *pPBK);

    const Core::List<Ptr<Assets::Texture> > & textures = pPBK->getTextureList();

    static const float DECAL_SIZE       = 1.0f;
    static const float DECAL_PADDING    = 0.2f;
    static const float DECAL_TOTAL_SIZE = DECAL_SIZE + DECAL_PADDING;

    int32 xCount = int32(Core::L_ceil(Core::L_sqrt(float(textures.size()))));
    int32 yCount = int32(Core::L_ceil(float(textures.size()) / float(xCount)));

    float xMid = float(xCount - 1);
    float yMid = float(yCount - 1);

    for(int32 ii=0; ii < textures.size(); ii++)
    {
        int32 y = ii / xCount;
        int32 x = ii % xCount;

        Core::Vector3f pos((float(x) - xMid) * DECAL_TOTAL_SIZE,
                           (float(y) - yMid) * -DECAL_TOTAL_SIZE,
                           0.0f);

        String picName(L"$PBK" + Core::toString(ii, 4));
        pResPool->addTexture(picName, textures[ii]);

        Ptr<Universe::NodeDecal> pDecal = getWorld()->createDecal(picName, Renderer::DECAL_LERP);
        pDecal->setLocalPosition(pos);
        pDecal->setSize(DECAL_SIZE, DECAL_SIZE);
        pDecal->setBoundingBoxVisible(true);
    }

    Ptr<Property> pPropertyPicBank(new PtyPicBank(getWorldDocument(), pPicBankInfo, L"PicBank"));
    
    getPropertyDocumentContent()->addChild(pPropertyPicBank);
    selectProperty(pPropertyPicBank);
    setGenerationProperty(pPropertyPicBank);
}
//-----------------------------------------------------------------------------
DocumentPicBank::~DocumentPicBank()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentPicBank::getDefaultProperty() const
{
    return getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
}  // namespace EPI
