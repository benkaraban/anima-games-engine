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
#include "GuiContext.moc.h"

#include <QAction>

#include <EPI/GUI/Viewport/WorldViewport.moc.h>
#include <EPI/GUI/Viewport/CameraManager.h>

#include <EPI/Gui/Viewport/Tools/TransformViewportTool.moc.h>
#include <EPI/Constants.h>
#include <EPI/Document/Property.moc.h>
namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewportDesc::ViewportDesc():
    _size(0,0),
    _renderMode(VIEWPORT_RDR_NORMAL),
    _polygonMode(VIEWPORT_POLY_SOLID),
    _shadowMode(VIEWPORT_SHADOW_ON),
    _renderStatsMode(VIEWPORT_RENDERSTATS_OFF),
    _cameraType(VIEWPORT_CAMERA_PERSPECTIVE),
    _cameraNavigation(VIEWPORT_CAMERA_QUAKE),
    _cameraView(VIEWPORT_CAMERA_USER),
    _pickingType(VIEWPORT_PICKING_DEFAULT),
    _isSelected(false),
    _showIHM(true),
    _aspectRatio(CAMERA_RATIO_FREE),
    _idCameraContent(-1)
{

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Core::Vector2f getViewportDefaultSize(EViewportLayoutMode mode, int32 numViewport)
{
    Core::Vector2f ret;

    switch(mode)
    {
    case SINGLE:
        ret.x = 1.f;
        ret.y = 1.f;
        break;

    case QUADRUPLE:
        ret.x = 0.5f;
        ret.y = 0.5f;
        break;

    case DOUBLE_VERTICAL:
        ret.x = 0.5f;
        ret.y = 1.f;
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewportContext::ViewportContext(EViewportLayoutMode        mode,
                                 EViewportCameraType        camType,
                                 EViewportCameraNavigation  camNav,
                                 EViewportCameraView        camView,
                                 EViewportPickingType       pickType):
    _activeMode(mode)
{
    for (int32 iMode=0; iMode<EViewportLayoutMode_COUNT; ++iMode)
    {
        _contexts[iMode].resize(getNbViewports((EViewportLayoutMode)iMode));

        for (int32 iView=0; iView<_contexts[iMode].size(); ++iView)
        {
            _contexts[iMode][iView]._id = iView;
            _contexts[iMode][iView]._size = getViewportDefaultSize((EViewportLayoutMode)iMode, iView);
            _contexts[iMode][iView]._cameraType = camType;
            _contexts[iMode][iView]._cameraView = camView;
            _contexts[iMode][iView]._cameraNavigation = camNav;
            _contexts[iMode][iView]._isSelected = false;
            _contexts[iMode][iView]._pickingType = pickType;
        }
    }
/*
    for (int32 iMode=0; iMode<EViewportLayoutMode_COUNT; ++iMode)
    {
        _contexts[iMode][0]._isSelected = true;
    }*/

    _contexts[QUADRUPLE][0]._cameraView = VIEWPORT_CAMERA_TOP;
    _contexts[QUADRUPLE][1]._cameraView = VIEWPORT_CAMERA_FRONT;
    _contexts[QUADRUPLE][2]._cameraView = VIEWPORT_CAMERA_USER;
    _contexts[QUADRUPLE][3]._cameraView = VIEWPORT_CAMERA_RIGHT;
}
//-----------------------------------------------------------------------------
ViewportContext::~ViewportContext()
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MemoViewportTool::MemoViewportTool()
{
    vToolActionCenter             = FREE_USER_CENTER_ROTATE_BOXCENTER;
    vToolOrientationType          = LOCAL_ORIENTATION;
}
//-----------------------------------------------------------------------------
void MemoViewportTool::stockViewportToolConfig(const Ptr<IViewportTool>& pVTool)
{
    if (pVTool!=null)
    {
        switch(pVTool->getType())
        {
        case MULTI_TRANSFORM_TOOL:
        case MOVE_TOOL:
        case ROTATE_TOOL:
            {
                Ptr<TransformTool> pTTool      = LM_DEBUG_PTR_CAST<TransformTool>(pVTool);
                vToolActionCenter           = pTTool->getActionCenterType();
                vToolOrientationType        = pTTool->getActionOrientationType();
            }
            break;
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GuiContext::GuiContext(const Ptr<GuiDescription>&   pGuiDesc, const Ptr<CameraManager>& pCamMng) :
    _viewports(pGuiDesc->getViewportLayoutModes()[0], pGuiDesc->getViewportCameraTypes()[0],
    pGuiDesc->getViewportCameraNavigations()[0], pGuiDesc->getViewportCameraViews()[0],
    pGuiDesc->getViewportPickingTypes()[0]),
    _pCamMng(pCamMng),
    _pCurrentView(NULL),
    _pickingFilterisOn(false)
{
    //viewport Layout
    const Core::List<EViewportLayoutMode>&  listLayoutModes = pGuiDesc->getViewportLayoutModes();
    for (int32 iMode=0; iMode<listLayoutModes.size(); ++iMode)
    {
        EViewportLayoutMode mode = listLayoutModes[iMode];
        switch(mode)
        {
            case SINGLE:
                _actionsViewportLayoutMode.push_back(new QAction("Single", this));
                connect(_actionsViewportLayoutMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportLayoutSingle()));
                break;
            case QUADRUPLE:
                _actionsViewportLayoutMode.push_back(new QAction("Quadruple", this));
                connect(_actionsViewportLayoutMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportLayoutQuadruple()));
                break;
            case DOUBLE_VERTICAL:
                _actionsViewportLayoutMode.push_back(new QAction("Double Vertical", this));
                connect(_actionsViewportLayoutMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportLayoutDoubleVertical()));
                break;
        }

        _actionsViewportLayoutMode.back()->setCheckable(true);
    }
    _actionGroupLayoutMode = new QActionGroup(this);
    for (int32 iAction=0; iAction<_actionsViewportLayoutMode.size(); ++iAction)
    {
        _actionGroupLayoutMode->addAction(_actionsViewportLayoutMode[iAction]);
    }
    _actionGroupLayoutMode->setExclusive(true);
    _actionsViewportLayoutMode[0]->setChecked(true);

    //cameraType
    const Core::List<EViewportCameraType>&  listCameraTypes = pGuiDesc->getViewportCameraTypes();
    for (int32 iMode=0; iMode<listCameraTypes.size(); ++iMode)
    {
        EViewportCameraType mode = listCameraTypes[iMode];
        switch(mode)
        {
            case VIEWPORT_CAMERA_PERSPECTIVE:
                _actionsViewportCameraType.push_back(new QAction("Perspective", this));
                connect(_actionsViewportCameraType.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraTypePerspective()));
                break;
            case VIEWPORT_CAMERA_ORTHOGONAL:
                _actionsViewportCameraType.push_back(new QAction("Orthogonal", this));
                connect(_actionsViewportCameraType.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraTypeOrthogonal()));
                break;
        }
    }
    _actionGroupCameraType = new QActionGroup(this);
    for (int32 iAction=0; iAction<_actionsViewportCameraType.size(); ++iAction)
    {
        _actionGroupCameraType->addAction(_actionsViewportCameraType[iAction]);
    }

    //cameraNavigation
    const Core::List<EViewportCameraNavigation>&  listCameraNavigations = pGuiDesc->getViewportCameraNavigations();
    for (int32 iMode=0; iMode<listCameraNavigations.size(); ++iMode)
    {
        EViewportCameraNavigation mode = listCameraNavigations[iMode];
        switch(mode)
        {
            case VIEWPORT_CAMERA_QUAKE:
                _actionsViewportCameraNavigation.push_back(new QAction("Quake", this));
                connect(_actionsViewportCameraNavigation.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraNavigationQuake()));
                break;
            case VIEWPORT_CAMERA_ORBITAL:
                _actionsViewportCameraNavigation.push_back(new QAction("Orbital", this));
                connect(_actionsViewportCameraNavigation.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraNavigationOrbital()));
                break;
        }
    }
    _actionGroupCameraNavigation = new QActionGroup(this);
    for (int32 iAction=0; iAction<_actionsViewportCameraNavigation.size(); ++iAction)
    {
        _actionGroupCameraNavigation->addAction(_actionsViewportCameraNavigation[iAction]);
    }

    //cameraView
    const Core::List<EViewportCameraView>&  listCameraViews = pGuiDesc->getViewportCameraViews();
    for (int32 iMode=0; iMode<listCameraViews.size(); ++iMode)
    {
        EViewportCameraView mode = listCameraViews[iMode];
        switch(mode)
        {
            case VIEWPORT_CAMERA_TOP:
                _actionsViewportCameraView.push_back(new QAction(StrToQStr(VIEW_TYPE_NAME_TOP), this));
                connect(_actionsViewportCameraView.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraViewTop()));
                break;
            case VIEWPORT_CAMERA_BOTTOM:
                _actionsViewportCameraView.push_back(new QAction(StrToQStr(VIEW_TYPE_NAME_BOTTOM), this));
                connect(_actionsViewportCameraView.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraViewBottom()));
                break;
            case VIEWPORT_CAMERA_BACK:
                _actionsViewportCameraView.push_back(new QAction(StrToQStr(VIEW_TYPE_NAME_BACK), this));
                connect(_actionsViewportCameraView.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraViewBack()));
                break;
            case VIEWPORT_CAMERA_FRONT:
                _actionsViewportCameraView.push_back(new QAction(StrToQStr(VIEW_TYPE_NAME_FRONT), this));
                connect(_actionsViewportCameraView.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraViewFront()));
                break;
            case VIEWPORT_CAMERA_RIGHT:
                _actionsViewportCameraView.push_back(new QAction(StrToQStr(VIEW_TYPE_NAME_RIGHT), this));
                connect(_actionsViewportCameraView.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraViewRight()));
                break;
            case VIEWPORT_CAMERA_LEFT:
                _actionsViewportCameraView.push_back(new QAction(StrToQStr(VIEW_TYPE_NAME_LEFT), this));
                connect(_actionsViewportCameraView.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraViewLeft()));
                break;
            case VIEWPORT_CAMERA_USER:
                _actionsViewportCameraView.push_back(new QAction(StrToQStr(VIEW_TYPE_NAME_USER), this));
                connect(_actionsViewportCameraView.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraViewUser()));
                break;
        }
    }


    //camera AspectRatio
    _actionsViewportCameraAspectRatio.push_back(new QAction(StrToQStr(ASPECT_RATIO_NAME_FREE), this));
        connect(_actionsViewportCameraAspectRatio.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraAspectRatioFree()));
  /*  _actionsViewportCameraAspectRatio.push_back(new QAction(StrToQStr(ASPECT_RATIO_NAME_5_4), this));
        connect(_actionsViewportCameraAspectRatio.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraAspectRatio_5_4()));*/
    _actionsViewportCameraAspectRatio.push_back(new QAction(StrToQStr(ASPECT_RATIO_NAME_4_3), this));
        connect(_actionsViewportCameraAspectRatio.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraAspectRatio_4_3()));
    /*_actionsViewportCameraAspectRatio.push_back(new QAction(StrToQStr(ASPECT_RATIO_NAME_3_2), this));
        connect(_actionsViewportCameraAspectRatio.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraAspectRatio_3_2()));
    _actionsViewportCameraAspectRatio.push_back(new QAction(StrToQStr(ASPECT_RATIO_NAME_16_10), this));
        connect(_actionsViewportCameraAspectRatio.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraAspectRatio_16_10()));*/
    _actionsViewportCameraAspectRatio.push_back(new QAction(StrToQStr(ASPECT_RATIO_NAME_16_9), this));
        connect(_actionsViewportCameraAspectRatio.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportCameraAspectRatio_16_9()));

    //renderMode
    const Core::List<EViewportRenderMode>&  listRenderModes = pGuiDesc->getViewportRenderModes();
    for (int32 iMode=0; iMode<listRenderModes.size(); ++iMode)
    {
        EViewportRenderMode mode = listRenderModes[iMode];

        switch(mode)
        {
            case VIEWPORT_RDR_NORMAL:
                _actionsViewportRenderMode.push_back(new QAction(StrToQStr(RENDER_MODE_NAME_RGB), this));
                connect(_actionsViewportRenderMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderModeNormal()));
                break;
            case VIEWPORT_RDR_HQ_PREVIEW:
                _actionsViewportRenderMode.push_back(new QAction(StrToQStr(RENDER_MODE_NAME_HQ_PREVIEW), this));
                connect(_actionsViewportRenderMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderModeHQPreview()));
                break;
            case VIEWPORT_RDR_DEBUG_SHADOW_MAP:
                _actionsViewportRenderMode.push_back(new QAction(StrToQStr(RENDER_MODE_NAME_SHADOWMAP), this));
                connect(_actionsViewportRenderMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderModeShawdowMap()));
                break;
            case VIEWPORT_RDR_DEBUG_UV:
                _actionsViewportRenderMode.push_back(new QAction(StrToQStr(RENDER_MODE_NAME_UV), this));
                connect(_actionsViewportRenderMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderModeUV()));
                break;
            case VIEWPORT_RDR_DEBUG_NORMAL:
                _actionsViewportRenderMode.push_back(new QAction(StrToQStr(RENDER_MODE_NAME_NORMAL), this));
                connect(_actionsViewportRenderMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderModeDebugNormal()));
                break;
            case VIEWPORT_RDR_DEBUG_T:
                _actionsViewportRenderMode.push_back(new QAction(StrToQStr(RENDER_MODE_NAME_T), this));
                connect(_actionsViewportRenderMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderModeT()));
                break;
            case VIEWPORT_RDR_DEBUG_B:
                _actionsViewportRenderMode.push_back(new QAction(StrToQStr(RENDER_MODE_NAME_B), this));
                connect(_actionsViewportRenderMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderModeB()));
                break;
            case VIEWPORT_RDR_DEBUG_N:
                _actionsViewportRenderMode.push_back(new QAction(StrToQStr(RENDER_MODE_NAME_N), this));
                connect(_actionsViewportRenderMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderModeN()));
                break;
            case VIEWPORT_RDR_DEBUG_MIP:
                _actionsViewportRenderMode.push_back(new QAction(StrToQStr(RENDER_MODE_NAME_MIP), this));
                connect(_actionsViewportRenderMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderModeMIP()));
                break;
        }
    }
    _actionGroupRenderMode = new QActionGroup(this);
    for (int32 iAction=0; iAction<_actionsViewportRenderMode.size(); ++iAction)
    {
        _actionGroupRenderMode->addAction(_actionsViewportRenderMode[iAction]);
    }

    //polygon mode
    const Core::List<EViewportPolygonMode>&  listPolygonModes = pGuiDesc->getViewportPolygonModes();
    for (int32 iMode=0; iMode<listPolygonModes.size(); ++iMode)
    {
        EViewportPolygonMode mode = listPolygonModes[iMode];
        switch(mode)
        {
            case VIEWPORT_POLY_SOLID:
                _actionsViewportPolygonMode.push_back(new QAction(StrToQStr(POLYGON_MODE_NAME_SOLID), this));
                connect(_actionsViewportPolygonMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportPolygonModeSolid()));
                break;
            case VIEWPORT_POLY_SOLID_WIREFRAME:
                _actionsViewportPolygonMode.push_back(new QAction(StrToQStr(POLYGON_MODE_NAME_SOLID_WIREFRAME), this));
                connect(_actionsViewportPolygonMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportPolygonModeSolidWireframe()));
                break;
            case VIEWPORT_POLY_WIREFRAME:
                _actionsViewportPolygonMode.push_back(new QAction(StrToQStr(POLYGON_MODE_NAME_POLY_WIREFRAME), this));
                connect(_actionsViewportPolygonMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportPolygonModeWireframe()));
                break;
        }
    }
    _actionGroupPolygonMode = new QActionGroup(this);
    for (int32 iAction=0; iAction<_actionsViewportPolygonMode.size(); ++iAction)
    {
        _actionGroupPolygonMode->addAction(_actionsViewportPolygonMode[iAction]);
    }


    //shadow mode
    const Core::List<EViewportShadowMode>&  listShadowModes = pGuiDesc->getViewportShadowModes();
    for (int32 iMode=0; iMode<listShadowModes.size(); ++iMode)
    {
        EViewportShadowMode mode = listShadowModes[iMode];

        switch(mode)
        {
            case VIEWPORT_SHADOW_ON:
                _actionsViewportShadowMode.push_back(new QAction("ShadowOn", this));
                connect(_actionsViewportShadowMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportShadowModeOn()));
                break;
            case VIEWPORT_SHADOW_OFF:
                _actionsViewportShadowMode.push_back(new QAction("ShadowOff", this));
                connect(_actionsViewportShadowMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportShadowModeOff()));
                break;
        }
    }
    _actionGroupShadowMode = new QActionGroup(this);
    for (int32 iAction=0; iAction<_actionsViewportShadowMode.size(); ++iAction)
    {
        _actionGroupShadowMode->addAction(_actionsViewportShadowMode[iAction]);
    }

    //render stats
    const Core::List<EViewportRenderStatsMode>& renderStatsModes = pGuiDesc->getViewportRenderStatsModes();
    for(int32 iMode=0; iMode < renderStatsModes.size(); iMode++)
    {
        switch(renderStatsModes[iMode])
        {
        case VIEWPORT_RENDERSTATS_ON:
            _actionsViewportRenderStatsMode.push_back(new QAction("RenderStatsOn", this));
            connect(_actionsViewportRenderStatsMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderStatsOn()));
            break;
        case VIEWPORT_RENDERSTATS_OFF:
            _actionsViewportRenderStatsMode.push_back(new QAction("RenderStatsOff", this));
            connect(_actionsViewportRenderStatsMode.back(), SIGNAL(triggered()), this, SLOT(setCurrentViewportRenderStatsOff()));
            break;
        }
    }
    _actionGroupRenderStatsMode = new QActionGroup(this);
    for (int32 iAction=0; iAction<_actionsViewportRenderStatsMode.size(); ++iAction)
    {
        _actionGroupRenderStatsMode->addAction(_actionsViewportRenderStatsMode[iAction]);
    }
}
//-----------------------------------------------------------------------------
GuiContext::~GuiContext()
{
}
//-----------------------------------------------------------------------------
bool GuiContext::propertyIsSelectable(const Property& pty) const
{
    if (_pickingFilterisOn == false)    return true;

    switch(pty.getCalssName())
    {
    case PTYNODE_WATER_PATCH:       return _pickingFilterParam.waterPatch;      break;
    case PTYNODE_CAMERA_CLASS:      return _pickingFilterParam.camera;          break;
    case PTYNODE_PLAYER_POSITION:   return _pickingFilterParam.playerPosition;  break;
    case PTYNODE_SOUND:             return _pickingFilterParam.sound;           break;
    case PTYNODE_INSTANCE:          return _pickingFilterParam.instance;        break;
    case PTYNODE_DECAL:             return _pickingFilterParam.decal;           break;
    case PTYNODE_POINT_LIGHT:       return _pickingFilterParam.pointLight;      break;
    case PTYNODE_SKELETON:          return _pickingFilterParam.skeleton;        break;
    case PTYNODE_TRAIL:             return _pickingFilterParam.trail;           break;
    case PTYNODE_EMITTER:           return _pickingFilterParam.emitter;         break;
    case PTYNODE_TERRAIN:           return _pickingFilterParam.terrain;         break;
    default:
        return true;
    }

    return true;
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderModeNormal()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderMode = VIEWPORT_RDR_NORMAL;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderModeShawdowMap()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderMode = VIEWPORT_RDR_DEBUG_SHADOW_MAP;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderModeHQPreview()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderMode = VIEWPORT_RDR_HQ_PREVIEW;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderModeUV()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderMode = VIEWPORT_RDR_DEBUG_UV;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderModeDebugNormal()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderMode = VIEWPORT_RDR_DEBUG_NORMAL;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderModeT()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderMode = VIEWPORT_RDR_DEBUG_T;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderModeB()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderMode = VIEWPORT_RDR_DEBUG_B;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderModeN()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderMode = VIEWPORT_RDR_DEBUG_N;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderModeMIP()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderMode = VIEWPORT_RDR_DEBUG_MIP;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportPolygonModeSolid()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._polygonMode = VIEWPORT_POLY_SOLID;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportPolygonModeSolidWireframe()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._polygonMode = VIEWPORT_POLY_SOLID_WIREFRAME;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportPolygonModeWireframe()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._polygonMode = VIEWPORT_POLY_WIREFRAME;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportShadowModeOn()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._shadowMode = VIEWPORT_SHADOW_ON;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportShadowModeOff()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._shadowMode = VIEWPORT_SHADOW_OFF;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderStatsOn()
{
    if(_pCurrentView != NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderStatsMode = VIEWPORT_RENDERSTATS_ON;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportRenderStatsOff()
{
    if(_pCurrentView != NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._renderStatsMode = VIEWPORT_RENDERSTATS_OFF;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportActiveDesactiveShowIHM()
{
    if(_pCurrentView != NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._showIHM = !(listDesc[id]._showIHM);

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportLayoutSingle()
{
    _pCurrentView = NULL;
    _viewports.setActivateMode(SINGLE);
    emit currentViewportLayoutChanged();
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportLayoutQuadruple()
{
    _pCurrentView = NULL;
    _viewports.setActivateMode(QUADRUPLE);
    emit currentViewportLayoutChanged();
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportLayoutDoubleVertical()
{
    _pCurrentView = NULL;
    _viewports.setActivateMode(DOUBLE_VERTICAL);
    emit currentViewportLayoutChanged();
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraTypePerspective()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraType = VIEWPORT_CAMERA_PERSPECTIVE;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraTypeOrthogonal()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraType = VIEWPORT_CAMERA_ORTHOGONAL;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraNavigationOrbital()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraNavigation = VIEWPORT_CAMERA_ORBITAL;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraNavigationQuake()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraNavigation = VIEWPORT_CAMERA_QUAKE;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraViewTop()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = VIEWPORT_CAMERA_TOP;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraViewBottom()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = VIEWPORT_CAMERA_BOTTOM;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraViewBack()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = VIEWPORT_CAMERA_BACK;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraViewFront()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = VIEWPORT_CAMERA_FRONT;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraViewRight()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = VIEWPORT_CAMERA_RIGHT;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraViewLeft()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = VIEWPORT_CAMERA_LEFT;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraViewUser()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = VIEWPORT_CAMERA_USER;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraView(EViewportCameraView view)
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = view;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraViewEdition()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = VIEWPORT_CAMERA_EDITION;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setActiveCameraContent(int32 idPtyCam)
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._cameraView = VIEWPORT_CAMERA_CONTENT;
        listDesc[id]._idCameraContent = idPtyCam;
        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
const QList<QAction*>& GuiContext::getActionsViewportLayoutMode() const
{
    return _actionsViewportLayoutMode;
}
//-----------------------------------------------------------------------------
const QList<QAction*>& GuiContext::getActionsViewportRenderMode() const
{
    return _actionsViewportRenderMode;
}
//-----------------------------------------------------------------------------
const QList<QAction*>& GuiContext::getActionsViewportPolygonMode() const
{
    return _actionsViewportPolygonMode;
}
//-----------------------------------------------------------------------------
const QList<QAction*>& GuiContext::getActionsViewportCameraAspectRatio() const
{
    return _actionsViewportCameraAspectRatio;
}
//-----------------------------------------------------------------------------
const QList<QAction*>& GuiContext::getActionsViewportShadowMode() const
{
    return _actionsViewportShadowMode;
}
//-----------------------------------------------------------------------------
const QList<QAction*>& GuiContext::getActionsViewportRenderStatsMode() const
{
    return _actionsViewportRenderStatsMode;
}
//-----------------------------------------------------------------------------
const QList<QAction*>& GuiContext::getActionsViewportCameraType() const
{
    return _actionsViewportCameraType;
}
//-----------------------------------------------------------------------------
const QList<QAction*>& GuiContext::getActionsViewportCameraNavigation() const
{
    return _actionsViewportCameraNavigation;
}
//-----------------------------------------------------------------------------
const QList<QAction*>& GuiContext::getActionsViewportCameraView() const
{
    return _actionsViewportCameraView;
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewport(WorldViewportContainer* pView, Ptr<Universe::NodeListener> pNodeL)
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._isSelected = false;

        if (pNodeL!=null)
        {
            Ptr<Universe::NodeCamera> pNodeC = _pCurrentView->getView()->getNodeRenderCamera();
                pNodeC->removeChild(pNodeL);
                pNodeL->setLocalPosition(Core::Vector3f(Core::ZERO));
        }
        _pCurrentView->setDesc(listDesc[id]);
    }

    _pCurrentView = pView;
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._isSelected = true;

        _pCurrentView->setDesc(listDesc[id]);


        if (pNodeL!=null)
        {
            Ptr<Universe::NodeCamera> pNodeC = _pCurrentView->getView()->getNodeRenderCamera();
                pNodeC->addChild(pNodeL,false);
                pNodeL->setLocalPosition(Core::Vector3f(Core::ZERO));
        }
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraAspectRatioFree()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._aspectRatio = CAMERA_RATIO_FREE;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraAspectRatio_5_4()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._aspectRatio = CAMERA_RATIO_5_4;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraAspectRatio_4_3()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._aspectRatio = CAMERA_RATIO_4_3;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraAspectRatio_3_2()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._aspectRatio = CAMERA_RATIO_3_2;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraAspectRatio_16_10()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._aspectRatio = CAMERA_RATIO_16_10;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
void GuiContext::setCurrentViewportCameraAspectRatio_16_9()
{
    if (_pCurrentView!=NULL)
    {
        int32 id = _pCurrentView->getDesc()._id;
        
        Core::List<ViewportDesc>& listDesc = _viewports.getDescs(_viewports.getActivateMode());
        listDesc[id]._aspectRatio = CAMERA_RATIO_16_9;

        _pCurrentView->setDesc(listDesc[id]);
    }
}
//-----------------------------------------------------------------------------
WorldViewportContainer* GuiContext::getCurrentViewport() const
{
    return _pCurrentView;
}
//-----------------------------------------------------------------------------
const Ptr<CameraManager>& GuiContext::getCameraManager() const
{
    return _pCamMng; 
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

