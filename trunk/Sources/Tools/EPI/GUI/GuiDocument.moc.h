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
#ifndef GUI_DOCUMENT_H_
#define GUI_DOCUMENT_H_

#include <QObject>
#include <Core/Standard.h>
#include <Universe/Tasks/ITask.h>
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class QAction;
class QTimer;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace Universe
{
    class World;
    class NodeCamera;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace Core
{
    template <class TElement> class List;
    struct AABoxf;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace EPI
{
class   WorldOverlay;
class   GuiContext;
class   IViewportTool;
class   Property;
class   DocumentBase;
struct  ImportInfo;
struct  DrawContext;
enum    EViewportCameraNavigation;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct ISpyTask;
class LM_API_EPI ICallBackTask
{
public:
    typedef  void (ICallBackTask::*callBack)(const Ptr<Universe::ITask>& pTask) ;
    
    virtual void signalEndTask(const Ptr<Universe::ITask>& pTask) {}

    inline Ptr<ISpyTask> createSpyTaskForEndTask(const Ptr<Universe::ITask>& pTask);
};

struct LM_API_EPI ISpyTask
{
    virtual bool update() = 0;

    Ptr<Universe::ITask> pTask;
    ICallBackTask::callBack call;
    ICallBackTask*  inst;
};

struct LM_API_EPI SpyTaskForEndTask : public ISpyTask
{
    virtual bool update();
};

inline Ptr<ISpyTask> ICallBackTask::createSpyTaskForEndTask(const Ptr<Universe::ITask>& pTask)
{
    Ptr<SpyTaskForEndTask> sp (new SpyTaskForEndTask());
        sp->pTask=pTask;
        sp->call=&ICallBackTask::signalEndTask;
        sp->inst = this;
    return sp;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI GuiDocument : public QObject, public Core::Object
{
    Q_OBJECT

public:
    GuiDocument();
    virtual ~GuiDocument();

    void setDocument(const Ptr<DocumentBase>& pDoc);

    const   Ptr<DocumentBase>&    getDocument   () const;
            Ptr<DocumentBase>&    getDocument   ();

            int32                 getID         ();
            int32                 getRendererId ();
    /*
	        bool                  canCloseUserInteraction();

            */
            bool                  canClose              (bool userInteraction);


    const   Ptr<Universe::World>&               getGuiWorld       () const;
            Ptr<Universe::World>                getWorldOverlay   (const DrawContext& dc) const;
    const   Ptr<WorldOverlay>&                  getWorldOverlay2  () const;  //TODO a effacer
            Core::List<Ptr<Universe::World> >   getWorlds         (const DrawContext& dc) const;

            Ptr<GuiContext>&      getGuiContext();
    const   Ptr<GuiContext>&      getGuiContext() const;


    const   Ptr<IViewportTool>& getViewportTool();
            Ptr<Property>       getPtyForEdition() const;
            
            QAction*            getActionExtend();

    bool    isAnimated() const;

    void addSpyTask(const Ptr<ISpyTask>& sp);

    void setMakeSleep(bool make);

    void restart();
    void stop();

    void activeDesactiveIHM();
private:
    void prepareWorldOverlayForRendering(const DrawContext& dc) const;

public Q_SLOTS:
    void setViewportTool    (const Ptr<IViewportTool>& pVTool);
    void startAnimate       ();
    void stopAnimate        ();
    void animate            ();
    void generateInfo       (const Ptr<ImportInfo> & pImportInfo);
    void zoomExtent         ();
    void documentContentIsModified();
    void selectMultiTransformVTool();
    void selectMoveVTool();
    void selectRotateVTool();
    void selectScaleVTool();
    void selectCopyLocationVTool();
    void selectCopyCameraLocationVTool(bool makeAttach = false);
    void copyCameraLocationToCurrentSelection();
    void engageCameraLocationAttach();

private Q_SLOTS:
    void SelectionChanged(const Core::List<Ptr<Property> >&  pties);
    
Q_SIGNALS:
    void signalActivatedViewportTool    (const Ptr<IViewportTool>& pVTool);
    void generate                       (const Ptr<ImportInfo> & pImportInfo);
    void reDrawMe                       ();
    void playStopChanged                ();

private:
	class implementation;
	implementation* const _impl;
    QTimer *_pTimer;
    
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} //namespace EPI

#endif //GUI_DOCUMENT_H_
