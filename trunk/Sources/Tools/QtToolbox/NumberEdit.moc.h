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
#ifndef QTTOOLBOX_NUMBER_EDIT_H_
#define QTTOOLBOX_NUMBER_EDIT_H_

#include <QWidget>

#include <Core/Standard.h>
#include <Core/Math/Vector.h>
#include <Core/DateAndTime.h>

namespace QtToolbox
{
//-----------------------------------------------------------------------------
class LM_API_QTB NumberEdit : public QWidget
{
    Q_OBJECT

public:
    NumberEdit(const QString& prefix, const QString& suffix, QWidget* parent);
    virtual ~NumberEdit();
    virtual bool eventFilter(QObject *dist, QEvent *event);
    virtual bool event (QEvent * e);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent * event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    virtual void focusInEvent ( QFocusEvent * event );

    double getValue() const;
    void   setValue(double val);
private:
    void setupUi();
    void updateEdit(double val);
    void stepBy(int steps);

    void emitValueUpdated();
    void emitValueHeld();
    void emitValueReleased();

private Q_SLOTS:
    void cursorPositionChanged(int old, int newP);
    void lineEditChanged();
    void lineEditFinished();
    void lineEditreturnPressed();
Q_SIGNALS:
    void signalWidgetUpdated();
    void signalWidgetHeld();
    void signalWidgetReleased();

private:
    QHBoxLayout *	 _playout;
    QLineEdit *      _pValue;
    QWidget *        _pGliss;
    QString          _suffix;
    QString          _prefix;

    double           _singleStep;
    int32           _screenHeight;
    int32           _screenWidth;
    int32           _mouseHeight;
    int32           _mouseWidth;
    bool            _isPressed;
    int32           _precision;
    Core::Vector2f  _curPos;

    double memo;
};
//-----------------------------------------------------------------------------

} //namespace EPI

#endif //QTTOOLBOX_NUMBER_EDIT_H_
