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
#ifndef SMARTGROUPBOX_H_
#define SMARTGROUPBOX_H_

#include <QGroupBox>

class QVBoxLayout;

namespace QtToolbox
{

class LM_API_QTB SmartGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    SmartGroupBox(  QWidget *parent = 0,
                    const QString & title = "",
                    bool collapsable = false);
    virtual ~SmartGroupBox();

    void addWidget(QWidget * widget, Qt::Alignment align = Qt::AlignLeft);
    void removeWidget(QWidget * widget);

    void collapse( bool collapse );
    void applyNewSize();

    inline QString getTitle() const { return title(); }
    inline void setTitle(const QString & title) { QGroupBox::setTitle(title); }

Q_SIGNALS:
    void newSize(int width, int height);

protected:
	virtual void paintEvent(QPaintEvent * paintEvent);
    virtual void mousePressEvent(QMouseEvent * mouseEvent);
	virtual void mouseReleaseEvent(QMouseEvent * mouseEvent);

private:
    void createPixMaps();
    
private:
    QVBoxLayout *   _layout;
    bool            _collapsable;
    bool            _collapsed;
    QPixmap         _pixOpen;
    QPixmap         _pixCollapsed;
    QPoint          _pointClicked;

};

}//namespace QtToolbox

#endif GROUPBOX_H_