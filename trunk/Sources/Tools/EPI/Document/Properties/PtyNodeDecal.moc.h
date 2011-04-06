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
#ifndef PTY_NODE_DECAL_H_
#define PTY_NODE_DECAL_H_

#include <EPI/Document/Properties/PtyNode.moc.h>

#include <Universe/NodeDecal.h>

#include <Renderer/IBillboard.h>

namespace QtToolbox
{
    class SingleSlidingHDR;
    class SingleSlidingDouble;
    class RangedSlidingDouble;
    class CollapsibleWidget;
    class QuickColorPicker;
    class ComboBox;
    class LineEdit;
}//namespace QtToolbox

class QGridLayout;
class QCheckBox;
class QPushButton;
class QHBoxLayout;

namespace EPI
{
class CustomLine;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyNodeDecal : public PtyNode
{
    friend class PtyWidgetNodeDecal;

public:
    PtyNodeDecal( const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeDecal> & pNodeDecal,
                  const Core::String& title = L"Node Decal");
    virtual ~PtyNodeDecal();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    virtual void updateData();
    void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual String getIconFileName() {return L":/icons/ptyDecal.png";}
    virtual EPropertyClassName getCalssName() const {return PTYNODE_DECAL;}
private:
    Core::String            _textureName;
    Renderer::EDecalMode    _decalMode;
    float   _width;
    float   _height;

    int32   _uSpriteLayout;
    int32   _vSpriteLayout;

    Core::Vector4f  _color;
    Core::Vector4f  _glow;

    float           _zBias;
    float           _fogFactor;

    double  _duration;
    double  _keyFrom;
    double  _keyTo;

    bool    _loop;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetNodeDecal : public PropertyWidget
{
    Q_OBJECT
    friend class PtyNodeDecal;

public:
    PtyWidgetNodeDecal(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetNodeDecal();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void applyNewSize();

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *      _groupBox;

    QtToolbox::CollapsibleWidget *      _groupTexture;
    CustomLine *                        _textureName;
    QtToolbox::ComboBox *               _decalMode;
    QtToolbox::LineEdit *               _uSpriteLayout;
    QtToolbox::LineEdit *               _vSpriteLayout;
    
    QtToolbox::CollapsibleWidget *      _groupColors;
    QtToolbox::QuickColorPicker *       _color;
    QtToolbox::QuickColorPicker *       _glow;

    QtToolbox::CollapsibleWidget *      _groupAnimation;
    QtToolbox::SingleSlidingHDR *       _width;
    QtToolbox::SingleSlidingHDR *       _height;
    
    QtToolbox::SingleSlidingHDR *       _zBias;
    QtToolbox::SingleSlidingDouble *    _fogFactor;

    QtToolbox::RangedSlidingDouble *    _keys;
    QtToolbox::SingleSlidingDouble *    _duration;
    QCheckBox *                         _loop;

    Ptr<PropertyWidget>                 _pPWNode;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_NODE_DECAL_H_*/
