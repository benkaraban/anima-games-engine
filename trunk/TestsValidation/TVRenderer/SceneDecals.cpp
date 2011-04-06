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
#include <Workflow/PicBankGeneration.h>
#include "SceneDecals.h"

SceneDecals::SceneDecals(const TestSceneData & data) :
    ITestScene(data)
{
    _prevCameraWorld.setZero();
    updateCamera();
    _alpha = 0.0;
    _beta = 0.0;
}

bool SceneDecals::initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow)
{
    bool result = true;

    try
    {
        _pScene = _pRenderer->createScene();
        _pHUD = _pRenderer->createHUD();

        // Textes

        Ptr<Renderer::IFont> pFont1 = _pRenderer->createFont(L"Arial", 20);
        Ptr<Renderer::IFont> pFont2 = _pRenderer->createFont(L"Segoe Script", 20);

        Ptr<Renderer::IText> pText1 = _pHUD->createText(pFont1, L"This is a test.");
        Ptr<Renderer::IText> pText2 = _pHUD->createText(pFont1, L"This is a bold test.");
        Ptr<Renderer::IText> pText3 = _pHUD->createText(pFont1, L"This is an italic test.");
        Ptr<Renderer::IText> pText4 = _pHUD->createText(pFont1, L"This is a bold italic test.");

        Ptr<Renderer::IText> pText5 = _pHUD->createText(pFont2, L"Yet another test.");
        Ptr<Renderer::IText> pText6 = _pHUD->createText(pFont2, L"It fucking works guys.");

        pText1->setPosition(Core::Vector2f(20.0f, 40.0f));
        pText2->setPosition(Core::Vector2f(20.0f, 65.0f));
        pText3->setPosition(Core::Vector2f(20.0f, 90.0f));
        pText4->setPosition(Core::Vector2f(20.0f, 115.0f));

        pText5->setPosition(Core::Vector2f(20.0f, 160.0f));
        pText6->setPosition(Core::Vector2f(20.0f, 185.0f));

        pText2->setBold(true);
        pText3->setItalic(true);
        pText4->setBold(true);
        pText4->setItalic(true);

        pText2->setColor(Core::Vector4f(1.00f, 0.25f, 0.25f, 1.00f));
        pText3->setColor(Core::Vector4f(0.25f, 1.00f, 0.25f, 0.50f));
        pText4->setColor(Core::Vector4f(0.75f, 0.75f, 1.00f, 0.25f));

        pText5->setColor(Core::Vector4f(0.75f, 0.75f, 1.00f, 1.00f));
        pText6->setColor(Core::Vector4f(0.75f, 1.00f, 1.00f, 0.25f));

        pText1->setPriority(1);
        pText2->setPriority(1);
        pText3->setPriority(1);
        pText4->setPriority(1);
        pText5->setPriority(1);
        pText6->setPriority(1);

        // Images
        Ptr<Assets::PictureBank> pPBKData = Workflow::generatePictureBank(L"Data/TUData/Pictures/Bank00", 256, Assets::TEX_FORMAT_ARGB8);
        Ptr<Renderer::IPictureBank> pPBK = _pRenderer->uploadPictureBank(pPBKData);

        const Renderer::PictureInfos & pic1 = pPBK->getPictureInfos(L"source-normmap.jpg");
        
        Ptr<Renderer::IRectangle> pRect1 = _pHUD->createRectangle(pic1);
        Ptr<Renderer::IRectangle> pRect2 = _pHUD->createRectangle(pic1);
        Ptr<Renderer::IRectangle> pRect3 = _pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);

        pRect1->setPosition(Core::Vector2f(100.0f,  50.0f));
        pRect2->setPosition(Core::Vector2f(170.0f,  50.0f));
        pRect3->setPosition(Core::Vector2f(100.0f, 120.0f));

        pRect1->setSize(Core::Vector2f(float(pic1.width), float(pic1.height)));
        pRect2->setSize(Core::Vector2f(float(pic1.width), float(pic1.height)));
        pRect3->setSize(Core::Vector2f(float(pic1.width), float(pic1.height)));

        pRect1->setPriority(0);
        pRect2->setPriority(2);
        pRect3->setPriority(0);

        pRect3->setColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.5f));

        // Fog
        Renderer::FogSettings fog(Core::Vector4f(0.3f, 0.1f, 0.1f, 1.0f), 300.0f, 1500.0f, 0.0f);
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
        mat1.shininess = 45.0;

        _pScene2 = _pRenderer->createScene();

        Ptr<Renderer::IMeshInstance> pTeapot(_pScene2->createMeshInstance(pRenderMesh));
        pTeapot->setMaterial(mat1);
        pTeapot->setWorldMatrix(Core::Matrix4f(Core::TRANSLATION, 0.0f, -30.0f, 0.0f));

        // Texture

        Ptr<Renderer::ITextureMap> pTexMap00(loadTexture(pVFS, L"/Data/TUData/Textures/decal00.tga", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pTexMap01(loadTexture(pVFS, L"/Data/TUData/Textures/decal01.tga", Workflow::TEXTURE_COLORMAP));
        Ptr<Renderer::ITextureMap> pTexMap02(loadTexture(pVFS, L"/Data/TUData/Textures/decal02.tga", Workflow::TEXTURE_COLORMAP));
        //Ptr<Renderer::ITextureMap> pTexMap03(loadTexture(pVFS, L"/Data/TUData/Textures/stones01-normal.jpg", Workflow::TEXTURE_NORMALMAP));
        Ptr<Renderer::ITextureMap> pTexMap03(loadTexture(pVFS, L"/Data/TUData/Textures/Water0.jpg", Workflow::TEXTURE_NORMALMAP));
        Ptr<Renderer::ITextureMap> pTexMap04(loadTexture(pVFS, L"/Data/TUData/Textures/refrac00.jpg", Workflow::TEXTURE_NORMALMAP));

        // Texture animée 0

        Workflow::TextureOptions options;
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

        Ptr<Assets::Texture> pTexAnim00(Workflow::generateTexture3D(options, images));
        Ptr<Renderer::ITextureMap> pTexMapAnim00(_pRenderer->uploadTexture(pTexAnim00));
        Ptr<Renderer::ITextureMap> pTexMapAnim01(loadTexture(pVFS, L"/Data/TUData/Textures/fire-sprite2.jpg", Workflow::TEXTURE_COLORMAP));

        // Instances

        Ptr<Renderer::IDecal> decal00(_pScene->createDecal(pTexMap00, Renderer::DECAL_LERP));
        Ptr<Renderer::IDecal> decal01(_pScene->createDecal(pTexMap00, Renderer::DECAL_LERP));
        Ptr<Renderer::IDecal> decal02(_pScene->createDecal(pTexMap00, Renderer::DECAL_LERP));
        Ptr<Renderer::IDecal> decal03(_pScene->createDecal(pTexMap00, Renderer::DECAL_LERP));
        Ptr<Renderer::IDecal> decal04(_pScene->createDecal(pTexMap00, Renderer::DECAL_LERP));
        Ptr<Renderer::IDecal> decal05(_pScene->createDecal(pTexMap00, Renderer::DECAL_LERP));

        Ptr<Renderer::IDecal> decal06(_pScene->createDecal(pTexMap01, Renderer::DECAL_LERP));
        Ptr<Renderer::IDecal> decal07(_pScene->createDecal(pTexMap01, Renderer::DECAL_LERP));

        Ptr<Renderer::IDecal> decal08(_pScene->createDecal(pTexMap02, Renderer::DECAL_LERP));
        Ptr<Renderer::IDecal> decal09(_pScene->createDecal(pTexMap02, Renderer::DECAL_LERP));

        Ptr<Renderer::IDecal> decal10(_pScene->createDecal(pTexMapAnim00, Renderer::DECAL_LERP));
        Ptr<Renderer::IDecal> decal11(_pScene->createDecal(pTexMapAnim01, Renderer::DECAL_ADD));

        Ptr<Renderer::IDecal> decal12(_pScene->createDecal(pTexMap03, Renderer::DECAL_REFRAC));

        _pDecalRefrac = decal12;

        decal11->setSpriteLayout(4, 4);
        
        decal00->setSize(40.0f);
        decal01->setSize(40.0f, 30.0f);
        decal02->setSize(30.0f, 40.0f);
        decal03->setSize(40.0f);
        decal04->setSize(40.0f, 30.0f);
        decal05->setSize(30.0f, 40.0f);

        decal06->setSize(60.0f);
        decal07->setSize(60.0f);

        decal08->setSize(60.0f);
        decal09->setSize(60.0f);

        decal10->setSize(60.0f);
        decal11->setSize(60.0f);

        decal12->setSize(100.0f);

        decal00->setColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.00f));
        decal01->setColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.00f));
        decal02->setColor(Core::Vector4f(0.0f, 0.0f, 1.0f, 1.00f));
        decal03->setColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.10f));
        decal04->setColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 0.25f));
        decal05->setColor(Core::Vector4f(0.0f, 0.0f, 1.0f, 0.50f));

        decal06->setColor(Core::Vector4f(0.5f, 0.8f, 0.9f, 1.00f));
        decal07->setColor(Core::Vector4f(0.5f, 0.8f, 0.9f, 1.00f));
        decal07->setGlow(Core::Vector4f(0.5f, 0.8f, 0.9f, 1.00f));

        decal08->setColor(Core::Vector4f(0.5f, 0.2f, 0.2f, 1.00f));
        decal09->setColor(Core::Vector4f(0.5f, 0.2f, 0.2f, 1.00f));
        decal09->setGlow(Core::Vector4f(0.5f, 0.2f, 0.2f, 1.00f));

        decal08->setMode(Renderer::DECAL_ADD);
        decal09->setMode(Renderer::DECAL_ADD);

        decal10->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.75f));
        decal11->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.75f));

        decal12->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

        Core::Matrix4f world00(Core::TRANSLATION, -50.0,  20.0, -50.0);
        Core::Matrix4f world01(Core::TRANSLATION,   0.0,  20.0, -50.0);
        Core::Matrix4f world02(Core::TRANSLATION,  50.0,  20.0, -50.0);
        Core::Matrix4f world03(Core::TRANSLATION, -50.0, -20.0, -50.0);
        Core::Matrix4f world04(Core::TRANSLATION,   0.0, -20.0, -50.0);
        Core::Matrix4f world05(Core::TRANSLATION,  50.0, -20.0, -50.0);

        Core::Matrix4f rotX(Core::ROTATION, f_PI_DIV_2, Core::ROT_X_PITCH);
        Core::Matrix4f rotY(Core::ROTATION, f_PI_DIV_2, Core::ROT_Y_YAW);
        Core::Matrix4f world06(Core::TRANSLATION,  100.0, -60.0,   0.0); world06.postMultiply(rotY);
        Core::Matrix4f world07(Core::TRANSLATION,  120.0, -45.0,  25.0); world07.postMultiply(rotY);

        Core::Matrix4f world08(Core::TRANSLATION,  100.0,  40.0,   0.0); world08.postMultiply(rotY);
        Core::Matrix4f world09(Core::TRANSLATION,  120.0,  65.0,  25.0); world09.postMultiply(rotY);
        Core::Matrix4f world10(Core::TRANSLATION, -120.0,  65.0,  25.0); world10.postMultiply(rotY);
        Core::Matrix4f world11(Core::TRANSLATION, -120.0, -45.0,  25.0); world11.postMultiply(rotY);
        Core::Matrix4f world12(Core::TRANSLATION,   50.0,  85.0,  25.0); world12.postMultiply(rotX);

        decal00->setWorldMatrix(world00);
        decal01->setWorldMatrix(world01);
        decal02->setWorldMatrix(world02);
        decal03->setWorldMatrix(world03);
        decal04->setWorldMatrix(world04);
        decal05->setWorldMatrix(world05);
        decal06->setWorldMatrix(world06);
        decal07->setWorldMatrix(world07);
        decal08->setWorldMatrix(world08);
        decal09->setWorldMatrix(world09);
        decal10->setWorldMatrix(world10);
        decal11->setWorldMatrix(world11);
        decal12->setWorldMatrix(world12);

        for(int32 ii=0; ii < 11; ii++)
            _boxes.push_back(_pScene->createAABox());

        _boxes[ 0]->setBox(decal00->getBoundingBox());
        _boxes[ 1]->setBox(decal01->getBoundingBox());
        _boxes[ 2]->setBox(decal02->getBoundingBox());
        _boxes[ 3]->setBox(decal03->getBoundingBox());
        _boxes[ 4]->setBox(decal04->getBoundingBox());
        _boxes[ 5]->setBox(decal05->getBoundingBox());
        _boxes[ 6]->setBox(decal06->getBoundingBox());
        _boxes[ 7]->setBox(decal07->getBoundingBox());
        _boxes[ 8]->setBox(decal08->getBoundingBox());
        _boxes[ 9]->setBox(decal09->getBoundingBox());
        _boxes[10]->setBox(pTeapot->getBoundingBox());

        _pDecalAnim01 = decal10;
        _pDecalAnim02 = decal11;

        setBoxesVisible(false);

        _pCamera = _pScene->createPerspectiveCamera(f_PI_DIV_4, pRenderWindow->getWidth() / (float) pRenderWindow->getHeight(), 10.0, 10000.0);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising teapot scene : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}

void SceneDecals::update(double elapsed)
{
    ITestScene::update(elapsed);

    _alpha = float(fmod(_alpha + 1.0 * elapsed, double(f_PI)));
    _beta = float(fmod(_beta + 1.0 * elapsed, double(f_PI_MUL_2)));

    float cb = float(Core::L_cos(_beta));
    float sb = float(Core::L_sin(_beta));
    float sa = float(Core::L_sin(_alpha));

    Renderer::PostFX fx = _pCamera->getPostFX();

    Core::Matrix4f view;
    Core::Matrix4f world;

    _pCamera->getViewMatrix(view);
    _pCamera->getWorldMatrix(world);

    if(!_prevCameraWorld.isZero())
    {
        Core::Vector3f camMove = world.getTVector() - _prevCameraWorld.getTVector();
        Core::Vector3f viewMove = view.apply3x3(camMove);
        //viewMove *= 0.10f;
        static const float MM = 2.0f;
        viewMove.clamp(Core::Vector3f(-MM), Core::Vector3f(MM));
        _moveBuffer.push_back(viewMove);
    }

    if(_moveBuffer.size() >= 50)
    {
        Core::Vector3f viewMove(Core::Vector3f::ZERO);

        for(int32 ii=0; ii < _moveBuffer.size(); ii++)
            viewMove += _moveBuffer[ii];

        viewMove /= float(_moveBuffer.size());

        //fx.blurDir1 = Core::Vector2f(viewMove.x, -viewMove.y);
        //fx.radialBlurFactor = std::max(0.0f, std::min(1.0f, abs(0.75f * viewMove.z)));

        _moveBuffer.erase(_moveBuffer.begin());
    }

    //fx.fadeColor = Core::Vector4f(0.0f, 0.0f, 1.0f, sa);
    //fx.saturation = sa;
    //fx.blurDir1 = Core::Vector2f(0.5f + 0.25f * cb, 0.0f);
    //fx.blurDir2 = Core::Vector2f(0.0f, 0.5f + 0.25f * sb);
    //fx.radialBlurFactor = sa;
    //fx.radialBlurCenter = Core::Vector2f(0.5f + 0.25f * cb, 0.5f + 0.25f * sb);
    fx.timeBlurOn = true;
    fx.timeBlurFactor = 0.001f;//float(1.0 - 5.0 * elapsed);
    _pCamera->setPostFX(fx);

    _prevCameraWorld = world;

    if(_pInput->isKeyDown('L'))
    {
        Core::Vector4f c = _pDecalRefrac->getColor();
        c.a -= 1.0f * float(elapsed);
        INF << Core::toString(c.a) << L"\n";
        _pDecalRefrac->setColor(c);
    }
    if(_pInput->isKeyDown('M'))
    {
        Core::Vector4f c = _pDecalRefrac->getColor();
        c.a += 1.0f * float(elapsed);
        INF << Core::toString(c.a) << L"\n";
        _pDecalRefrac->setColor(c);
    }

    _pDecalAnim01->setTimeKey(float(fmod(_pDecalAnim01->getTimeKey() + elapsed * 0.25, 1.0)));
    _pDecalAnim02->setTimeKey(float(fmod(_pDecalAnim02->getTimeKey() + elapsed * 0.25, 1.0)));
}

void SceneDecals::render()
{
    _pCamera->setWorldMatrix(_matWorldCamera);

    Core::List<Ptr<Renderer::IScene> > scenes;
    scenes.push_back(_pScene);
    scenes.push_back(_pScene2);

    _pRenderer->renderScenes(_pRenderer->getDefaultView(), scenes, _pCamera);
    _pRenderer->renderHUD(_pRenderer->getDefaultView(), _pHUD);
}



