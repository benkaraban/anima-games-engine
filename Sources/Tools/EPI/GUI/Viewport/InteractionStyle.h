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
#ifndef INTERACTION_STYLE_H_
#define INTERACTION_STYLE_H_

class QMouseEvent;
class QKeyEvent;

#include <Core/Standard.h>

#include <EPI/GUI/Viewport/INavigationStyle.moc.h>
#include <EPI/GUI/Viewport/IInteractionStyle.h>
#include <EPI/GUI/Viewport/IViewportPicking.h>
#include <EPI/GUI/Viewport/ISelectionManager.moc.h>
#include <EPI/GUI/Viewport/ViewportToolInteraction.moc.h>


namespace EPI
{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

 /**
 * 
 */
LM_ENUM_3 (EIsItASingleClick,
           SINGLE_CLICK_YES,
           SINGLE_CLICK_NO,
           SINGLE_CLICK_UNDEFINED)
class LM_API_EPI DefaultInteractionStyle : public IInteractionStyle
{
public:
    DefaultInteractionStyle(const Ptr<INavigationStyle>& pNavigation, const Ptr<ViewportToolInteraction>& pVTInteraction, const Ptr<IViewportPicking>& pPicking, 
                            const Ptr<ISelectionManager>& pSelectMng, const Ptr<Universe::NodeCamera>& pNodeCam);
    virtual ~DefaultInteractionStyle();

    virtual void setNavigationStyle(const Ptr<INavigationStyle>& pNavigation);
    virtual void setPicking(const Ptr<IViewportPicking> pPick);

    virtual void keyPressEvent         (QKeyEvent* event,   const DrawContext& dc);
    virtual void keyReleaseEvent       (QKeyEvent* event,   const DrawContext& dc);
    virtual void mousePressEvent       (QMouseEvent* event, const DrawContext& dc);
    virtual void mouseMoveEvent        (QMouseEvent* event, const DrawContext& dc);
    virtual void mouseReleaseEvent     (QMouseEvent* event, const DrawContext& dc);
    virtual void wheelEvent            (QWheelEvent* event, const DrawContext& dc);
    virtual void mouseDoubleClickEvent (QMouseEvent* event, const DrawContext& dc);

    virtual void mouseLeftSingleClick(QMouseEvent* event, const DrawContext& dc);
    virtual void mouseRightSingleClick(QMouseEvent* event, const DrawContext& dc);
    virtual void mouseMiddleSingleClick(QMouseEvent* event, const DrawContext& dc);

    virtual void mouseLeftDoubleClick(QMouseEvent* event, const DrawContext& dc);

    virtual const Ptr<INavigationStyle>& getNavigationStyle() const {return _pNavigation;}

    virtual void updateCameraMove(Ptr<Universe::NodeCamera> pNCam);

private:
    Ptr<Universe::NodeCamera>       _pNodeCam;
    Ptr<INavigationStyle>           _pNavigation;
    Ptr<ViewportToolInteraction>    _pVTInteraction;

    Ptr<IViewportPicking>           _pPicking;
    Ptr<ISelectionManager>          _pSelectMng;

    EIsItASingleClick               _singleClickLeftButton;
    EIsItASingleClick               _singleClickRightButton;
    EIsItASingleClick               _singleClickMiddleButton;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif // INTERACTION_STYLE_H_
