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
#include <EPI/GUI/Widget/ToolsTransform.moc.h>

#include <QAction>

#include <EPI/GUI/Viewport/Tools/TransformViewportTool.moc.h>
#include <EPI/GUI/GuiDocument.moc.h>
#include <EPI/Document/DocumentBase.moc.h>
#include <EPI/DocumentRenderer.h>

namespace EPI
{
//-----------------------------------------------------------------------------
ToolsTransform::ToolsTransform(Ptr<GuiDocument>& pGDoc, QWidget* parent) :
    PushButtonMenu(parent, Core::microseconds(0.1*1000000)),
    _pGDoc(pGDoc)
{
    QList<QAction*> actions;
        actions.push_back(new QAction(QIcon(":/icons/moveRotateScale.png"), "MoveAndRotate", this));
        actions.push_back(new QAction(QIcon(":/icons/move.png"),"Move", this));
        actions.push_back(new QAction(QIcon(":/icons/rotate.png"),"Rotate", this));
        actions.push_back(new QAction(QIcon(":/icons/scale.png"),"Scale", this));
        actions.push_back(new QAction(QIcon(":/icons/copyLocation.png"),"CopyLocation", this));
        actions.push_back(new QAction(QIcon(":/icons/copyCameraLocation.png"),"CopyCameraLocation", this));


    //QtToolbox::PushButtonMenu *boutontest = new QtToolbox::PushButtonMenu(actions, this);
    setActions(actions);
    setFlat(true);


    connect(this, SIGNAL(actionChanged(int32, bool)), this, SLOT(internalToolChanged(int32, bool)));


    //setCheckable(true); 

    addActions(_actions);
    connectSignals(true);
}
//-----------------------------------------------------------------------------
ToolsTransform::~ToolsTransform()
{
}
//-----------------------------------------------------------------------------
void ToolsTransform::connectSignals(bool flag)
{
    if (flag == true)
    {
        connect(_pGDoc.get(), SIGNAL(signalActivatedViewportTool(const Ptr<IViewportTool>&)), this, SLOT(guiDocChangeVTool(const Ptr<IViewportTool>&)));
    }
    else
    {
        disconnect(_pGDoc.get(), SIGNAL(signalActivatedViewportTool(const Ptr<IViewportTool>&)), this, SLOT(guiDocChangeVTool(const Ptr<IViewportTool>&)));
    }
}
//-----------------------------------------------------------------------------
void ToolsTransform::guiDocChangeVTool(const Ptr<IViewportTool>& pVTool)
{
    if (pVTool==null)
    {
        setDown(false);
    }
    else
    {
        if (pVTool->getType() == MULTI_TRANSFORM_TOOL   ||
            pVTool->getType() == MOVE_TOOL              ||
            pVTool->getType() == ROTATE_TOOL            ||
            pVTool->getType() == SCALE_TOOL             ||
            pVTool->getType() == COPY_LOCATION_TOOL     ||
            pVTool->getType() == COPY_CAMERA_LOCATION_TOOL)
        {
            setDown(true);
            
            switch(pVTool->getType())
            {
            case MULTI_TRANSFORM_TOOL:          actualizeIHM(0, true); break;
            case MOVE_TOOL:                     actualizeIHM(1, true); break;
            case ROTATE_TOOL:                   actualizeIHM(2, true); break;
            case SCALE_TOOL:                    actualizeIHM(3, true); break;
            case COPY_LOCATION_TOOL:            actualizeIHM(4, true); break;
            case COPY_CAMERA_LOCATION_TOOL:     actualizeIHM(5, true); break;
            }
        }
        else
        {
            setDown(false);
        }
    }
}
//-----------------------------------------------------------------------------
void ToolsTransform::internalToolChanged(int32 index, bool isChecked)
{
    connectSignals(false);
    if (isChecked==true)
    {
        switch(index)
        {
        case 0:
            _pGDoc->selectMultiTransformVTool();
            break;
        case 1:
            _pGDoc->selectMoveVTool();
            break;
        case 2: 
            _pGDoc->selectRotateVTool();
            break;
        case 3:
            _pGDoc->selectScaleVTool();
            break;
        case 4:
            _pGDoc->selectCopyLocationVTool();
            break;
        case 5:
            _pGDoc->selectCopyCameraLocationVTool();
            break;
        }
    }
    else
    {
        emit signalToolChanged(null);
    }
    connectSignals(true);
}
//-----------------------------------------------------------------------------
} //namespace EPI
