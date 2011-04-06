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
#include "SmartGroupBox.moc.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>

namespace QtToolbox
{

SmartGroupBox::SmartGroupBox(   QWidget *parent,
                                const QString & title, 
                                bool collapsable)
:   QGroupBox(title, parent),
    _collapsable(collapsable),
    _collapsed(false),
    _pixOpen(8, 12),
    _pixCollapsed(8, 12)
{
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setAlignment(Qt::AlignTop);
    createPixMaps();
    setLayout(_layout);
}

SmartGroupBox::~SmartGroupBox()
{
    delete _layout;
    _layout = NULL;
}

void SmartGroupBox::addWidget(QWidget * widget, Qt::Alignment align)
{
    _layout->addWidget(widget, 0, align);
    applyNewSize();
}

void SmartGroupBox::removeWidget(QWidget * widget)
{
    _layout->removeWidget(widget);
}

void SmartGroupBox::mousePressEvent(QMouseEvent * mouseEvent )
{
    if(_collapsable)
    {
        if( mouseEvent->button() == Qt::LeftButton )
        {
            QRect buttonArea( 0, 0, width(), 12);
            if( buttonArea.contains( mouseEvent->pos() ) )
            {
                _pointClicked = mouseEvent->pos();
            }
        }
    }
    QGroupBox::mousePressEvent(mouseEvent);
}

void SmartGroupBox::mouseReleaseEvent(QMouseEvent * mouseEvent)
{
    if(_collapsable)
    {
        if( mouseEvent->button() == Qt::LeftButton && _pointClicked == mouseEvent->pos() )
            collapse( !_collapsed );
    }
    QGroupBox::mouseReleaseEvent(mouseEvent);
}

void SmartGroupBox::collapse( bool collapse )
{
    _collapsed = collapse;

    for(int ii=0; ii<_layout->count(); ++ii)
    {
        _layout->itemAt(ii)->widget()->setHidden(_collapsed);
    }

    applyNewSize();
}

void SmartGroupBox::paintEvent(QPaintEvent * paintEvent)
{
    QGroupBox::paintEvent(paintEvent);
    
    if(_collapsable)
    {
        //On recopie la pixmap dans le widget
        QPainter smartGBDPainter(this);
        if(_collapsed)
            smartGBDPainter.drawPixmap(rect().topLeft(), _pixCollapsed);
        else
            smartGBDPainter.drawPixmap(rect().topLeft(), _pixOpen);
    }
}

void SmartGroupBox::applyNewSize()
{
    long layoutWidth = 0;
    long layoutHeight = 0;
   
    for(long i = 0; i<_layout->count(); i++)
    {
        long widgetWidth = _layout->itemAt(i)->widget()->width();
        if(widgetWidth > layoutWidth)
            layoutWidth = widgetWidth;
        
        if(!_collapsed)
            layoutHeight += _layout->itemAt(i)->widget()->height();
    }

    if(_collapsed)
        layoutHeight = 13;
    else
        layoutHeight = layoutHeight + (6 * _layout->count()) + 11;
    
    setFixedWidth(layoutWidth + 4);
    setFixedHeight(layoutHeight);
    
    emit newSize(width(), height());
}

void SmartGroupBox::createPixMaps()
{
    QPainter openPainter(&_pixOpen);
    openPainter.fillRect(_pixOpen.rect(), palette().color(QPalette::Window));

    QPointF openPoints[3] = {   QPointF(_pixOpen.width()-1, _pixOpen.height()/4),
                                QPointF(_pixOpen.width()-1, 3*_pixOpen.height()/4),
                                QPointF(1, 3*_pixOpen.height()/4)};

    openPainter.setPen(Qt::black);
    openPainter.setBrush(QBrush(Qt::darkGray));
    openPainter.drawPolygon(openPoints, 3);

    QPainter collapsedPainter(&_pixCollapsed);
    collapsedPainter.fillRect(_pixCollapsed.rect(), palette().color(QPalette::Window));
    
    QPointF collapsedPoints[3] = {  QPointF(1, _pixCollapsed.height()/4),
                                    QPointF(_pixCollapsed.width()-1, _pixCollapsed.height()/2),
                                    QPointF(1, 3*_pixCollapsed.height()/4)};

    collapsedPainter.setPen(Qt::darkGray);
    collapsedPainter.setBrush(QBrush(palette().color(QPalette::Window)));
    collapsedPainter.drawPolygon(collapsedPoints, 3);
}

}//namespace QtToolbox