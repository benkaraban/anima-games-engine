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
#ifndef GUI_DESCRIPTION_H_
#define GUI_DESCRIPTION_H_

#include <Core/Standard.h>
#include <Core/List.h>

namespace EPI
{
//-----------------------------------------------------------------------------
LM_ENUM_3(EViewportLayoutMode,
          SINGLE,
          QUADRUPLE,
          DOUBLE_VERTICAL)

LM_ENUM_9(EViewportRenderMode, 
          VIEWPORT_RDR_NORMAL, 
          VIEWPORT_RDR_HQ_PREVIEW,
          VIEWPORT_RDR_DEBUG_SHADOW_MAP,
          VIEWPORT_RDR_DEBUG_UV,
          VIEWPORT_RDR_DEBUG_NORMAL,
          VIEWPORT_RDR_DEBUG_T,
          VIEWPORT_RDR_DEBUG_B,
          VIEWPORT_RDR_DEBUG_N,
          VIEWPORT_RDR_DEBUG_MIP);

LM_ENUM_3(EViewportPolygonMode,
          VIEWPORT_POLY_SOLID,
          VIEWPORT_POLY_SOLID_WIREFRAME,
          VIEWPORT_POLY_WIREFRAME);

LM_ENUM_2(EViewportShadowMode,
          VIEWPORT_SHADOW_ON,
          VIEWPORT_SHADOW_OFF);

LM_ENUM_2(EViewportRenderStatsMode,
          VIEWPORT_RENDERSTATS_ON,
          VIEWPORT_RENDERSTATS_OFF);

LM_ENUM_2(EViewportCameraType,
          VIEWPORT_CAMERA_PERSPECTIVE,
          VIEWPORT_CAMERA_ORTHOGONAL);

LM_ENUM_2(EViewportCameraNavigation,
          VIEWPORT_CAMERA_QUAKE,
          VIEWPORT_CAMERA_ORBITAL);

LM_ENUM_2(EViewportPickingType,
          VIEWPORT_PICKING_DEFAULT,
          VIEWPORT_PICKING_PTY)

LM_ENUM_9(EViewportCameraView,
          VIEWPORT_CAMERA_TOP,
          VIEWPORT_CAMERA_BOTTOM,
          VIEWPORT_CAMERA_BACK,
          VIEWPORT_CAMERA_FRONT,
          VIEWPORT_CAMERA_RIGHT,
          VIEWPORT_CAMERA_LEFT,
          VIEWPORT_CAMERA_USER,
          VIEWPORT_CAMERA_CONTENT,
          VIEWPORT_CAMERA_EDITION);

LM_ENUM_6   (ECameraAspectRatio,
             CAMERA_RATIO_FREE,
             CAMERA_RATIO_5_4,
             CAMERA_RATIO_4_3,
             CAMERA_RATIO_3_2,
             CAMERA_RATIO_16_10,
             CAMERA_RATIO_16_9)


inline LM_API_EPI String StringToECameraAspectRatio(ECameraAspectRatio ratio)
{
    String ret;

    switch(ratio)
    {
    case CAMERA_RATIO_FREE: ret  = L"Free Aspect";
    case CAMERA_RATIO_5_4: ret   = L"5:4";
    case CAMERA_RATIO_4_3: ret   = L"4:3";
    case CAMERA_RATIO_3_2: ret   = L"3:2";
    case CAMERA_RATIO_16_10: ret = L"16:10";
    case CAMERA_RATIO_16_9: ret  = L"16:9";
    }

    return ret;
}

LM_API_EPI int32 getNbViewports(EViewportLayoutMode mode);
//-----------------------------------------------------------------------------
class LM_API_EPI GuiDescription : public Core::Object
{
public:
    GuiDescription();
    virtual ~GuiDescription();

    const Core::List<EViewportLayoutMode>& getViewportLayoutModes() const {return _viewLayoutModes;}
    void setViewportLayoutModes(const Core::List<EViewportLayoutMode>& viewLayoutModes) {_viewLayoutModes = viewLayoutModes;}

    const Core::List<EViewportCameraType>& getViewportCameraTypes() const {return _viewCameraTypes;}
    void setViewportCameraTypes(const Core::List<EViewportCameraType>& viewCameraTypes) {_viewCameraTypes = viewCameraTypes;}

    const Core::List<EViewportCameraNavigation>& getViewportCameraNavigations() const {return _viewCameraNavigations;}
    void setViewportCameraNavigations(const Core::List<EViewportCameraNavigation>& viewCameraNavigations) {_viewCameraNavigations = viewCameraNavigations;}

    const Core::List<EViewportCameraView>& getViewportCameraViews() const {return _viewCameraViews;}
    void setViewportCameraViews(const Core::List<EViewportCameraView>& viewCameraViews) {_viewCameraViews = viewCameraViews;}

    const Core::List<EViewportRenderMode>& getViewportRenderModes() const {return _viewRenderModes;}
    void setViewportRenderModes(const Core::List<EViewportRenderMode>& viewRenderModes) {_viewRenderModes = viewRenderModes;}

    const Core::List<EViewportPolygonMode>& getViewportPolygonModes() const {return _viewPolygonModes;}
    void setViewportPolygonModes(const Core::List<EViewportPolygonMode>& viewPolygonModes) {_viewPolygonModes = viewPolygonModes;}

    const Core::List<EViewportShadowMode>& getViewportShadowModes() const {return _viewShadowModes;}
    void setViewportShadowModes(const Core::List<EViewportShadowMode>& viewShadowModes) {_viewShadowModes = viewShadowModes;}

    const Core::List<EViewportPickingType>& getViewportPickingTypes() const {return _viewPickingTypes;}
    void setViewportPickingTypes(const Core::List<EViewportPickingType>& viewPickingTypes) {_viewPickingTypes = viewPickingTypes;}

    const Core::List<EViewportRenderStatsMode>& getViewportRenderStatsModes() const { return _viewRenderStatsModes; }
    void setViewportRenderStatsModes(const Core::List<EViewportRenderStatsMode>& modes) {_viewRenderStatsModes = modes;}

private:
    Core::List<EViewportLayoutMode>         _viewLayoutModes;
    Core::List<EViewportCameraType>         _viewCameraTypes;
    Core::List<EViewportCameraNavigation>   _viewCameraNavigations;
    Core::List<EViewportPickingType>        _viewPickingTypes;
    Core::List<EViewportCameraView>         _viewCameraViews;
    Core::List<EViewportRenderMode>         _viewRenderModes;
    Core::List<EViewportPolygonMode>        _viewPolygonModes;
    Core::List<EViewportShadowMode>         _viewShadowModes;
    Core::List<EViewportRenderStatsMode>    _viewRenderStatsModes;
};
//-----------------------------------------------------------------------------
} //namespace EPI

#endif // GUI_DESCRIPTION_H_
