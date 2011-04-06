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
#include <Workflow/ModelImporterX.h>
#include <Workflow/ModelOptimize.h>
#include <Workflow/TexCompression.h>
#include "SceneBillboards.h"

SceneBillboards::SceneBillboards(const TestSceneData & data) :
    ITestScene(data)
{
    updateCamera();
}

bool SceneBillboards::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        _pScene = _pRenderer->createScene();

        // Fog
        Renderer::FogSettings fog(Core::Vector4f(0.5, 0.5, 0.5, 1.0), 300.0, 800.0, 0.0);
        _pScene->setFogSettings(fog);

        // Modèle

        Workflow::ModelImporterX importX;
        Ptr<Core::InputStream> pInputModel(pVFS->readFile(L"/Data/TUData/Modeles/teapot.x"));
        Ptr<Assets::Model> pModelTeapot(importX.importModel(*pInputModel));

        Workflow::ModelPackingOptions modelOptions;

        Workflow::packModel(modelOptions, *pModelTeapot);

        int32 idTeapotMesh = pModelTeapot->getMeshId(L"Teapot01");

        if(idTeapotMesh == Assets::UNDEFINED_NODE_ID)
            throw Core::Exception(L"Model does not contain objet 'Teapot01'");

        TextureGrabber texGrabber(pVFS, _pRenderer);
        Ptr<Renderer::IMesh> pRenderMesh = _pRenderer->uploadMesh(pModelTeapot->getMeshPtr(idTeapotMesh), texGrabber);

        Renderer::Material mat1;
        mat1.diffuse = Core::Vector4f(0.42f, 0.64f, 0.87f, 1.00f);
        mat1.specular = Core::Vector4f(0.40f, 0.40f, 0.40f, 1.00f);
        mat1.shininess = 45.0f;

        Ptr<Renderer::IMeshInstance> pTeapot(_pScene->createMeshInstance(pRenderMesh));
        pTeapot->setMaterial(mat1);
        pTeapot->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.0f, -30.0f, 0.0f));

        // Texture

        Ptr<Renderer::ITextureMap> pTexMap00(loadTexture(pVFS, L"/Data/TUData/Textures/decal00.tga", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pTexMap01(loadTexture(pVFS, L"/Data/TUData/Textures/decal01.tga", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pTexMap02(loadTexture(pVFS, L"/Data/TUData/Textures/decal02.tga", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pTexMap03(loadTexture(pVFS, L"/Data/TUData/Textures/water2.tga", Workflow::TEXTURE_NORMALMAP));

        // Texture animée 0

        Workflow::TextureOptions optionsColor;
        Core::List<Ptr<Images::Image> > images;
        images.push_back(Ptr<Images::Image>(new Images::Image()));
        images.push_back(Ptr<Images::Image>(new Images::Image()));
        images.push_back(Ptr<Images::Image>(new Images::Image()));
        images.push_back(Ptr<Images::Image>(new Images::Image()));

        Ptr<Core::InputStream> pInputAnim00(pVFS->readFile(L"/Data/TUData/Textures/Anim00/00.jpg"));
        Ptr<Core::InputStream> pInputAnim01(pVFS->readFile(L"/Data/TUData/Textures/Anim00/01.jpg"));
        Ptr<Core::InputStream> pInputAnim02(pVFS->readFile(L"/Data/TUData/Textures/Anim00/02.jpg"));
        Ptr<Core::InputStream> pInputAnim03(pVFS->readFile(L"/Data/TUData/Textures/Anim00/03.jpg"));

        images[0]->load((Core::SeekableInputStream &)*pInputAnim00);
        images[1]->load((Core::SeekableInputStream &)*pInputAnim01);
        images[2]->load((Core::SeekableInputStream &)*pInputAnim02);
        images[3]->load((Core::SeekableInputStream &)*pInputAnim03);

        Ptr<Assets::Texture> pTexAnim00(Workflow::generateTexture3D(optionsColor, images));
        Ptr<Renderer::ITextureMap> pTexMapAnim00(_pRenderer->uploadTexture(pTexAnim00));

        // Texture animée 1 (sprite)

        Ptr<Renderer::ITextureMap> pTexMapAnim01(loadTexture(pVFS, L"/Data/TUData/Textures/fire-sprite2.jpg", Workflow::TEXTURE_COLORMAP));

        // Instances

        _pBill1 = _pScene->createBillboardGroup(20, pTexMap01, Renderer::BILLBOARD_LERP);
        _pBill2 = _pScene->createBillboardGroup(20, pTexMap02, Renderer::BILLBOARD_ADD);
        _pBill3 = _pScene->createBillboardGroup(20, pTexMap03, Renderer::BILLBOARD_REFRAC);
        _pBill4 = _pScene->createBillboardGroup(20, pTexMapAnim01, Renderer::BILLBOARD_ADD);

        {
            Core::List<Renderer::Billboard> & bills1 = _pBill1->beginUpdate();

            bills1.resize(6);

            bills1[0].position = Core::Vector3f(-50.0f, 20.0f, -50.0f);
            bills1[0].color = Core::Vector4f(1.0f, 0.5f, 0.5f, 1.0f);
            bills1[0].width = 40.0f;
            bills1[0].height = 40.0f;
            bills1[0].angle = 0.0f;

            bills1[1].position = Core::Vector3f(0.0f, 20.0f, -50.0f);
            bills1[1].color = Core::Vector4f(0.5f, 1.0f, 0.5f, 1.0f);
            bills1[1].width = 50.0f;
            bills1[1].height = 40.0f;
            bills1[1].angle = 0.0f;

            bills1[2].position = Core::Vector3f(50.0f, 20.0f, -50.0);
            bills1[2].color = Core::Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
            bills1[2].width = 40.0f;
            bills1[2].height = 50.0f;
            bills1[2].angle = 1.0f;

            bills1[3].position = Core::Vector3f(-50.0f, -20.0f, -50.0f);
            bills1[3].color = Core::Vector4f(1.0f, 0.5f, 0.5f, 0.5f);
            bills1[3].width = 40.0f;
            bills1[3].height = 40.0f;
            bills1[3].angle = 1.5f;

            bills1[4].position = Core::Vector3f(0.0f, -20.0f, -50.0f);
            bills1[4].color = Core::Vector4f(0.5f, 1.0f, 0.5f, 0.6f);
            bills1[4].width = 50.0f;
            bills1[4].height = 40.0f;
            bills1[4].angle = 2.0f;

            bills1[5].position = Core::Vector3f(50.0f, -20.0f, -50.0);
            bills1[5].color = Core::Vector4f(0.5f, 0.5f, 1.0f, 0.8f);
            bills1[5].width = 40.0f;
            bills1[5].height = 50.0f;
            bills1[5].angle = 2.5f;

            _pBill1->endUpdate();
        }

        {
            Core::List<Renderer::Billboard> & bills1 = _pBill2->beginUpdate();

            bills1.resize(6);

            bills1[0].position = Core::Vector3f(-100.0f, 20.0f, -50.0f);
            bills1[0].color = Core::Vector4f(1.0f, 0.5f, 0.5f, 1.0f);
            bills1[0].width = 40.0f;
            bills1[0].height = 40.0f;
            bills1[0].angle = 0.0f;

            bills1[1].position = Core::Vector3f(-50.0f, 20.0f, -50.0f);
            bills1[1].color = Core::Vector4f(0.5f, 1.0f, 0.5f, 1.0f);
            bills1[1].width = 50.0f;
            bills1[1].height = 40.0f;
            bills1[1].angle = 0.0f;

            bills1[2].position = Core::Vector3f(0.0f, 20.0f, -50.0f);
            bills1[2].color = Core::Vector4f(0.5f, 0.5f, 1.0f, 1.0f);
            bills1[2].width = 40.0f;
            bills1[2].height = 50.0f;
            bills1[2].angle = 0.0f;

            bills1[3].position = Core::Vector3f(-100.0f, -20.0f, -50.0f);
            bills1[3].color = Core::Vector4f(1.0f, 0.5f, 0.5f, 0.5f);
            bills1[3].width = 40.0f;
            bills1[3].height = 40.0f;
            bills1[3].angle = 0.0f;

            bills1[4].position = Core::Vector3f(-50.0f, -20.0f, -50.0f);
            bills1[4].color = Core::Vector4f(0.5f, 1.0f, 0.5f, 0.6f);
            bills1[4].width = 50.0f;
            bills1[4].height = 40.0f;
            bills1[4].angle = 0.0f;

            bills1[5].position = Core::Vector3f(0.0f, -20.0f, -50.0f);
            bills1[5].color = Core::Vector4f(0.5f, 0.5f, 1.0f, 0.8f);
            bills1[5].width = 40.0f;
            bills1[5].height = 50.0f;
            bills1[5].angle = 0.0f;

            _pBill2->endUpdate();
        }

        {
            Core::List<Renderer::Billboard> & bills = _pBill3->beginUpdate();

            bills.resize(2);

            bills[0].position = Core::Vector3f(-40.0, 80.0, 0.0);
            bills[0].color = Core::Vector4f(1.0, 0.5, 0.5, 1.0);
            bills[0].width = 40.0;
            bills[0].height = 40.0;
            bills[0].angle = 0.0;

            bills[1].position = Core::Vector3f(40.0, 80.0, 0.0);
            bills[1].color = Core::Vector4f(0.5, 1.0, 0.5, 0.25);
            bills[1].width = 40.0;
            bills[1].height = 40.0;
            bills[1].angle = 0.0;

            _pBill3->endUpdate();
        }

        {
            Core::List<Renderer::Billboard> & bills = _pBill4->beginUpdate();

            bills.resize(2);

            bills[0].position = Core::Vector3f(-40.0, -80.0, 0.0);
            bills[0].color = Core::Vector4f(1.0, 1.0, 1.0, 1.0);
            bills[0].width = 40.0;
            bills[0].height = 40.0;
            bills[0].angle = 0.0;
            bills[0].time = 0.0f;

            bills[1].position = Core::Vector3f(40.0, -80.0, 0.0);
            bills[1].color = Core::Vector4f(1.0, 1.0, 1.0, 1.0);
            bills[1].width = 40.0;
            bills[1].height = 40.0;
            bills[1].angle = 0.0;
            bills[1].time = 0.5f;

            _pBill4->endUpdate();
            _pBill4->setSpriteLayout(4, 4);
            _pBill4->setVisible(true);
        }

        Core::Matrix4f rotY(Core::ROTATION, f_PI_DIV_2, Core::ROT_Y_YAW);
        _pBill2->setWorldMatrix(rotY);

        _boxes.push_back(_pScene->createAABox());
        _boxes.push_back(_pScene->createAABox());
        _boxes.push_back(_pScene->createAABox());

        _boxes[0]->setBox(pTeapot->getBoundingBox());
        _boxes[1]->setBox(_pBill1->getBoundingBox());
        _boxes[2]->setBox(_pBill2->getBoundingBox());

        setBoxesVisible(false);

        _pCamera = _pScene->createPerspectiveCamera(f_PI * 0.25f, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 10.0, 10000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising teapot scene : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}

void SceneBillboards::update(double elapsed)
{
    ITestScene::update(elapsed);

    if(_pInput->isKeyDown('L'))
    {
        float c = _pBill3->getRefractionScale();
        c -= 1.0f * float(elapsed);
        INF << Core::toString(c) << L"\n";
        _pBill3->setRefractionScale(c);
    }
    if(_pInput->isKeyDown('M'))
    {
        float c = _pBill3->getRefractionScale();
        c += 1.0f * float(elapsed);
        INF << Core::toString(c) << L"\n";
        _pBill3->setRefractionScale(c);
    }

    Core::List<Renderer::Billboard> & bills = _pBill4->beginUpdate();

    for(int32 ii=0; ii < bills.size(); ii++)
        bills[ii].time = float(fmod(bills[ii].time + elapsed * 0.25, 1.0));

    _pBill4->endUpdate();
}

void SceneBillboards::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);
    _pRenderer->renderScene(_pRenderer->getDefaultView(), _pScene, _pCamera);
}




