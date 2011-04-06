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
#ifndef QTTOOLBOX_NUMBER_EDIT_VECTOR_3_H_
#define QTTOOLBOX_NUMBER_EDIT_VECTOR_3_H_

#include <QWidget>
#include <Core/Standard.h>
#include <Core/Math/Vector.h>

namespace QtToolbox
{
class NumberEdit;
//-----------------------------------------------------------------------------
class LM_API_QTB NumberEditVector3 : public QWidget
{
    Q_OBJECT

public:
    NumberEditVector3(QWidget* parent, const QString & name = "", const QString& unit=" m", const QString& xLabel="X: ", const QString& yLabel="Y: ", const QString& zLabel="Z: ");
    virtual ~NumberEditVector3();

    void setValue(const Core::Vector3f& value);
    void getValue(Core::Vector3f& value) const;

private:
    void setupUi();

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
    QString          _unit;
    QString          _xLabel;
    QString          _yLabel;
    QString          _zLabel;

    QGridLayout*     _pGridLayout;

    QLabel*         _labelTitle;
    NumberEdit*     _pSpinBoxX;
    NumberEdit*     _pSpinBoxY;
    NumberEdit*     _pSpinBoxZ;
};
//-----------------------------------------------------------------------------
} //namespace EPI

#endif //QTTOOLBOX_NUMBER_EDIT_VECTOR_3_H_
