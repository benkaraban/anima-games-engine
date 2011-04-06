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
#ifndef SPIN_BOX_VECTOR_H_
#define SPIN_BOX_VECTOR_H_

#include <QWidget>

#include <Core/Math/Vector.h>

//-----------------------------------------------------------------------------
class QGridLayout;
class QLabel;
//-----------------------------------------------------------------------------
namespace QtToolbox
{
class SpinBox;
//-----------------------------------------------------------------------------
class LM_API_QTB SpinBoxVector3 : public QWidget
{
    Q_OBJECT

public:
    SpinBoxVector3( QWidget *parent = 0, const QString & name = "", Core::String textX = L"X : ", Core::String textY = L"Y : ", Core::String textZ = L"Z : ");
    virtual ~SpinBoxVector3();

    void setValue(const Core::Vector3f& value);
    void getValue(Core::Vector3f& value) const;

    void setSuffix(const Core::String& suffix);
    void setSuffix(const char* suffix);
    void setRange(double min, double max);
    void setSingleStep(double step);

    virtual void setupUi();

private Q_SLOTS:
    void spinBoxXChanged();
    void spinBoxYChanged();
    void spinBoxZChanged();
    void widgetHolding();
    void widgetReleasing();

Q_SIGNALS:
    void signalWidgetUpdated();
    void signalWidgetHeld();
    void signalWidgetReleased();



private:
    QString          _name;
    QGridLayout*     _pGridLayout;
    Core::String     _textX;
    Core::String     _textY;
    Core::String     _textZ;
    QLabel*         _labelTitle;

    SpinBox*  _pSpinBoxX;
    SpinBox*  _pSpinBoxY;
    SpinBox*  _pSpinBoxZ;

};
//-----------------------------------------------------------------------------
}//namespace QtToolbox

#endif /*SPIN_BOX_VECTOR_H_*/