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
#ifndef GUI_CONTEXT_H_
#define GUI_CONTEXT_H_

#include <QObject>


#include <Core/Standard.h>
#include <Core/Math/Vector.h>

#include <EPI/Document/GuiDescription.h>
#include <Universe/NodeListener.h>

class QAction;
class QActionGroup;

namespace EPI
{
enum EActionCenterType;
enum EOrientationType;
class CameraManager;
class WorldViewport;
class WorldViewportContainer;
class INavigationStyle;
class IViewportTool;
class Property;
//-----------------------------------------------------------------------------
struct LM_API_EPI ViewportDesc
{
    ViewportDesc();

    int32                       _id;

    Core::Vector2f              _size;

    EViewportRenderMode         _renderMode;
    EViewportPolygonMode        _polygonMode;
    EViewportShadowMode         _shadowMode;
    EViewportRenderStatsMode    _renderStatsMode;


    EViewportCameraType         _cameraType;
    EViewportCameraNavigation   _cameraNavigation;
    EViewportCameraView         _cameraView;
    EViewportPickingType        _pickingType;
    String                      _tagCamera;
    bool                        _isSelected;
    bool                        _showIHM;
    ECameraAspectRatio          _aspectRatio;
    int32                       _idCameraContent;
};

class LM_API_EPI ViewportContext : public Core::Object
{
public:
    ViewportContext(EViewportLayoutMode         mode,
                    EViewportCameraType         camType,
                    EViewportCameraNavigation   camNav,
                    EViewportCameraView         camView,
                    EViewportPickingType        pickType);
    virtual ~ViewportContext();

    EViewportLayoutMode getActivateMode() const {return _activeMode;}
    void setActivateMode(EViewportLayoutMode mode) {_activeMode=mode;}

    const Core::List<ViewportDesc>& getDescs(EViewportLayoutMode mode) const {return _contexts[mode];}
    Core::List<ViewportDesc>& getDescs(EViewportLayoutMode mode) {return _contexts[mode];}
    void setDescs(EViewportLayoutMode mode, const Core::List<ViewportDesc>& context) {_contexts[mode] = context;}

private:
    EViewportLayoutMode _activeMode;
    Core::List<ViewportDesc> _contexts[EViewportLayoutMode_COUNT];
};

struct MemoViewportTool
{
    MemoViewportTool();
    void stockViewportToolConfig(const Ptr<IViewportTool>& pVTool);

    EActionCenterType vToolActionCenter;
    EOrientationType  vToolOrientationType;
};

struct pickingFilterParam
{
    pickingFilterParam() : 
        waterPatch(false),
        camera(false),
        playerPosition(true),
        sound(false),
        instance(true),
        decal(true),
        pointLight(true),
        skeleton(true),
        trail(true),
        emitter(false),
        terrain(false)
        {}

    bool waterPatch;
    bool camera;
    bool playerPosition;
    bool sound;
    bool instance;
    bool decal;
    bool pointLight;
    bool skeleton;
    bool trail;
    bool emitter;
    bool terrain;
};
//-----------------------------------------------------------------------------
/**
* 
*/
class LM_API_EPI GuiContext : public QObject, public Core::Object
{
    Q_OBJECT

public:
    GuiContext(const Ptr<GuiDescription>&   pGuiDesc, const Ptr<CameraManager>& pCamMng);
    virtual ~GuiContext();

    const Ptr<CameraManager>& getCameraManager() const;
    const ViewportContext& getViewportContext() const {return _viewports;}
    ViewportContext& getViewportContext() {return _viewports;}

    const QList<QAction*>& getActionsViewportLayoutMode()       const;
    const QList<QAction*>& getActionsViewportCameraType()       const;
    const QList<QAction*>& getActionsViewportCameraNavigation() const;
    const QList<QAction*>& getActionsViewportCameraView()       const;
    const QList<QAction*>& getActionsViewportRenderMode()       const;
    const QList<QAction*>& getActionsViewportPolygonMode()      const;
    const QList<QAction*>& getActionsViewportCameraAspectRatio() const;
    const QList<QAction*>& getActionsViewportShadowMode()       const;
    const QList<QAction*>& getActionsViewportRenderStatsMode()  const;

    void setCurrentViewport(WorldViewportContainer* pView, Ptr<Universe::NodeListener> pNodeL);
    WorldViewportContainer* getCurrentViewport() const;

    void setActiveCameraContent(int32 idPtyCam);

    void setCurrentViewportCameraView(EViewportCameraView view);
    MemoViewportTool& getMemoViewportTool() {return _memoVTool;}
    
    bool pickingFilterActivated() const {return _pickingFilterisOn;}
    void setPickingFilter(bool flag) {_pickingFilterisOn = flag;}
    void setPickingFilterParam(const pickingFilterParam& param) { _pickingFilterParam = param;}
    const pickingFilterParam& getPickingFilterParam() const {return _pickingFilterParam;}
    bool propertyIsSelectable(const Property& pty) const;

public Q_SLOTS:
    void setCurrentViewportRenderModeNormal();
    void setCurrentViewportRenderModeHQPreview();
    void setCurrentViewportRenderModeShawdowMap();
    void setCurrentViewportRenderModeUV();
    void setCurrentViewportRenderModeDebugNormal();
    void setCurrentViewportRenderModeT();
    void setCurrentViewportRenderModeB();
    void setCurrentViewportRenderModeN();
    void setCurrentViewportRenderModeMIP();
    void setCurrentViewportPolygonModeSolid();
    void setCurrentViewportPolygonModeSolidWireframe();
    void setCurrentViewportPolygonModeWireframe();
    void setCurrentViewportShadowModeOn();
    void setCurrentViewportShadowModeOff();
    void setCurrentViewportLayoutSingle();
    void setCurrentViewportLayoutQuadruple();
    void setCurrentViewportLayoutDoubleVertical();
    void setCurrentViewportCameraTypePerspective();
    void setCurrentViewportCameraTypeOrthogonal();
    void setCurrentViewportCameraNavigationQuake();
    void setCurrentViewportCameraNavigationOrbital();
    void setCurrentViewportCameraViewTop();
    void setCurrentViewportCameraViewBottom();
    void setCurrentViewportCameraViewBack();
    void setCurrentViewportCameraViewFront();
    void setCurrentViewportCameraViewRight();
    void setCurrentViewportCameraViewLeft();
    void setCurrentViewportCameraViewUser();
    void setCurrentViewportCameraViewEdition();
    void setCurrentViewportCameraAspectRatioFree();
    void setCurrentViewportCameraAspectRatio_5_4();
    void setCurrentViewportCameraAspectRatio_4_3();
    void setCurrentViewportCameraAspectRatio_3_2();
    void setCurrentViewportCameraAspectRatio_16_10();
    void setCurrentViewportCameraAspectRatio_16_9();
    void setCurrentViewportRenderStatsOn();
    void setCurrentViewportRenderStatsOff();
    void setCurrentViewportActiveDesactiveShowIHM();
Q_SIGNALS:
    void currentViewportLayoutChanged();

private:
    ViewportContext     _viewports;
    Ptr<CameraManager>  _pCamMng;

    QList <QAction*>    _actionsViewportLayoutMode;
    QList <QAction*>    _actionsViewportCameraType;
    QList <QAction*>    _actionsViewportCameraAspectRatio;
    QList <QAction*>    _actionsViewportCameraNavigation;
    QList <QAction*>    _actionsViewportCameraView;
    QList <QAction*>    _actionsViewportRenderMode;
    QList <QAction*>    _actionsViewportPolygonMode;
    QList <QAction*>    _actionsViewportShadowMode;
    QList <QAction*>    _actionsViewportRenderStatsMode;

    QAction*            _actionCurrent;

    QActionGroup*       _actionGroupLayoutMode;
    QActionGroup*       _actionGroupCameraType;
    QActionGroup*       _actionGroupCameraNavigation;

    QActionGroup*       _actionGroupRenderMode;
    QActionGroup*       _actionGroupPolygonMode;
    QActionGroup*       _actionGroupShadowMode;
    QActionGroup*       _actionGroupRenderStatsMode;

    WorldViewportContainer*     _pCurrentView;

    MemoViewportTool            _memoVTool;
    bool                        _pickingFilterisOn;
    pickingFilterParam          _pickingFilterParam;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif GUI_CONTEXT_H_