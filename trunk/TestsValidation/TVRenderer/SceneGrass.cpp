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
#include <Core/Math/Random.h>
#include <Core/Math/MathTools.h>
#include "SceneGrass.h"
#include <Assets/Tool.h>
#include <Assets/GeometricPrimitive.h>
#include <Workflow/ModelImporterX.h>
#include <Workflow/TexCompression.h>

const int32 GRASS_COUNT      = 1000;
const float GRASS_DIMENSION  = 5000.0;

const float MESH_DIMENSION   = 5000.0;

using Core::frand;

SceneGrass::SceneGrass(const TestSceneData & data) :
    ITestScene(data),
    _worldMatTeapot(Core::IDENTITY),
    _angle(0.0)
{
    _quakeCamSpeed = 1000.0;
    setCamera(CAMERA_QUAKE);
}

bool SceneGrass::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        _pScene = _pRenderer->createScene();

        // Fog
        Renderer::FogSettings fog(Core::Vector4f(0.0, 0.0, 0.0, 0.0), 100.0, 2000.0, 0.0);
        _pScene->setFogSettings(fog);

        // Modèle
        {
            Ptr<Renderer::ITextureMap> pGrassTexture(loadTexture(pVFS, L"/Data/TUData/Textures/grass-ground00.tga", Workflow::TEXTURE_COLORMAP));

            Ptr<Assets::ModelMesh> pMesh(new Assets::ModelMesh(0, L"Terrain"));
            Ptr<Assets::VerticeSet> pVert(pMesh->getLOD(Assets::LOD_HIGH));

            Assets::createPlane(*pVert, MESH_DIMENSION, MESH_DIMENSION, 181, 181, 20.0, 20.0);
            pMesh->cleanup();

            TextureGrabber texGrabber(pVFS, _pRenderer);
            Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(pMesh, texGrabber);

            // Instances

            Ptr<Renderer::IMeshInstance> pTerrain = _pScene->createMeshInstance(pRenderMesh);
            pTerrain->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.0f, -100.0f, 0.0f));
            {
                Renderer::Material mat;
                mat.diffuse  = Core::Vector4f(1.00, 1.00, 1.00, 1.00);
                mat.specular = Core::Vector4f(0.00, 0.00, 0.00, 0.00);
                mat.pDiffuseMap  = pGrassTexture;

                pTerrain->setMaterial(mat);

                Ptr<Renderer::IAABox> pBox = _pScene->createAABox();
                pBox->setBox(pTerrain->getBoundingBox());
                _boxes.push_back(pBox);
            }
        }

        // SkySphere
        {
            Ptr<Renderer::ITextureMap> pSkyTexture(loadTexture(pVFS, L"/Data/TUData/Textures/FullskiesSunset0024_1_S.jpg", Workflow::TEXTURE_COLORMAP));
            _pScene->setSkysphereSettings(Renderer::SkysphereSettings(pSkyTexture, -0.25f));
        }

        // Grass
        {

            Ptr<Renderer::ITextureMap> pGrassTexture(loadTexture(pVFS, L"/Data/TUData/Textures/grass02.tga", Workflow::TEXTURE_COLORMAP));
            Core::List<Renderer::GrassPatch> patches;
#if 0
            {
                Renderer::GrassPatch patch;
                patch.centerPos = Core::Vector3f(100.0, 0.0, -600.0);
                patch.radius = 100.0;
                patch.height = 100.0;
                patches.push_back(patch);
                patch.teta = 1.5;
            }
#else

            for(int32 ii=0; ii < GRASS_COUNT; ii++)
            {
                Renderer::GrassPatch patch;
                patch.centerPos = Core::Vector3f((frand()-0.5f)*GRASS_DIMENSION, (frand()-0.5f)*20.0f, (frand()-0.5f)*GRASS_DIMENSION);
                patch.radius = 200.0;
                patch.height = 200.0;
                patch.teta = f_PI * frand();

                patches.push_back(patch);
            }
#endif
            Ptr<Renderer::IGrassCluster> pGrass(_pScene->createGrassCluster(patches, pGrassTexture));
            pGrass->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.0, -100.0, 0.0));

            Renderer::WindSettings wind0;
            wind0.direction  = Core::Vector3f(1.0f, 0.0f, 0.0f).getNormalizedSafe();
            wind0.waveLength = 2000.0f;
            wind0.period     = 3.0f;
            wind0.phase      = 1.5f;
            wind0.intensity  = 3.0f;

            Renderer::WindSettings wind1;
            wind1.direction  = Core::Vector3f(1.0f, 0.0f, 1.0f).getNormalizedSafe();
            wind1.waveLength = 1300.0;
            wind1.period     = 3.5f;
            wind1.phase      = 0.9f;
            wind1.intensity  = 2.0f;

            Renderer::WindSettings wind3;
            wind1.direction  = Core::Vector3f(-2.0f, 0.0f, 1.0f).getNormalizedSafe();
            wind1.waveLength = 700.0f;
            wind1.period     = 2.5f;
            wind1.phase      = 0.0f;
            wind1.intensity  = 1.0f;

            Renderer::WindSettings wind4;
            wind1.direction  = Core::Vector3f(1.0f, 0.0f, -1.0f).getNormalizedSafe();
            wind1.waveLength = 400.0f;
            wind1.period     = 2.1f;
            wind1.phase      = 0.5f;
            wind1.intensity  = 0.5f;

            _pScene->setWindSettings(0, wind0);
            _pScene->setWindSettings(1, wind1);
            _pScene->setWindSettings(2, wind3);
            _pScene->setWindSettings(3, wind4);

            Ptr<Renderer::IAABox> pBox = _pScene->createAABox();
            pBox->setBox(pGrass->getBoundingBox());
            _boxes.push_back(pBox);
        }

        setBoxesVisible(false);

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 10.0, 10000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising grass scene : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}

void SceneGrass::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}


