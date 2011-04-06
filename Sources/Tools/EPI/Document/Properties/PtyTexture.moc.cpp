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
#include "PtyTexture.moc.h"

#include <QGridLayout>

#include <Universe/World.h>

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/CollapsibleWidget.moc.h>

#include <EPI/ImportInfo/ImportTextureInfo.h>
#include <EPI/GUI/Widget/TextureGenerationOptions.moc.h>

#include <Workflow/TexCompression.h>

#include <QLabel>

#include <EPI/Constants.h>

namespace EPI
{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

PtyTexture::PtyTexture( const Ptr<Universe::World>& pWorld,
                        const Ptr<ImportTextureInfo> & pImportTextureInfo, 
                        const Core::String& title)
:   PropertyNode(title, true, false, CONTENT),
    _pImportTextureInfo(pImportTextureInfo),
    _pWorld(pWorld)
{
    _pNodeDecal = _pWorld->createDecal(pImportTextureInfo->textureName, Renderer::DECAL_LERP);

    _pNodeDecal->beginMatrixUpdate();
    _pNodeDecal->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    _pNodeDecal->endMatrixUpdate();

    updateProperty();
}

PtyTexture::~PtyTexture()
{}

Ptr<PropertyWidget> PtyTexture::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetTexture> pPW (new PtyWidgetTexture(pDataProxy, _pImportTextureInfo, parent));
    return pPW;
}

void PtyTexture::updateProperty()
{
    _bcgColor = _pWorld->getFogColor();

    Ptr<Assets::Texture> pTexture = _pImportTextureInfo->pTexture;
    if (pTexture != null)
    {
        if (pTexture->getMipmapCount() == 0)
        {
            pTexture = _pWorld->getRessourcesPool()->getTextureData(_pImportTextureInfo->textureName);
        }

        float w     = pTexture->getWidth();
        float h     = pTexture->getHeight();
        float ratio = 1.f;

        if (h>0)
            ratio = w/h;

        _pNodeDecal->setSize(1, 1/ratio);
    }

    
}

void PtyTexture::updateData()
{
    _pWorld->setFogColor(_bcgColor);
}

Ptr<Property> PtyTexture::clone() const
{
    return Ptr<Property>(new PtyTexture( *this ));
}

void PtyTexture::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyTexture> pPty = LM_DEBUG_PTR_CAST<PtyTexture>(pSrc);

    _pWorld   = pPty->_pWorld;
    _bcgColor = pPty->_bcgColor;

    updateData();
}

void PtyTexture::generateTexture(const Ptr<Workflow::TextureOptions> & pTextureOptions)
{
    _pImportTextureInfo->pTextureOptions = pTextureOptions;

    emit generate(_pImportTextureInfo);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

PtyWidgetTexture::PtyWidgetTexture( const Ptr<PropertyWidgetDataProxy>& data, 
                                    const Ptr<ImportTextureInfo> & pImportTextureInfo, 
                                    QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
    _textureGenerationOptions->setTextureInfo(pImportTextureInfo);
}

PtyWidgetTexture::~PtyWidgetTexture()
{}

void PtyWidgetTexture::readProperty()
{
    Ptr<PtyTexture> pPtyTex = LM_DEBUG_PTR_CAST<PtyTexture>(getDataProxy()->getProperty());

    _backGroundColor->setColorLinear(pPtyTex->_bcgColor);

    updateInfo();
}

void PtyWidgetTexture::writeProperty(QWidget* pWidget)
{
    Ptr<PtyTexture> pPtyTex = LM_DEBUG_PTR_CAST<PtyTexture>(getDataProxy()->getProperty());

    _backGroundColor->getColorLinear(pPtyTex->_bcgColor);
}

void PtyWidgetTexture::setupUi()
{
    Ptr<PtyTexture> pPtyTex = LM_DEBUG_PTR_CAST<PtyTexture>(getDataProxy()->getProperty());

    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    

    _groupBoxDoc   = new QtToolbox::CollapsibleWidget(this, "World", false);
    _groupBoxTex   = new QtToolbox::CollapsibleWidget(this, "Texture", false);
    _groupBoxInfo  = new QtToolbox::CollapsibleWidget(this, "Information", false);

    _backGroundColor          = new QtToolbox::QuickColorPicker(this, "BackGround Color");
    _textureGenerationOptions = new TextureGenerationOptions(this, false);
    _width                    = new QLabel();
    _height                   = new QLabel();
    _size                     = new QLabel();

    _groupBoxDoc->getLayout()->addWidget(_backGroundColor);
    _groupBoxTex->getLayout()->addWidget(_textureGenerationOptions);
    _groupBoxInfo->getLayout()->addWidget(_width);
    _groupBoxInfo->getLayout()->addWidget(_height);
    _groupBoxInfo->getLayout()->addWidget(_size);

    _layout->addWidget(_groupBoxDoc);
    _layout->addWidget(_groupBoxTex);
    _layout->addWidget(_groupBoxInfo);


    getWidgetsForUndoRedo().push_back(_backGroundColor);

    PropertyWidget::setupUi();

    connect(_textureGenerationOptions, SIGNAL(accept()), this, SLOT(launchGeneration()));
    connect(pPtyTex->_pImportTextureInfo.get(), SIGNAL(importationFinished()), this, SLOT(updateIHM()));
    _groupBoxDoc->setFixedHeight(150);
}

void PtyWidgetTexture::launchGeneration()
{
    Ptr<Workflow::TextureOptions> pTextureOptions = _textureGenerationOptions->getTextureOptions();
    LM_DEBUG_PTR_CAST<PtyTexture>(getDataProxy()->getProperty())->generateTexture(pTextureOptions);
}

void PtyWidgetTexture::updateIHM()
{
    updateInfo();
    _textureGenerationOptions->optionsChanged();
}

void PtyWidgetTexture::updateInfo()
{
    Ptr<PtyTexture> pPtyTex = LM_DEBUG_PTR_CAST<PtyTexture>(getDataProxy()->getProperty());

    Ptr<Assets::Texture> pTexture = pPtyTex->_pImportTextureInfo->pTexture;
    if (pTexture->getMipmapCount() == 0)
    {
        pTexture = pPtyTex->_pWorld->getRessourcesPool()->getTextureData(pPtyTex->_pImportTextureInfo->textureName);
    }

    int32 w = pTexture->getWidth();
    int32 h = pTexture->getHeight();
    float size = 0;

    for (int32 iMipMap = 0; iMipMap<pTexture->getMipmapCount(); ++iMipMap)
    {
        Core::List<Assets::TextureImage> pImage = pTexture->getImage(iMipMap);

        for (int32 iImage = 0; iImage < pImage.size(); iImage++)
        {
            size += pImage[iImage].getDataSize();
        }
    }

    size /= 1048576.0f;

    _width->setText("Width : " + StrToQStr(Core::toString(w)));
    _height->setText("Height : " + StrToQStr(Core::toString(h)));
    _size->setText("Size : " + StrToQStr(Core::toString(size, 3)) + " Mo");

    pPtyTex->updateProperty();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}//namespace EPI