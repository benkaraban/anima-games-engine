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
#ifndef PTY_NODE_STORM_H_
#define PTY_NODE_STORM_H_

#include <EPI/Document/Properties/PtyNode.moc.h>
#include <Universe/NodeStorm.h>

namespace QtToolbox
{
    class SingleSlidingHDR;
    class RangedSlidingTimeValue;
    class RangedSlidingHDR;
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
class LM_API_EPI PtyNodeStorm : public PtyNode
{
    friend class PtyWidgetNodeStorm;

public:
    PtyNodeStorm( const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeStorm> & pNodeStorm,
                  const Core::String& title = L"Node Storm");
    virtual ~PtyNodeStorm();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    virtual void updateData();
    void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual String getIconFileName() {return L":/icons/ptyStorm.png";}

private:
    int32           _maxCount;
    float           _minPeriod;
    float           _maxPeriod;
    bool            _burstStart;

    double          _startTime;
    double          _endTime;

    float           _entropy;
    int32           _fertility;
    int32           _resolution;

    String          _textureName;
    Core::Vector4f  _color1;
    Core::Vector4f  _color2;

    Renderer::EFreeFormMode _mode;

    Core::Vector4f  _glow1;
    Core::Vector4f  _glow2;

    float           _fadeIn;
    float           _duration;
    float           _fadeOut;

    float           _headWidth;
    float           _tailWidth;

    Core::Vector3f  _headCenter;
    Core::Vector3f  _headSize;
    Core::Vector3f  _tailCenter;
    Core::Vector3f  _tailSize;

    float           _travelTime;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetNodeStorm : public PropertyWidget
{
    Q_OBJECT
    friend class PtyNodeStorm;

public:
    PtyWidgetNodeStorm(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetNodeStorm();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void applyNewSize();
    void restartStorm();

private:
    QGridLayout *                       _layout;
    QPushButton *                       _restartStorm;
    QtToolbox::CollapsibleWidget *      _groupBox;
    QtToolbox::CollapsibleWidget *      _groupBoxGeneration;
    QtToolbox::CollapsibleWidget *      _groupBoxAnimation;

    QtToolbox::RangedSlidingHDR *       _period;
    QtToolbox::SingleSlidingInt *       _maxCount;
    QCheckBox *                         _burstStart;

    QtToolbox::RangedSlidingTimeValue * _time;

    QtToolbox::SingleSlidingHDR *       _entropy;
    QtToolbox::SingleSlidingInt *       _fertility;
    QtToolbox::SingleSlidingInt *       _resolution;

    CustomLine *                        _textureName;
    QtToolbox::QuickColorPicker *       _color1;
    QtToolbox::QuickColorPicker *       _color2;
    QtToolbox::ComboBox *               _mode;

    QtToolbox::QuickColorPicker *       _glow1;
    QtToolbox::QuickColorPicker *       _glow2;

    QtToolbox::SingleSlidingHDR *       _fadeIn;
    QtToolbox::SingleSlidingHDR *       _duration;
    QtToolbox::SingleSlidingHDR *       _fadeOut;

    QtToolbox::SingleSlidingHDR *       _headWidth;
    QtToolbox::SingleSlidingHDR *       _tailWidth;

    QtToolbox::SingleSlidingHDR *       _headCenterX;
    QtToolbox::SingleSlidingHDR *       _headCenterY;
    QtToolbox::SingleSlidingHDR *       _headCenterZ;

    QtToolbox::SingleSlidingHDR *       _headSizeX;
    QtToolbox::SingleSlidingHDR *       _headSizeY;
    QtToolbox::SingleSlidingHDR *       _headSizeZ;

    QtToolbox::SingleSlidingHDR *       _tailCenterX;
    QtToolbox::SingleSlidingHDR *       _tailCenterY;
    QtToolbox::SingleSlidingHDR *       _tailCenterZ;

    QtToolbox::SingleSlidingHDR *       _tailSizeX;
    QtToolbox::SingleSlidingHDR *       _tailSizeY;
    QtToolbox::SingleSlidingHDR *       _tailSizeZ;

    QtToolbox::SingleSlidingHDR *       _travelTime;

    Ptr<PropertyWidget>                 _pPWNode;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*PTY_NODE_STORM_H_*/
