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
#include "GuiDescription.h"

namespace EPI
{
//-----------------------------------------------------------------------------
LM_API_EPI int32 getNbViewports(EViewportLayoutMode mode)
{
    int32 ret;

    switch(mode)
    {
        case SINGLE: ret = 1; break;
        case QUADRUPLE: ret = 4; break;
        case DOUBLE_VERTICAL: ret = 2; break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
GuiDescription::GuiDescription()
{
    //EViewportLayoutMode
    Core::List<EViewportLayoutMode> viewLayoutModes;
        viewLayoutModes.push_back(SINGLE);
        viewLayoutModes.push_back(DOUBLE_VERTICAL);
        viewLayoutModes.push_back(QUADRUPLE);
    setViewportLayoutModes(viewLayoutModes);

    //EViewportCameraType
    Core::List<EViewportCameraType> viewCameraTypes;
        viewCameraTypes.push_back(VIEWPORT_CAMERA_PERSPECTIVE);
        viewCameraTypes.push_back(VIEWPORT_CAMERA_ORTHOGONAL);
    setViewportCameraTypes(viewCameraTypes);

    //EViewportCameraNavigation
    Core::List<EViewportCameraNavigation> viewCameraNavigations;
        viewCameraNavigations.push_back(VIEWPORT_CAMERA_QUAKE);
        viewCameraNavigations.push_back(VIEWPORT_CAMERA_ORBITAL);
    setViewportCameraNavigations(viewCameraNavigations);

    //EViewportCameraView
    Core::List<EViewportCameraView> viewCameraViews;
        viewCameraViews.push_back(VIEWPORT_CAMERA_USER);
        viewCameraViews.push_back(VIEWPORT_CAMERA_TOP);
        viewCameraViews.push_back(VIEWPORT_CAMERA_BOTTOM);
        viewCameraViews.push_back(VIEWPORT_CAMERA_BACK);
        viewCameraViews.push_back(VIEWPORT_CAMERA_FRONT);
        viewCameraViews.push_back(VIEWPORT_CAMERA_RIGHT);
        viewCameraViews.push_back(VIEWPORT_CAMERA_LEFT);
    setViewportCameraViews(viewCameraViews);

    //EViewportRenderMode
    Core::List<EViewportRenderMode> viewRenderModes;
        viewRenderModes.push_back(VIEWPORT_RDR_NORMAL);
        viewRenderModes.push_back(VIEWPORT_RDR_HQ_PREVIEW);
        viewRenderModes.push_back(VIEWPORT_RDR_DEBUG_SHADOW_MAP);
        viewRenderModes.push_back(VIEWPORT_RDR_DEBUG_UV);
        viewRenderModes.push_back(VIEWPORT_RDR_DEBUG_NORMAL);
        viewRenderModes.push_back(VIEWPORT_RDR_DEBUG_T);
        viewRenderModes.push_back(VIEWPORT_RDR_DEBUG_B);
        viewRenderModes.push_back(VIEWPORT_RDR_DEBUG_N);
        viewRenderModes.push_back(VIEWPORT_RDR_DEBUG_MIP);
    setViewportRenderModes(viewRenderModes);

    //EViewportPolygonMode
    Core::List<EViewportPolygonMode> viewPolygonModes;
        viewPolygonModes.push_back(VIEWPORT_POLY_SOLID);
        viewPolygonModes.push_back(VIEWPORT_POLY_SOLID_WIREFRAME);
        viewPolygonModes.push_back(VIEWPORT_POLY_WIREFRAME);
    setViewportPolygonModes(viewPolygonModes);

    //EViewportShadowMode
    Core::List<EViewportShadowMode> viewportShadowModes;
        viewportShadowModes.push_back(VIEWPORT_SHADOW_ON);
        viewportShadowModes.push_back(VIEWPORT_SHADOW_OFF);
    setViewportShadowModes(viewportShadowModes);

    //EViewportRenderStatsMode
    Core::List<EViewportRenderStatsMode> viewportRenderStatsMode;
        viewportRenderStatsMode.push_back(VIEWPORT_RENDERSTATS_OFF);
        viewportRenderStatsMode.push_back(VIEWPORT_RENDERSTATS_ON);
    setViewportRenderStatsModes(viewportRenderStatsMode);

    //EViewportPicking
    Core::List<EViewportPickingType> viewportPickingTypes;
        viewportPickingTypes.push_back(VIEWPORT_PICKING_DEFAULT);
    setViewportPickingTypes(viewportPickingTypes);
}
//-----------------------------------------------------------------------------
GuiDescription::~GuiDescription()
{
}
//-----------------------------------------------------------------------------
} //namespace EPI
