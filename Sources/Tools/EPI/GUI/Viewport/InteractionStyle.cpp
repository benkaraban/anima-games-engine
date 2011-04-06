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
#include "InteractionStyle.h"

#include <QMouseEvent>
#include <QKeyEvent>

#include <EPI/Document/Property.moc.h>

namespace EPI
{
int32 KEY_ADD_TO_SELECTION         = Qt::Key_Control;

//-----------------------------------------------------------------------------
DefaultInteractionStyle::DefaultInteractionStyle(const Ptr<INavigationStyle>& pNavigation, const Ptr<ViewportToolInteraction>& pVTInteraction, const Ptr<IViewportPicking>& pPicking, const Ptr<ISelectionManager>& pSelectMng, const Ptr<Universe::NodeCamera>& pNodeCam):
    _pNavigation(pNavigation),
    _pVTInteraction(pVTInteraction),
    _pPicking(pPicking),
    _pSelectMng(pSelectMng),
    _pNodeCam(pNodeCam),
    _singleClickLeftButton(SINGLE_CLICK_NO),
    _singleClickRightButton(SINGLE_CLICK_NO),
    _singleClickMiddleButton(SINGLE_CLICK_NO)
{
    if (_pNavigation != null && _pSelectMng != null)
    {
        _pNavigation->selectionChanged(pSelectMng->getPtySelection()->getSelectedPties());
        QObject::connect(_pSelectMng.get(), SIGNAL(selectionChanged(const Core::List<Ptr<Property> >&)), _pNavigation.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));
    }                                              
}
//-----------------------------------------------------------------------------
DefaultInteractionStyle::~DefaultInteractionStyle()
{
}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::keyPressEvent(QKeyEvent* event,   const DrawContext& dc)
{
    if (event->key() == KEY_ADD_TO_SELECTION)
    {
        _pSelectMng->configureAddToSelection(true);
    }

    _pNavigation->keyPressEvent(event, dc);
}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::keyReleaseEvent(QKeyEvent* event,   const DrawContext& dc)
{
    if (event->key() == KEY_ADD_TO_SELECTION)
    {
        _pSelectMng->configureAddToSelection(false);
    }

    _pNavigation->keyReleaseEvent(event, dc);
}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::mousePressEvent(QMouseEvent* event, const DrawContext& dc)
{
    Ptr<Renderer::IViewVolume> volume = _pNodeCam->getBaseCamera()->createViewVolume();

    if (event->button() & Qt::LeftButton)  
        _singleClickLeftButton   = SINGLE_CLICK_UNDEFINED;
    if (event->button() & Qt::RightButton) 
        _singleClickRightButton  = SINGLE_CLICK_UNDEFINED;
    if (event->button() & Qt::MidButton)   
        _singleClickMiddleButton = SINGLE_CLICK_UNDEFINED;

    if (_pVTInteraction->isHighlighted() == true || _pVTInteraction->isSelected())
    {
        _pVTInteraction->mousePressEvent(event, dc);
    }
    else
    {
        _pNavigation->mousePressEvent(event, dc);
    }
}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::mouseMoveEvent(QMouseEvent* event, const DrawContext& dc)
{
    _singleClickLeftButton   = SINGLE_CLICK_NO;
    _singleClickRightButton  = SINGLE_CLICK_NO;
    _singleClickMiddleButton = SINGLE_CLICK_NO;

    Ptr<Renderer::IViewVolume> volume = _pNodeCam->getBaseCamera()->createViewVolume();
    
    _pVTInteraction->mouseMoveEvent(event, dc);

    if (_pVTInteraction->isSelected() == false)
    {
        _pNavigation->mouseMoveEvent(event, dc);
    }

}
void DefaultInteractionStyle::wheelEvent(QWheelEvent* event, const DrawContext& dc)
{
    _pNavigation->wheelEvent(event, dc);
}

void DefaultInteractionStyle::updateCameraMove(Ptr<Universe::NodeCamera> pNCam)
{
    _pVTInteraction->updateCameraMove(pNCam);
}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::mouseReleaseEvent(QMouseEvent* event, const DrawContext& dc)
{
    Ptr<Renderer::IViewVolume> volume = _pNodeCam->getBaseCamera()->createViewVolume();

    if (event->button() & Qt::LeftButton  && _singleClickLeftButton == SINGLE_CLICK_UNDEFINED)
        _singleClickLeftButton   = SINGLE_CLICK_YES;
    if (event->button() & Qt::RightButton && _singleClickRightButton == SINGLE_CLICK_UNDEFINED)
        _singleClickRightButton  = SINGLE_CLICK_YES;
    if (event->button() & Qt::MidButton   && _singleClickMiddleButton == SINGLE_CLICK_UNDEFINED)
        _singleClickMiddleButton = SINGLE_CLICK_YES;

    if (_singleClickLeftButton   == SINGLE_CLICK_YES && 
        _singleClickRightButton  == SINGLE_CLICK_NO &&
        _singleClickMiddleButton == SINGLE_CLICK_NO)
    {
        mouseLeftSingleClick(event, dc);
    }

    if (_singleClickLeftButton   == SINGLE_CLICK_NO && 
        _singleClickRightButton  == SINGLE_CLICK_YES &&
        _singleClickMiddleButton == SINGLE_CLICK_NO)
    {
        mouseRightSingleClick(event, dc);
    }

    if (_singleClickLeftButton   == SINGLE_CLICK_NO && 
        _singleClickRightButton  == SINGLE_CLICK_NO &&
        _singleClickMiddleButton == SINGLE_CLICK_YES)
    {
        mouseMiddleSingleClick(event, dc);
    }

    _singleClickLeftButton   = SINGLE_CLICK_NO;
    _singleClickRightButton  = SINGLE_CLICK_NO;
    _singleClickMiddleButton = SINGLE_CLICK_NO;

    if (_pVTInteraction->isSelected() == true)
    {
        _pVTInteraction->mouseReleaseEvent(event, dc);
    }
    else
    {
        _pNavigation->mouseReleaseEvent(event, dc);
    }

}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::mouseDoubleClickEvent (QMouseEvent* event, const DrawContext& dc)
{
    if (event->button() & Qt::LeftButton)
        mouseLeftDoubleClick(event, dc);
}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::mouseLeftSingleClick(QMouseEvent* event, const DrawContext& dc)
{
    //selection
    if (_pSelectMng != null)
    {
        Ptr<Renderer::IViewVolume> volume = dc.pCamera->getBaseCamera()->createViewVolume();
        Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);

        Core::Rayf ray = volume->makeRayFrom2DPoint(pos);

        Ptr<Property> pPty;

        const Core::List<Ptr<Property> >& curentSelection = _pSelectMng->getPtySelection()->getSelectedPties();

        //if (_pPicking->rayToProperty(ray, pPty) == true)
        if (_pPicking->selectNext(curentSelection, ray, pPty) == true)
        {
            _pSelectMng->select(pPty);
        }
        else
        { 
            _pSelectMng->deselectAll();
        }
    }
}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::mouseRightSingleClick(QMouseEvent* event, const DrawContext& dc)
{

}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::mouseMiddleSingleClick(QMouseEvent* event, const DrawContext& dc)
{
    _pVTInteraction->mouseMiddleSingleClick(event, dc);
}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::mouseLeftDoubleClick(QMouseEvent* event, const DrawContext& dc)
{

}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::setNavigationStyle(const Ptr<INavigationStyle>& pNavigation)
{
    if (_pNavigation != null && _pSelectMng != null)
        QObject::disconnect(_pSelectMng.get(), SIGNAL(selectionChanged(const Core::List<Ptr<Property> >&)), _pNavigation.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));

    _pNavigation = pNavigation;

    if (_pNavigation != null && _pSelectMng != null)
        QObject::connect(_pSelectMng.get(), SIGNAL(selectionChanged(const Core::List<Ptr<Property> >&)), _pNavigation.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));
}
//-----------------------------------------------------------------------------
void DefaultInteractionStyle::setPicking(const Ptr<IViewportPicking> pPick)
{
    _pPicking = pPick;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI
