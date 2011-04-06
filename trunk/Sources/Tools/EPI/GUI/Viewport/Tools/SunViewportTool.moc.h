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
#ifndef EPI_SUN_VIEWPORT_TOOL_H_
#define EPI_SUN_VIEWPORT_TOOL_H_

#include <Core/Standard.h>
#include <EPI/Gui/Viewport/Tools/ViewportTool.moc.h>
namespace EPI
{
//-----------------------------------------------------------------------------           
class LM_API_EPI SunViewportTool : public IViewportTool
{
    Q_OBJECT

public:
    SunViewportTool(const Ptr<StateRecorder>& pStateRecorder, const Ptr<Property>& pPtySun);
    virtual ~SunViewportTool();

    virtual Ptr<IViewportToolWidget> createViewportToolWidget(QWidget * parent);
    virtual void guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc);

    virtual void mouseMoveEvent         (QMouseEvent* event,const DrawContext& dc);
    virtual void mousePressEvent        (QMouseEvent* event,const DrawContext& dc);
    virtual void mouseReleaseEvent      (QMouseEvent* event,const DrawContext& dc);

    virtual const Core::List<Ptr<Property> >& getProperties() const;
    virtual Core::List<Ptr<Property> >& getProperties();

    virtual bool isHighlighted() const;
    virtual bool isSelected() const;

    virtual EViewportToolType   getType() const {return SUN_TOOL;}

    static void creatToolModels   (Ptr<Universe::RessourcePool>& pPool);
    static Ptr<Universe::NodeGroup>  createTool         (Ptr<Universe::World>& pWorld);

public Q_SLOTS:
   // virtual void selectionChanged(const Core::List<Ptr<Property> >& pties);
    virtual void updateViewportTool();

private:
    struct implementation;
	implementation* const _impl;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif //EPI_SUN_VIEWPORT_TOOL_H_