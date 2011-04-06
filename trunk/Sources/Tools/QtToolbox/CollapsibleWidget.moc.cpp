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
#include "CollapsibleWidget.moc.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QLabel>

namespace QtToolbox
{

CollapsibleWidget::CollapsibleWidget(   QWidget *parent,
                                const QString & title, 
                                bool collapsable)
:   QWidget(parent),
    _collapsable(collapsable),
    _collapsed(false),
    _pixOpen(8, 12),
    _pixCollapsed(8, 12),
    _widgetLabel(NULL),
    _style(CW_TITLE_2),
    _colorBgTitle1(210,210,210),
    _colorBgTitle2(230,230,230)
{
    _layout = new QVBoxLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        QWidget::setLayout(_layout);

    _widgetTitle = new QWidget(this);
    _layoutTitle = new QHBoxLayout(NULL);
            _layoutTitle->setContentsMargins(0, 0, 0, 0);
            _layoutTitle->setAlignment(Qt::AlignTop);

        QWidget* margeLeft = new QWidget(_widgetTitle);
            margeLeft->setFixedSize(13,13);
        _widgetLabel = new QLabel(title, _widgetTitle);
            _widgetLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
            _widgetTitle->setLayout(_layoutTitle);
        _layoutTitle->addWidget(margeLeft);
        _layoutTitle->addWidget(_widgetLabel);
    
    _mainWidget = new QWidget(this);
        QGridLayout* layoutMainWidget = new QGridLayout(NULL);
            layoutMainWidget->setSpacing(0);
            layoutMainWidget->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        _mainWidget->setLayout(layoutMainWidget);
    
    _layout->addWidget(_widgetTitle);
    _layout->addWidget(_mainWidget);

    createPixMaps();
}

CollapsibleWidget::~CollapsibleWidget()
{
    delete _layout;
    delete _mainWidget;
    _mainWidget = NULL;
    _layout = NULL;
}

void CollapsibleWidget::addWidgetToTitle(QWidget* w)
{
    _layoutTitle->addWidget(w, 0, Qt::AlignRight);
    w->setFixedHeight(12);
    w->setFixedWidth(12);
}

QString CollapsibleWidget::getTitle() const
{
    LM_ASSERT(_widgetLabel!=NULL); 
    return _widgetLabel->text();
}

void CollapsibleWidget::getTitle(Core::String& title) const
{
    LM_ASSERT(_widgetLabel!=NULL); 
    Core::String8 title8 (_widgetLabel->text().toStdString().c_str());
    title = Core::String(title8);
}

void CollapsibleWidget::setTitle(const QString & title)
{
    LM_ASSERT(_widgetLabel!=NULL);
    
    _widgetLabel->setText(title);
}

void CollapsibleWidget::setTitle(const String & title)
{
    LM_ASSERT(_widgetLabel!=NULL);
    _widgetLabel->setText(Core::String8(title).c_str());
}

void CollapsibleWidget::setAlignmentTitle(const Qt::Alignment& align)
{
    LM_ASSERT(_widgetLabel!=NULL);
    _widgetLabel->setAlignment(align);
}

void CollapsibleWidget::setLayout ( QLayout * layout )
{
    if (_mainWidget->layout() != NULL) delete _mainWidget->layout();
    _mainWidget->setLayout(layout);
}

QLayout* CollapsibleWidget::getLayout()
{
    return _mainWidget->layout();
}

void CollapsibleWidget::setStyle(ECollacpsibleWTitleStyle style)
{
    _style = style;
    createPixMaps();

    if (_style==CW_TITLE_1)
        getLayout()->setContentsMargins(0, 0, 0, 0);
}

void CollapsibleWidget::mousePressEvent(QMouseEvent * mouseEvent )
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
    QWidget::mousePressEvent(mouseEvent);
}

void CollapsibleWidget::mouseReleaseEvent(QMouseEvent * mouseEvent)
{
    if(_collapsable)
    {
        if( mouseEvent->button() == Qt::LeftButton && _pointClicked == mouseEvent->pos() )
            collapse( !_collapsed );
    }
    QWidget::mouseReleaseEvent(mouseEvent);
}

void CollapsibleWidget::collapse( bool collapse )
{
    _collapsed = collapse;
/*
    for(int ii=0; ii<_layout->count(); ++ii)
    {
        _layout->itemAt(ii)->widget()->setHidden(_collapsed);
    }*/


    _mainWidget->setHidden(_collapsed);

   // applyNewSize();
}

const QColor&   CollapsibleWidget::getBackgroundTitleColor() const
{
    switch(_style)
    {
        case CW_TITLE_1: return _colorBgTitle1; break;
        case CW_TITLE_2: return _colorBgTitle2; break;
    }

    return _colorBgTitle1;
}


void CollapsibleWidget::paintEvent(QPaintEvent * paintEvent)
{
    QWidget::paintEvent(paintEvent);
   
    QPainter smartGBDPainter(this);
    QRect frameGeo = rect();

    QRect backgroundTitle (frameGeo.topLeft().x(), frameGeo.topLeft().y(), frameGeo.width(), 13);
    smartGBDPainter.fillRect(backgroundTitle, getBackgroundTitleColor());

    



    if(_collapsable)
    {
        if(_collapsed)
            smartGBDPainter.drawPixmap(rect().topLeft(), _pixCollapsed);
        else
            smartGBDPainter.drawPixmap(rect().topLeft(), _pixOpen);
    }


    switch(_style)
    {
        case CW_TITLE_1:
            {
 
            }
            break;
        case CW_TITLE_2:
            {
                smartGBDPainter.setPen(QPen(Qt::darkGray, 1)) ;
                smartGBDPainter.drawLine(frameGeo.topLeft(), frameGeo.topRight());
            }
            break;
    }
}

void CollapsibleWidget::applyNewSize()
{/*
    if(_collapsed)
    {
        setMinimumSize(_mainWidget->width(), 13);
    }
    emit newSize(width(), height());*/
}

void CollapsibleWidget::createPixMaps()
{
    QPainter openPainter(&_pixOpen);
    openPainter.setBackground(QBrush(getBackgroundTitleColor()));
    openPainter.fillRect(_pixOpen.rect(), getBackgroundTitleColor());

    QPointF openPoints[3] = {   QPointF(_pixOpen.width()-1, _pixOpen.height()/4),
                                QPointF(_pixOpen.width()-1, 3*_pixOpen.height()/4),
                                QPointF(1, 3*_pixOpen.height()/4)};

    
    openPainter.setPen(Qt::black);
    openPainter.setBrush(QBrush(Qt::darkGray));
    openPainter.drawPolygon(openPoints, 3);

    QPainter collapsedPainter(&_pixCollapsed);
    collapsedPainter.setBackground(QBrush(getBackgroundTitleColor()));
    collapsedPainter.fillRect(_pixCollapsed.rect(), getBackgroundTitleColor());
    
    QPointF collapsedPoints[3] = {  QPointF(1, _pixCollapsed.height()/4),
                                    QPointF(_pixCollapsed.width()-1, _pixCollapsed.height()/2),
                                    QPointF(1, 3*_pixCollapsed.height()/4)};

    
    collapsedPainter.setPen(Qt::darkGray);
    collapsedPainter.setBrush(QBrush(palette().color(QPalette::Window)));
    collapsedPainter.drawPolygon(collapsedPoints, 3);
}

}//namespace QtToolbox