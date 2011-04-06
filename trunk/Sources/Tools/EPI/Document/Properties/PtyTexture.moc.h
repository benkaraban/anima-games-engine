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
#ifndef EPI_PROPERTY_TEXTURE_H_
#define EPI_PROPERTY_TEXTURE_H_

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>
#include <EPI/ImportInfo/ImportTextureInfo.h>
class QLabel;

namespace Universe
{
    class World;
}

namespace Workflow
{
    struct TextureOptions;
}

namespace QtToolbox
{
    class CollapsibleWidget;
    class QuickColorPicker;
}//namespace QtToolbox

class QGridLayout;

namespace EPI
{
class PtyWidgetTexture;
class TextureGenerationOptions;

//-----------------------------------------------------------------------------
class LM_API_EPI PtyTexture : public PropertyNode
{
    friend class PtyWidgetTexture;

public:
    PtyTexture(const Ptr<Universe::World>& pWorld, const Ptr<ImportTextureInfo> & pImportTextureInfo, const Core::String& title = L"Texture");
    virtual ~PtyTexture();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    virtual void updateData();
    virtual void updateProperty();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual String getIconFileName() {return L":/icons/ptyTexture.png";}

private:
    void generateTexture(const Ptr<Workflow::TextureOptions> & pTextureOptions);

private:
    Core::Vector4f                  _bcgColor;
    Ptr<Universe::World>            _pWorld;
    Ptr<ImportTextureInfo>          _pImportTextureInfo;
    Core::Ptr<Universe::NodeDecal>  _pNodeDecal; 
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetTexture : public PropertyWidget
{
    Q_OBJECT

    friend class PtyTexture;

public:
    PtyWidgetTexture(const Ptr<PropertyWidgetDataProxy>& data, const Ptr<ImportTextureInfo> & pImportTextureInfo, QWidget * parent = 0);
    virtual ~PtyWidgetTexture();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

protected Q_SLOTS:
    virtual void launchGeneration();
    void updateIHM();

private:
    void updateInfo();

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *   _groupBoxDoc;
    QtToolbox::CollapsibleWidget *   _groupBoxTex;
    QtToolbox::CollapsibleWidget *   _groupBoxInfo;
    QLabel*                          _width;
    QLabel*                          _height;
    QLabel*                          _size;

    QtToolbox::QuickColorPicker* _backGroundColor;
    TextureGenerationOptions *   _textureGenerationOptions;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //EPI_PROPERTY_TEXTURE_H_