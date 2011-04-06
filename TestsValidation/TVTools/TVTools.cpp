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
#include <Core/Logger.h>
#include "TVTools.h"
#include <winsafe.h>

namespace TVTools
{
//-----------------------------------------------------------------------------
void updateLighting(Renderer::MainLightSettings & settings, const Window::InputManager & input, float sensibility)
{
    int32 roll = input.getMouseWheelDelta();

    // Main light -------------------------------------------------------------

    if(input.isKeyDown(VK_LSHIFT) && input.isLMBDown())
    {
        int32 dx = input.getMouseXDelta();
        int32 dy = input.getMouseYDelta();

        settings.sunAngleToZenith = std::max(0.0f, std::min(f_PI, settings.sunAngleToZenith + dy * 0.01f));
        settings.sunAngleToNorth = std::max(0.0f, std::min(f_PI_MUL_2, settings.sunAngleToNorth + dx * 0.01f));

        INF << L"Zenith/North : " << Core::toString(settings.sunAngleToZenith) << L" / " << Core::toString(settings.sunAngleToNorth) << L"\n";
    }

    if(roll != 0)
    {
        if(input.isKeyDown(VK_LCONTROL) && input.isKeyDown(VK_LSHIFT))
        {
            float delta = float(roll) * sensibility;

            settings.specular.r = Core::clamp(0.0f, 1.0f, settings.specular.r + delta);
            settings.specular.g = Core::clamp(0.0f, 1.0f, settings.specular.g + delta);
            settings.specular.b = Core::clamp(0.0f, 1.0f, settings.specular.b + delta);

            INF << L"Specular     : " << Core::toString(settings.specular) << L"\n";
        }
        else if(input.isKeyDown(VK_LSHIFT))
        {
            float delta = float(roll) * sensibility;

            settings.ambient.r = Core::clamp(0.0f, 1.0f, settings.ambient.r + delta);
            settings.ambient.g = Core::clamp(0.0f, 1.0f, settings.ambient.g + delta);
            settings.ambient.b = Core::clamp(0.0f, 1.0f, settings.ambient.b + delta);

            INF << L"Ambient      : " << Core::toString(settings.ambient) << L"\n";
        }
        else if(input.isKeyDown(VK_LCONTROL))
        {
            float delta = float(roll) * sensibility;

            settings.diffuse0.r = Core::clamp(0.0f, 1.0f, settings.diffuse0.r + delta);
            settings.diffuse0.g = Core::clamp(0.0f, 1.0f, settings.diffuse0.g + delta);
            settings.diffuse0.b = Core::clamp(0.0f, 1.0f, settings.diffuse0.b + delta);

            INF << L"Diffuse      : " << Core::toString(settings.diffuse0) << L"\n";
        }
    }

    // Shadows ----------------------------------------------------------------

    if(roll != 0)
    {
        if(input.isKeyDown('U'))
        {
            static const float SENSIBILITY_SHADOW = 1.0f;
            float delta = float(roll) * SENSIBILITY_SHADOW;

            settings.dynamicShadowMiddle = Core::clamp(0.0f, 200.0f, settings.dynamicShadowMiddle + delta);

            INF << L"Shadow range  : " << Core::toString(settings.dynamicShadowMiddle)
                << L" (" << Core::toString(settings.dynamicShadowMiddleBlend) 
                << L"  ->  " << Core::toString(settings.dynamicShadowEnd) 
                << L" (" << Core::toString(settings.dynamicShadowEndBlend) << L")\n";
        }
        else if(input.isKeyDown('I'))
        {
            static const float SENSIBILITY_SHADOW = 1.0f;
            float delta = float(roll) * SENSIBILITY_SHADOW;

            settings.dynamicShadowMiddleBlend = Core::clamp(0.0f, 200.0f, settings.dynamicShadowMiddleBlend + delta);

            INF << L"Shadow range  : " << Core::toString(settings.dynamicShadowMiddle)
                << L" (" << Core::toString(settings.dynamicShadowMiddleBlend) 
                << L"  ->  " << Core::toString(settings.dynamicShadowEnd) 
                << L" (" << Core::toString(settings.dynamicShadowEndBlend) << L")\n";
        }
        else if(input.isKeyDown('O'))
        {
            static const float SENSIBILITY_SHADOW = 1.0f;
            float delta = float(roll) * SENSIBILITY_SHADOW;

            settings.dynamicShadowEnd = Core::clamp(0.0f, 200.0f, settings.dynamicShadowEnd + delta);

            INF << L"Shadow range  : " << Core::toString(settings.dynamicShadowMiddle)
                << L" (" << Core::toString(settings.dynamicShadowMiddleBlend) 
                << L"  ->  " << Core::toString(settings.dynamicShadowEnd) 
                << L" (" << Core::toString(settings.dynamicShadowEndBlend) << L")\n";
        }
        else if(input.isKeyDown('P'))
        {
            static const float SENSIBILITY_SHADOW = 1.0f;
            float delta = float(roll) * SENSIBILITY_SHADOW;

            settings.dynamicShadowEndBlend = Core::clamp(0.0f, 200.0f, settings.dynamicShadowEndBlend + delta);

            INF << L"Shadow range  : " << Core::toString(settings.dynamicShadowMiddle)
                << L" (" << Core::toString(settings.dynamicShadowMiddleBlend) 
                << L"  ->  " << Core::toString(settings.dynamicShadowEnd) 
                << L" (" << Core::toString(settings.dynamicShadowEndBlend) << L")\n";
        }
        else if(input.isKeyDown('Y'))
        {
            static const float SENSIBILITY_SHADOW_OPACITY = 0.1f;
            float delta = float(roll) * SENSIBILITY_SHADOW_OPACITY;

            settings.dynamicShadowOpacity = Core::clamp(0.0f, 1.0f, settings.dynamicShadowOpacity + delta);

            INF << L"Shadow opacity: " << Core::toString(settings.dynamicShadowOpacity) << L")\n";
        }
    }
}
//-----------------------------------------------------------------------------
void updateRenderSettings(Renderer::RendererSettings & settings, const Window::InputManager & input)
{
    if(input.isKeyDown(VK_LCONTROL))
    {
        if(input.isKeyTyped(VK_F1))
            settings.setGlobalLevel(Renderer::GLOBAL_VERY_LOW);
        if(input.isKeyTyped(VK_F2))
            settings.setGlobalLevel(Renderer::GLOBAL_LOW);
        if(input.isKeyTyped(VK_F3))
            settings.setGlobalLevel(Renderer::GLOBAL_MEDIUM);
        if(input.isKeyTyped(VK_F4))
            settings.setGlobalLevel(Renderer::GLOBAL_HIGH);
        if(input.isKeyTyped(VK_F5))
            settings.setGlobalLevel(Renderer::GLOBAL_VERY_HIGH);

        if(input.isKeyTyped(VK_F6))
        {
            settings.width = 800;
            settings.height = 600;
        }

        if(input.isKeyTyped(VK_F7))
        {
            settings.width = 1920;
            settings.height = 1080;
        }
    }

    if(input.isKeyDown(VK_LMENU) && input.isKeyTyped(VK_RETURN))
        settings.fullscreen = !settings.fullscreen;
}
//-----------------------------------------------------------------------------
void updateDebugSettings(Renderer::DebugSettings & settings, const Window::InputManager & input)
{
    if(input.isKeyTyped(VK_NUMPAD0))
        settings.renderMode = Renderer::RDR_HQ_PREVIEW;
    else if(input.isKeyTyped(VK_NUMPAD1))
        settings.polyMode = Renderer::POLY_SOLID;
    else if(input.isKeyTyped(VK_NUMPAD2))
        settings.polyMode = Renderer::POLY_SOLID_WIREFRAME;
    else if(input.isKeyTyped(VK_NUMPAD3))
        settings.polyMode = Renderer::POLY_WIREFRAME;
    else if(input.isKeyTyped(VK_NUMPAD4))
        settings.renderMode = Renderer::RDR_NORMAL;
    else if(input.isKeyTyped(VK_NUMPAD5))
        settings.renderMode = Renderer::RDR_DEBUG_UV;
    else if(input.isKeyTyped(VK_NUMPAD6))
        settings.renderMode = Renderer::RDR_DEBUG_NORMAL;
    else if(input.isKeyTyped(VK_NUMPAD7))
        settings.renderMode = Renderer::RDR_DEBUG_T;
    else if(input.isKeyTyped(VK_NUMPAD8))
        settings.renderMode = Renderer::RDR_DEBUG_B;
    else if(input.isKeyTyped(VK_NUMPAD9))
        settings.renderMode = Renderer::RDR_DEBUG_N;
    else if(input.isKeyTyped(VK_DECIMAL))
        settings.renderMode = Renderer::RDR_DEBUG_MIP;
        //settings.renderMode = Renderer::RDR_DEBUG_SHADOW_MAP;
}
//-----------------------------------------------------------------------------
void updateSky(Renderer::SkysphereSettings & sky, const Window::InputManager & input)
{
    if(input.isKeyTyped('1'))
    {
        sky.skyRoof = std::max(10.0f, std::min(10000.0f, sky.skyRoof + 10.0f));
        INF << L"SkyRoof : " << Core::toString(sky.skyRoof, 3) << L"\n";
    }
    else if(input.isKeyTyped('2'))
    {
        sky.skyRoof = std::max(10.0f, std::min(10000.0f, sky.skyRoof - 10.0f));
        INF << L"SkyRoof : " << Core::toString(sky.skyRoof, 3) << L"\n";
    }
    else if(input.isKeyTyped('3'))
    {
        sky.sphericity = std::max(0.0f, std::min(1.0f, sky.sphericity + 0.01f));
        INF << L"Sphericity : " << Core::toString(sky.sphericity, 3) << L"\n";
    }
    else if(input.isKeyTyped('4'))
    {
        sky.sphericity = std::max(0.0f, std::min(1.0f, sky.sphericity - 0.01f));
        INF << L"Sphericity : " << Core::toString(sky.sphericity, 3) << L"\n";
    }
    else if(input.isKeyTyped('5'))
    {
        sky.horizon = std::max(-1.0f, std::min(1.0f, sky.horizon + 0.01f));
        INF << L"Horizon : " << Core::toString(sky.horizon, 3) << L"\n";
    }
    else if(input.isKeyTyped('6'))
    {
        sky.horizon = std::max(-1.0f, std::min(1.0f, sky.horizon - 0.01f));
        INF << L"Horizon : " << Core::toString(sky.horizon, 3) << L"\n";
    }
    else if(input.isKeyTyped('7'))
    {
        sky.isAtInfinity = !sky.isAtInfinity;
        INF << L"AtInfinity : " << Core::toString(sky.isAtInfinity) << L"\n";
    }
}
//-----------------------------------------------------------------------------
}
