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
#ifndef EPI_COPY_CAMERA_LOCATION_TOOL_H_
#define EPI_COPY_CAMERA_LOCATION_TOOL_H_

#include <Core/Standard.h>
#include <EPI/Gui/Viewport/Tools/ViewportTool.moc.h>

class QPushButton;

namespace QtToolbox
{
    class SingleSlidingDouble;
}

namespace EPI
{
class GuiDocument;
class CopyLocationWidget;

//-----------------------------------------------------------------------------           
class LM_API_EPI CopyCameraLocation : public IViewportTool
{
    Q_OBJECT
    friend class CopyCameraLocationWidget;

public:
    CopyCameraLocation(const Ptr<StateRecorder>& pStateRecorder, const GuiDocument& gDoc, const Core::List<Ptr<Property> > pties, bool attach);
    virtual ~CopyCameraLocation();

    virtual Ptr<IViewportToolWidget> createViewportToolWidget(QWidget * parent);
    virtual void guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc);

    virtual const Core::List<Ptr<Property> >& getProperties() const;
    virtual Core::List<Ptr<Property> >& getProperties();

    virtual void mouseMoveEvent         (QMouseEvent* event,const DrawContext& dc) {};
    virtual void mousePressEvent        (QMouseEvent* event,const DrawContext& dc) {};
    virtual void mouseReleaseEvent      (QMouseEvent* event,const DrawContext& dc) {};
    virtual void mouseMiddleSingleClick (QMouseEvent* event, const DrawContext& dc) {};

    virtual bool isHighlighted() const;
    virtual bool isSelected() const;

    virtual EViewportToolType   getType() const {return COPY_CAMERA_LOCATION_TOOL;}
    void setContainer(const Ptr<Property>& pContainer);
    
    void apply();

    static void toCameraLocation(float distance, const Ptr<Universe::NodeCamera>& pNCam, const Core::List<Ptr<Property> > pties, Ptr<StateRecorder> pStateRecorder, bool findPtyTransform = true, bool makeUndo = false);
    virtual void updateCameraMove  (Ptr<Universe::NodeCamera> pNCam);
    void makeAttach(bool doAttach);

private:
    void updatePropertiesTargets(const Core::List<Ptr<Property> >& ptiesSelected);
    void computeActionCenters();
    void computeActionOrientations();

public Q_SLOTS:
    virtual void selectionChanged(const Core::List<Ptr<Property> >& pties);
    virtual void updateViewportTool();

private:
    struct implementation;
	implementation* const _impl;
};
//-----------------------------------------------------------------------------
class LM_API_EPI CopyCameraLocationWidget : public IViewportToolWidget
{
    Q_OBJECT

public:
    CopyCameraLocationWidget(CopyCameraLocation& tool, QWidget * parent);
    virtual ~CopyCameraLocationWidget();
    virtual IViewportTool& getViewportTool();

private:
    void setupUi();
    virtual void readViewportTool();
    virtual void writeViewportTool(QWidget* pWidget = NULL);

private Q_SLOTS:
    void applyLocation();
    void makeAttach(bool doAttach);


private:
    CopyCameraLocation&             _tool;
    QGridLayout *                   _layout;
    QtToolbox::SingleSlidingDouble* _distance;
    QPushButton*                    _apply;
    QPushButton*                    _lockPtiesToCamera;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif //EPI_COPY_CAMERA_LOCATION_TOOL_H_