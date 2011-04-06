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
#include "WidgetRenderArea.moc.h"

namespace QtToolbox
{
//-----------------------------------------------------------------------------
WidgetRenderArea::WidgetRenderArea(QWidget* parent, Qt::WindowFlags f):
QWidget(parent, f)
{
    setAttribute(Qt::WA_PaintOnScreen, true);
}   
//-----------------------------------------------------------------------------
WidgetRenderArea::~WidgetRenderArea()
{
}
//-----------------------------------------------------------------------------
void WidgetRenderArea::setMouseTracking(bool enable)
{
    QWidget::setMouseTracking(enable);
}
//-----------------------------------------------------------------------------
void WidgetRenderArea::initializeRenderArea()
{
}
//-----------------------------------------------------------------------------

bool WidgetRenderArea::event(QEvent *e)
{
//    Q_D(QGLWidget);
/*
    if (e->type() == QEvent::Paint)
    {
        updateRenderArea();
        return true;
    }*/
/*
#if defined(Q_WS_X11)
    // prevents X errors on some systems, where we get a flush to a
    // hidden widget
    if (e->type() == QEvent::Hide) 
    {
        makeCurrent();
        glFinish();
        doneCurrent();
    } 
    else if (e->type() == QEvent::ParentChange) 
    {
        if (d->glcx->d_func()->screen != d->xinfo.screen()) 
        {
            setContext(new QGLContext(d->glcx->requestedFormat(), this));
            // ### recreating the overlay isn't supported atm
        }
#if defined(QT_OPENGL_ES)
        // The window may have been re-created during re-parent - if so, the EGL
        // surface will need to be re-created.
        d->recreateEglSurface(false);
#endif
    }
#elif defined(Q_WS_WIN)
    if (e->type() == QEvent::ParentChange)
    {
        QGLContext *newContext = new QGLContext(d->glcx->requestedFormat(), this);
        qgl_share_reg()->replaceShare(d->glcx, newContext);
        setContext(newContext);
        // the overlay needs to be recreated as well
        delete d->olcx;
        if (isValid() && context()->format().hasOverlay()) {
            d->olcx = new QGLContext(QGLFormat::defaultOverlayFormat(), this);
            if (!d->olcx->create(isSharing() ? d->glcx : 0)) {
                delete d->olcx;
                d->olcx = 0;
                d->glcx->d_func()->glFormat.setOverlay(false);
            }
        } else {
            d->olcx = 0;
        }
    } else if (e->type() == QEvent::Show) 
    {
        if (!format().rgba())
            d->updateColormap();
    }
#elif defined(Q_WS_MAC)
    if (e->type() == QEvent::MacGLWindowChange
#if 0 //(MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5)
        && ((QSysInfo::MacintoshVersion >= QSysInfo::MV_10_5 && isWindow())
            || QSysInfo::MacintoshVersion <= QSysInfo::MV_10_4)
#endif
        ) 
        {
            if (d->needWindowChange) 
            {
                d->needWindowChange = false;
                d->glcx->updatePaintDevice();
                update();
            }
        return true;
#  if defined(QT_MAC_USE_COCOA)
    } else if (e->type() == QEvent::MacGLClearDrawable) {
        d->glcx->d_ptr->clearDrawable();
#  endif
    }
#endif
*/
    return QWidget::event(e);
}
//-----------------------------------------------------------------------------
void WidgetRenderArea::paintEvent(QPaintEvent*)
{
    updateRenderArea();
}
//-----------------------------------------------------------------------------
void WidgetRenderArea::resizeEvent(QResizeEvent * e)
{
/*    if (!isValid())
        return;
    makeCurrent();
    if (!d->glcx->initialized())
        glInit();*/
    resizeRenderArea();

 /*   if (d->olcx) {
        makeOverlayCurrent();
        resizeOverlayGL(width(), height());
    }*/
}
//-----------------------------------------------------------------------------
void WidgetRenderArea::updateRenderArea()
{
    if (updatesEnabled() == true)
    {
        drawRenderArea();
    }
}
//-----------------------------------------------------------------------------
void WidgetRenderArea::drawRenderArea()
{
    paintRenderArea();
}
//-----------------------------------------------------------------------------
QPaintEngine * WidgetRenderArea::paintEngine () const 
{
    return NULL;
}
//-----------------------------------------------------------------------------
} //namespace QtToolbox
