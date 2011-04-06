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
#include <UTests.h>

#include <Assets/GeometricPrimitive.h>

#include <Core/VFS/VFSDirectMapping.h>
#include <Core/Logger.h>

#include <Gfx/GfxDx9/DriverDx9.h>

#include <Renderer/SM2/RendererSM2.h>

#include <Window/Window.h>
#include <Window/WinHandler.h>
#include <Window/InputManager.h>

#include <Universe/World.h>

void testIntersection(Universe::World & world)
{
    UTEST_START_TEST(L"NodeMeshIntersection");

    String modelName = L"modelIntersect";
    String meshName = L"meshIntersect";

    Ptr<Assets::ModelMesh> meshIntersect (new Assets::ModelMesh());
    Assets::VerticeSet & vset = meshIntersect->getLODByID(0);
    Assets::createGeoSphere(vset, 1, 3, true);
    meshIntersect->cleanup();

    Ptr<Assets::Model> pIntersectModel (new Assets::Model());
    pIntersectModel->addMesh(meshName, *meshIntersect);
    world.getRessourcesPool()->addModel(modelName, pIntersectModel);
    
    Ptr<Universe::NodeMesh> pNodeMeshIntersect (world.createMesh(modelName, meshName));
    
    float distance;

    Core::Rayf ray_1 (Core::Ray_OriginDirection, Core::Vector3f(0,5,0), Core::Vector3f(0,-1,0));
    bool isIntersect_1 = pNodeMeshIntersect->isIntersecting(ray_1, distance);

    Core::Rayf ray_2 (Core::Ray_OriginDirection, Core::Vector3f(10,5,0), Core::Vector3f(0,-1,0));
    pNodeMeshIntersect->setWorldPosition(Core::Vector3f(10,0,0));
    bool isIntersect_2 = pNodeMeshIntersect->isIntersecting(ray_2, distance);

    pNodeMeshIntersect->kill();
    UTEST_END_TEST;
}

void testWithRenderer()
{
    Ptr<Core::VFSDirectMapping> pVFS(new Core::VFSDirectMapping());
    if(!pVFS->linkDirectory(L"."))
    {
        ERR << L"Unable to initialise VFS\n";
        return;
    }

    Core::LoggerManager::getInstance().getDefaultLogger()->setLogMsgLevel(Core::L_WARNING);

    Ptr<Window::InputManager> pInput(new Window::InputManager());
    Ptr<Window::WinHandler> pWinHandler(new Window::WinHandler(pInput));
    Ptr<Window::Window> pRenderWindow(new Window::Window(
            NULL,
            L"TVRenderer",
            0, 0,    // position
            1, 1,    // dimension
            false,   // fullscreen
            pWinHandler));

    pInput->initialise(pRenderWindow->getHandle());
    Ptr<Gfx::IDriver> pDriver(new Gfx::DriverDx9());

    Renderer::RendererSettings rs(Renderer::GLOBAL_VERY_HIGH);
    rs.fullscreen = false;
    rs.width = pRenderWindow->getWidth();
    rs.height = pRenderWindow->getHeight();

    Ptr<Renderer::IRenderer> pRenderer(new Renderer::RendererSM2(
            pDriver,
            rs,
            pRenderWindow->getHandle(),
            pVFS,
            L"/Shaders",
            0));

    bool result = pRenderer->initialise();
    LM_ASSERT(result);


    {
        Universe::World world(pRenderer, pVFS);
        Core::LoggerManager::getInstance().getDefaultLogger()->setLogMsgLevel(Core::L_DEBUG);

        testIntersection(world);
        Core::LoggerManager::getInstance().getDefaultLogger()->setLogMsgLevel(Core::L_WARNING);
    }

    pRenderer = null;
    pDriver = null;
    pRenderWindow = null;
    pWinHandler = null;
    pInput = null;
    pVFS = null;

    Core::LoggerManager::getInstance().getDefaultLogger()->setLogMsgLevel(Core::L_DEBUG);
}
