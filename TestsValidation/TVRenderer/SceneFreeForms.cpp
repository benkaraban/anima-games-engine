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
#include <Core/Math/Interpolate.h>
#include <Core/Math/Random.h>
#include <Assets/GeometricPrimitive.h>
#include <Workflow/COLLADA/ModelImporterCOLLADA.h>
#include <Workflow/ModelOptimize.h>
#include <Workflow/TexCompression.h>
#include "SceneFreeForms.h"


SceneFreeForms::SceneFreeForms(const TestSceneData & data) :
    ITestScene(data)
{
    updateCamera();
}

bool SceneFreeForms::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        _pScene = _pRenderer->createScene();

        // Fog
        Renderer::FogSettings fog(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 300.0f, 1500.0f, 0.0f);
        _pScene->setFogSettings(fog);

        // Main light

        Renderer::MainLightSettings settings;
        settings.ambient  = Core::Vector4f(0.00f, 0.00f, 0.00f, 0.0f);
        settings.diffuse  = Core::Vector4f(0.65f, 0.65f, 0.65f, 0.0f);
        settings.specular = Core::Vector4f(0.80f, 0.80f, 0.80f, 0.0f);
        settings.bloomFactor = 1.0f;
        settings.bloomThreshold = 1.0f;
        settings.sunAngleToZenith = 1.0f;
        settings.sunAngleToNorth = 0.5f;

        _pScene->setLightSettings(settings);

        // Modèle

        Ptr<Assets::VerticeSet> pCube(Ptr<Assets::VerticeSet>(new Assets::VerticeSet()));
        Assets::createCubeWithNormals(*pCube, Core::Vector3f(1.0f, 1.0f, 1.0f));

        // Instances

        Ptr<Renderer::IFreeForm> pFreeForm = _pScene->createFreeForm(null, Renderer::FREE_FORM_LIGHT_MESH);

        _pGroup = Ptr<Renderer::InstancesGroup>(new Renderer::InstancesGroup(pFreeForm, pCube));

        _cubes.resize(2000);

        for(int32 ii=0; ii < _cubes.size(); ii++)
        {
            _cubes[ii].alpha = Core::frand(0.0f, f_PI_MUL_2);
            _cubes[ii].beta = Core::frand(0.0f, f_PI);
            _cubes[ii].dist = Core::frand(300.0f, 900.0f);

            _cubes[ii].teta = Core::frand(0.0f, f_PI_MUL_2);
            _cubes[ii].zeta = Core::frand(0.0f, f_PI_MUL_2);

            _cubes[ii].dteta = Core::frand(0.0f, 0.2f * f_PI_MUL_2);
            _cubes[ii].dzeta = Core::frand(0.0f, 0.2f * f_PI_MUL_2);
        }

        Core::List<Renderer::Instance> & instances = _pGroup->beginUpdate();

        instances.resize(_cubes.size());

        for(int32 ii=0; ii < _cubes.size(); ii++)
        {
            //_cubes[ii].teta = fmod(_cubes[ii].teta + float(elapsed * _cubes[ii].dteta), f_PI_MUL_2);
            //_cubes[ii].zeta = fmod(_cubes[ii].zeta + float(elapsed * _cubes[ii].dzeta), f_PI_MUL_2);

            float r = _cubes[ii].dist;
            float cost, sint, cosz, sinz;

            Core::L_sincos(_cubes[ii].alpha, &sint, &cost);
            Core::L_sincos(_cubes[ii].beta, &sinz, &cosz);

            Core::Vector3f pos((cost * r) * sinz,
                               (sint * r) * sinz,
                                r * cosz);

            //Core::Matrix4f matS(Core::SCALE, Core::Vector3f(10.0f));
            Core::Matrix4f matT(Core::TRANSLATION, pos);
            //Core::Matrix4f matR(Core::ROTATION, _cubes[ii].teta, Core::ROT_X_PITCH);
            //Core::Matrix4f matY(Core::ROTATION, _cubes[ii].zeta, Core::ROT_Y_YAW);

            //instances[ii].worldMat = matT * (matR * (matY * matS));
            instances[ii].worldMat = matT;
            instances[ii].color = Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
            instances[ii].glow = Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
        }
    
        _pGroup->endUpdate();

        _pCamera = _pScene->createPerspectiveCamera(f_PI_DIV_4, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 10.0, 10000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising free forms scene : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}

void SceneFreeForms::update(double elapsed)
{
    ITestScene::update(elapsed);

    Core::List<Renderer::Instance> & instances = _pGroup->beginUpdate();

    instances.resize(_cubes.size());

    int32 ii = 0;

//#pragma omp parallel for default(none) shared(elapsed, instances) private(ii)
    for(ii=0; ii < _cubes.size(); ii++)
    {
        _cubes[ii].teta = fmod(_cubes[ii].teta + float(elapsed * _cubes[ii].dteta), f_PI_MUL_2);
        _cubes[ii].zeta = fmod(_cubes[ii].zeta + float(elapsed * _cubes[ii].dzeta), f_PI_MUL_2);

        float r = _cubes[ii].dist;
        float cost, sint, cosz, sinz;

        Core::L_sincos(_cubes[ii].alpha, &sint, &cost);
        Core::L_sincos(_cubes[ii].beta, &sinz, &cosz);

        Core::Vector3f pos((cost * r) * sinz,
                           (sint * r) * sinz,
                            r * cosz);

        Core::Matrix4f matS(Core::SCALE, Core::Vector3f(10.0f));
        Core::Matrix4f matT(Core::TRANSLATION, pos);
        Core::Matrix4f matR(Core::ROTATION, _cubes[ii].teta, Core::ROT_X_PITCH);
        Core::Matrix4f matY(Core::ROTATION, _cubes[ii].zeta, Core::ROT_Y_YAW);
        //Core::Matrix4f matR(Core::IDENTITY);
        //Core::Matrix4f matY(Core::IDENTITY);

        instances[ii].worldMat = matT * (matR * (matY * matS));
        instances[ii].color = Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
        instances[ii].glow = Core::Vector4f(0.0f, 1.0f, 0.0f, 0.0f);
    }

    _pGroup->endUpdate();
}

void SceneFreeForms::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}



