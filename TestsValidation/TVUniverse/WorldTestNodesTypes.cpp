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
#include <Core/Math/MathTools.h>
#include <Universe/Particles/EmitterShapeCone.h>
#include <Universe/Particles/EmitterShapeModel.h>
#include <Universe/Particles/EmitterShapeSpline.h>
#include <Universe/Particles/PartEffectAnimate.h>
#include <Universe/Particles/PartEffectFade.h>
#include <Universe/Particles/PartEffectForce.h>
#include <Universe/Particles/PartRepresentationBillboard.h>
#include <Universe/Tasks/TurnAround.h>
#include <Images/Image.h>
#include <Packing/TexCompressionOnTheFly.h>
#include <Workflow/TexCompression.h>
#include "WorldTestNodesTypes.h"

static const bool VISIBLE_BOUNDING_BOXES = false;

//-----------------------------------------------------------------------------
void WorldTestNodesTypes::initalise(const Ptr<Renderer::IRenderer> & pRenderer, 
                                    const Ptr<Core::VirtualFileSystem> & pVFS, 
                                    const Ptr<Window::InputManager> & pInput, 
                                    const Ptr<Audio::IDevice> & pDevice)
{
    _pRenderer = pRenderer;
    _pVFS = pVFS;
    _pAudioDevice = pDevice;
    _pWorld = Ptr<Universe::World>(new Universe::World(pRenderer, pVFS, null, _pAudioDevice));
    _pWorld->getRessourcesPool()->setOnTheFlyTextureGenerator(Ptr<Universe::IOnTheFlyTextureGenerator>(new Packing::OnTheFlyTextureGenerator()));
    _pInput = pInput;
    

    {
        Core::CRSpline spline;
        spline.getPoints().push_back(Core::Vector3f(0.0f, 0.0f, 0.0f));
        spline.getPoints().push_back(Core::Vector3f(10.0f, 5.0f, 5.0f));
        spline.getPoints().push_back(Core::Vector3f(15.0f, 15.0f, -10.0f));
        spline.getPoints().push_back(Core::Vector3f(25.0f, 5.0f, 0.0f));

        Core::Trajectory traj1(spline);
        Ptr<Core::XMLDocument> pXMLDoc(new Core::XMLDocument(L"root"));
        Ptr<Core::XMLNode> pXMLNode(new Core::XMLNode(L"node"));

        pXMLDoc->addNode(pXMLNode);
        traj1.exportXML(pXMLNode);
        pXMLDoc->saveDocument(L"Local/test.spline.xml");
    }

#if 1
    {
        Core::List<Ptr<Images::Image> > images;
        images.push_back(Ptr<Images::Image>(new Images::Image()));
        images.push_back(Ptr<Images::Image>(new Images::Image()));
        images.push_back(Ptr<Images::Image>(new Images::Image()));
        images.push_back(Ptr<Images::Image>(new Images::Image()));

        Ptr<Core::InputStream> pInputAnim00(pVFS->readFile(L"/TUDataRaw/Textures/Anim00/00.jpg"));
        Ptr<Core::InputStream> pInputAnim01(pVFS->readFile(L"/TUDataRaw/Textures/Anim00/01.jpg"));
        Ptr<Core::InputStream> pInputAnim02(pVFS->readFile(L"/TUDataRaw/Textures/Anim00/02.jpg"));
        Ptr<Core::InputStream> pInputAnim03(pVFS->readFile(L"/TUDataRaw/Textures/Anim00/03.jpg"));

        images[0]->load((Core::SeekableInputStream &)*pInputAnim00);
        images[1]->load((Core::SeekableInputStream &)*pInputAnim01);
        images[2]->load((Core::SeekableInputStream &)*pInputAnim02);
        images[3]->load((Core::SeekableInputStream &)*pInputAnim03);

        Workflow::TextureOptions options;
        Ptr<Assets::Texture> pTexAnim00(Workflow::generateTexture3D(options, images));
        pVFS->write(L"/TUDataRaw/Textures/TexAnim00.tex", *pTexAnim00);
    }
#endif

#if 0
    {
        Ptr<Images::Image> pImageFire(new Images::Image());

        Ptr<Core::InputStream> pInputFire(pVFS->readFile(L"/TestsValidation/TVData/Textures/fire-sprite2.jpg"));
        pImageFire->load((Core::SeekableInputStream &)*pInputFire);

        Workflow::TextureOptions options;
        Ptr<Assets::Texture> pTexAnim01(Workflow::generateTexture(options, pImageFire));
        pVFS->write(L"/TestsValidation/TVData/Textures/fire-sprite.tex", *pTexAnim01);
    }
#endif

    // Sun

    Renderer::MainLightSettings light = _pWorld->getMainLightSettings();
    light.ambient  = Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    light.diffuse0 = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    light.diffuse1 = Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    light.diffuse2 = Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    light.specular = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    light.mulIntensity = 1.0f;
    light.sunAngleToZenith = 0.0f;
    light.sunAngleToNorth = 1.0f;
    light.dynamicShadowOpacity = 0.5f;
    light.dynamicShadowMiddle = 10.0f;
    light.dynamicShadowMiddleBlend = 5.0f;
    light.dynamicShadowEnd = 50.0f;
    light.dynamicShadowEndBlend = 0.0f;
    light.bloomThreshold = 0.0f;
    light.bloomFactor = 0.0f;
    light.occlusionContrast = 0.0f;
    light.occlusionLuminosity = 0.0f;

    // Ciel

    _pWorld->setSkyTexture(L"/TUData/Textures/FullskiesSunset0024_1_S.tex");
    _pWorld->setSkyColor(Core::Vector4f(1.0, 1.0, 1.0, 0.0));
    _pWorld->setSkyHorizon(-0.25);

    // Fog

    _pWorld->setFogColor(Core::Vector4f(0.15f, 0.1f, 0.1f, 0.8f));
    _pWorld->setFogRange(10.0f, 20.0f);
    _pWorld->setFogSkyOpacity(0.0f);

    // Teapot

    Ptr<Universe::NodeMesh> pMesh(_pWorld->createMesh(L"/TUData/Models/teapot.mdl", L"Teapot01"));

    pMesh->beginMatrixUpdate();
    pMesh->setWorldPosition(Core::Vector3f(0.0, -0.5, 0.0));
    pMesh->setLocalScale(Core::Vector3f(0.01f, 0.01f, 0.01f));
    pMesh->endMatrixUpdate();
    pMesh->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);

    pMesh->setNodeScript("taskRotateLoop(node, 0.0, 1.0, 0.0, 5.0):wait()\n taskRotateLoop(node, 0.0, -2.0, 0.0, 5.0)");

    pMesh->addTag(L"MY-MESH");
    pMesh->addTag(L"TEST-TAG");

    // Bitonio sauteur
    Ptr<Universe::NodeSkeleton> pSkeleton(_pWorld->createSkeleton(L"/TUData/Models/skinned.mdl"));
    Ptr<Universe::SkeletonSkin> pSkin(pSkeleton->createSkin(L"/TUData/Models/skinned.mdl"));

    pSkeleton->beginMatrixUpdate();
    pSkeleton->setWorldPosition(Core::Vector3f(1.0, -0.5, 0.0));
    pSkeleton->setLocalScale(Core::Vector3f(0.01f, 0.01f, 0.01f));
    pSkeleton->endMatrixUpdate();
    pSkeleton->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);

    pSkeleton->animate(0, 3.0, 0.25, 0.75, true);
    pSkeleton->addTag(L"MY-SKELETON");

    for(int32 ii=0; ii < pSkeleton->getSkinCount(); ii++)
        pSkeleton->getSkin(ii)->setBoundingBoxVisible(true);

    // Point Light

    Ptr<Universe::NodePointLight> pPointLight = _pWorld->createPointLight();
    pPointLight->setDiffuseColor(Core::Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
    pPointLight->setRadius(0.750f);

    pPointLight->beginMatrixUpdate();
    pPointLight->setWorldPosition(Core::Vector3f(0.0, 0.0, 0.0));
    pPointLight->setWorldDirection(Core::Vector3f(0.0, 0.0, -1.0));
    pPointLight->endMatrixUpdate();

    Ptr<Universe::NodeDecal> pDecalPL(_pWorld->createDecal(L"/TUData/Textures/part00.tex", Renderer::DECAL_LERP));
    pDecalPL->setColor(pPointLight->getDiffuseColor());
    pDecalPL->setSize(0.1f);
    pPointLight->addChild(pDecalPL);
    pPointLight->addTag(L"MY-POINT-LIGHT");

    // Decal

    Ptr<Universe::NodeDecal> pDecal(_pWorld->createDecal(L"/TUData/Textures/fire-sprite.tex", Renderer::DECAL_ADD));
    pDecal->setSpriteLayout(4, 4);
    pDecal->animateSprite(8.0, 0.25, 0.75, true);
    pDecal->beginMatrixUpdate();
    pDecal->setWorldPosition(pSkeleton->getWorldPosition() + Core::Vector3f(0.0, -0.50, 0.0));
    pDecal->localPitch(f_PI_DIV_2);
    pDecal->endMatrixUpdate();
    pDecal->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);

    pDecal->setSize(1.0f);
    pDecal->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    pDecal->addTag(L"MY-DECAL");

    pDecal->setNodeScript(""
        "if node:getLocalScale().x > 0.0 then \n"
        "    print('toto') \n"
        "end"
        "");

    // Générateur particules
    Ptr<Universe::NodeEmitter> pEmitter = null;
    {
        pEmitter = Ptr<Universe::NodeEmitter>(_pWorld->createBillboardEmitter());

        Ptr<Universe::IPartRepresentation> pRep(pEmitter->getRepresentation());
        LM_ASSERT(pRep->getType() == Universe::PR_BILLBOARD);
        Ptr<Universe::PartRepresentationBillboard> pRepBill = LM_DEBUG_PTR_CAST<Universe::PartRepresentationBillboard>(pRep);

        pRepBill->setTexture(L"/TUData/Textures/part00.tex");
        pRepBill->setMode(Renderer::BILLBOARD_LERP);

        pEmitter->beginMatrixUpdate();
        pEmitter->setWorldPosition(Core::Vector3f(2.0, -0.5, 0.0));
        pEmitter->endMatrixUpdate();

        pEmitter->setPeriod(0.01f, 0.01f);
        pEmitter->setStartColor(Core::Vector4f(1.0, 1.0, 0.0, 1.0), Core::Vector4f(1.0, 0.0, 0.0, 1.0));
        pEmitter->setStartWidth(0.8f, 0.9f);
        pEmitter->setStartHeight(0.8f, 0.9f);
        pEmitter->setStartAngle(0.0, f_PI_MUL_2);

        //Ptr<Universe::EmitterShapeSpline> pShape(new Universe::EmitterShapeSpline(_pWorld->getRessourcesPool()));
        //pShape->setSpline(L"/Local/test.spline.xml");
        //pEmitter->setShape(pShape);
        //pEmitter->setStickyParticles(true);

        Ptr<Universe::EmitterShapeModel> pShape(new Universe::EmitterShapeModel(_pWorld->getRessourcesPool()));
        pShape->setModel(L"/TUData/Models/simplescene.mdl", L"Teapot01");
        pEmitter->setShape(pShape);
        pEmitter->setLocalScale(Core::Vector3f(0.01f));
        pEmitter->setStartWidth(0.05f, 0.05f);
        pEmitter->setStartHeight(0.05f, 0.05f);

        //Ptr<Universe::EmitterShapeCone> pShape(new Universe::EmitterShapeCone());
        //pShape->setRadius(0.0f, 0.05f);
        //pShape->setAngle(0.0f, f_PI);
        //pShape->setSpeed(0.01f, 0.01f);
        //pEmitter->setShape(pShape);

        Ptr<Universe::PartEffectFade2> pEffectFade(new Universe::PartEffectFade2(
                Core::Vector4f(0.8f, 0.8f, 1.0f, 0.0f),
                Core::Vector4f(0.8f, 0.8f, 1.0f, 1.0f),
                Core::Vector4f(0.8f, 0.8f, 1.0f, 0.0f),
                0.5f,
                1.0f,
                true,
                false));

        Ptr<Universe::PartEffectForce> pEffectForce(new Universe::PartEffectForce(
                Core::Vector3f(0.0, 1.0, 0.0),
                0.5));

        pEmitter->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);

        pEmitter->addEffect(pEffectFade);
        //pEmitter->addEffect(pEffectForce);
        pEmitter->addTag(L"MY-EMITTER");
    }


    // Générateur particules SPRITES
    {
        Ptr<Universe::NodeEmitter> pEmitter2(_pWorld->createBillboardEmitter());

        Ptr<Universe::IPartRepresentation> pRep(pEmitter2->getRepresentation());
        LM_ASSERT(pRep->getType() == Universe::PR_BILLBOARD);
        Ptr<Universe::PartRepresentationBillboard> pRepBill = LM_DEBUG_PTR_CAST<Universe::PartRepresentationBillboard>(pRep);

        pRepBill->setTexture(L"/TUData/Textures/fire-sprite.tex");
        pRepBill->setMode(Renderer::BILLBOARD_ADD);
        pRepBill->setSpriteLayout(4, 4);

        pEmitter2->setWorldPosition(Core::Vector3f(-4.0, -0.5, 0.0));

        pEmitter2->setPeriod(2.0f, 2.0f);
        pEmitter2->setStartColor(Core::Vector4f(1.0, 1.0, 1.0, 1.0), Core::Vector4f(1.0, 1.0, 1.0, 1.0));
        pEmitter2->setStartWidth(0.5, 0.5);
        pEmitter2->setStartHeight(0.5, 0.5);

        Ptr<Universe::EmitterShapeCone> pShape(new Universe::EmitterShapeCone());
        pShape->setRadius(0.0f, 0.05f);
        pShape->setAngle(0.0f, f_PI);
        pShape->setSpeed(0.01f, 0.01f);
        pEmitter2->setShape(pShape);

        Ptr<Universe::PartEffectFade2> pEffectFade(new Universe::PartEffectFade2(
                Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f),
                Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
                Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f),
                0.5f,
                2.0f,
                true,
                false));

        Ptr<Universe::PartEffectAnimate> pEffectAnim(new Universe::PartEffectAnimate(
            0.0f, 
            1.0f,
            1.0f,
            true,
            false));

        Ptr<Universe::PartEffectForce> pEffectForce(new Universe::PartEffectForce(
                Core::Vector3f(0.0, 1.0, 0.0),
                0.5));

        pEmitter2->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);

        pEmitter2->addEffect(pEffectFade);
        pEmitter2->addEffect(pEffectForce);
        pEmitter2->addEffect(pEffectAnim);
        pEmitter2->addTag(L"MY-EMITTER");
        pEmitter2->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);
    }

    //Node Water Patch

    Ptr<Universe::NodeWaterPatch> pWaterPatch(_pWorld->createWaterPatch(200.0f, 200.0f));

    _pWorld->setWaterLevel(-5.0f);

    pWaterPatch->beginMatrixUpdate();
    pWaterPatch->setWorldPosition(Core::Vector3f(-10.0f, 0.0f, 50.0f));
    pWaterPatch->endMatrixUpdate();

    pWaterPatch->setLayerTexture(0L, L"/TUData/Textures/Water1.tex");
    pWaterPatch->setLayerScaleU(0L, 1.5f);
    pWaterPatch->setLayerScaleV(0L, 1.5f);
    pWaterPatch->setLayerSpeedU(0L, 0.01f);
    pWaterPatch->setLayerSpeedU(0L, -0.01f);
    pWaterPatch->setLayerAmplitude(0L, 1.f);

    pWaterPatch->setLayerTexture(1L, L"/TUData/Textures/Water1.tex");
    pWaterPatch->setLayerScaleU(1L, 1.5f);
    pWaterPatch->setLayerScaleV(1L, 1.5f);
    pWaterPatch->setLayerSpeedU(1L, 0.01f);
    pWaterPatch->setLayerSpeedU(1L, 0.01f);
    pWaterPatch->setLayerAmplitude(1L, 1.0f);

    pWaterPatch->setLayerTexture(2L, L"/TUData/Textures/Water2.tex");
    pWaterPatch->setLayerScaleU(2L, 1.0f);
    pWaterPatch->setLayerScaleV(2L, 1.0f);
    pWaterPatch->setLayerSpeedU(2L, 0.00f);
    pWaterPatch->setLayerSpeedU(2L, -0.00f);
    pWaterPatch->setLayerAmplitude(2L, 0.00f);

    pWaterPatch->setLayerTexture(3L, L"/TUData/Textures/Water2.tex");
    pWaterPatch->setLayerScaleU(3L, 1.0f);
    pWaterPatch->setLayerScaleV(3L, 1.0f);
    pWaterPatch->setLayerSpeedU(3L, 0.00f);
    pWaterPatch->setLayerSpeedU(3L, 0.00f);
    pWaterPatch->setLayerAmplitude(3L, 0.00f);

    pWaterPatch->setDiffuse(Core::Vector4f(0.0f, 0.10f, 0.15f, 1.0f));
    pWaterPatch->setSpecular(Core::Vector4f(0.04f, 0.04f, 0.04f, 1.0f));
    pWaterPatch->setShininess(32.0f);
    pWaterPatch->setReflectionScale(0.50f);
    pWaterPatch->setRefractionScale(0.25f);
    pWaterPatch->addTag(L"MY-WATER-PATCH");

    // Teapot instance

    Ptr<Universe::Node> pInst1(_pWorld->createInstance(L"/TUData/Models/teapot.mdl"));

    pInst1->beginMatrixUpdate();
    pInst1->setWorldPosition(Core::Vector3f(0.0, 0.5, 0.0));
    pInst1->setLocalScale(Core::Vector3f(0.01f, 0.01f, 0.01f));
    pInst1->endMatrixUpdate();
    pInst1->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);

    pInst1->setNodeScript("taskRotateLoop(node, 0.0, -1.0, 0.0, 5.0):wait()\n taskRotateLoop(node, 0.0, 2.0, 0.0, 5.0)");

    LM_ASSERT(pInst1->getChildWithTag(L"Teapot01", true) != null);
    LM_ASSERT(pInst1->getNodeType() == Universe::NODE_SKELETON);

    // Bitonio instance

    Ptr<Universe::Node> pInst2(_pWorld->createInstance(L"/TUData/Models/skinned.mdl"));

    pInst2->beginMatrixUpdate();
    pInst2->setWorldPosition(Core::Vector3f(1.0, 0.5, 0.0));
    pInst2->setLocalScale(Core::Vector3f(0.01f, 0.01f, 0.01f));
    pInst2->endMatrixUpdate();
    pInst2->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);

    Ptr<Universe::NodeSkeleton> pInstSkeleton = LM_DEBUG_PTR_CAST<Universe::NodeSkeleton>(pInst2);
    pInstSkeleton->animate(0, 3.0, 0.25, 0.75, true);

    // Decal ori

    Ptr<Universe::NodeDecal> pDecalOri(_pWorld->createDecal(L"./Data/TUData/Textures/decal-arrow-up.tga", Renderer::DECAL_LERP));

    pDecalOri->setSize(1.0f, 2.0f);
    pDecalOri->setWorldPosition(Core::Vector3f(-1.0, 2.0, 0.0));
    pDecalOri->setWorldDirection(Core::Vector3f(1.0f, 0.0f, 0.0f));
    pDecalOri->setDecalNormal(Renderer::DECAL_NORMAL_Y);
    pDecalOri->setBillboard(true);


    // Flame instance

    Ptr<Universe::Node> pInst3(_pWorld->createInstance(L"/TUDataRaw/Universe/Flame00.node.xml"));

    pInst3->beginMatrixUpdate();
    pInst3->setWorldPosition(Core::Vector3f(2.0, 0.5, 0.0));
    pInst3->setWorldDirection(Core::Vector3f(0.0, 0.0, -1.0));
    pInst3->setLocalScale(Core::Vector3f(0.01f, 0.01f, 0.01f));
    pInst3->endMatrixUpdate();
    pInst3->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);
    pInst3->addTag(L"INSTANCE3");

    LM_ASSERT(pInst3->getNodeType() == Universe::NODE_EMITTER);

    // Node group

    Ptr<Universe::NodeGroup> pGroup(_pWorld->createGroup());

    pGroup->addChild(pInst1);
    pGroup->addChild(pInst2);
    pGroup->addChild(pEmitter);
    pGroup->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);

    // Spline
    
    Core::CRSpline spline;
    spline.getPoints().push_back(Core::Vector3f(0.0f, 0.0f, 0.0f));
    spline.getPoints().push_back(Core::Vector3f(0.0f, 0.0f, 0.0f));
    spline.getPoints().push_back(Core::Vector3f(0.0f, 0.0f, 0.0f));
    spline.getPoints().push_back(Core::Vector3f(10.0f, 5.0f, 5.0f));
    spline.getPoints().push_back(Core::Vector3f(15.0f, 15.0f, -10.0f));
    spline.getPoints().push_back(Core::Vector3f(25.0f, 5.0f, 0.0f));
    spline.getPoints().push_back(Core::Vector3f(25.0f, 5.0f, 0.0f));
    spline.getPoints().push_back(Core::Vector3f(25.0f, 5.0f, 0.0f));

    Ptr<Universe::NodeSpline> pSpline(_pWorld->createSpline());

    pSpline->setResolution(200);
    pSpline->setColor(Core::Vector4f(1.0f, 0.5f, 0.5f, 1.0f));
    pSpline->setSpline(spline);
    pSpline->setWorldPosition(Core::Vector3f(-30.0f, 0.0f, 0.0f));
    pSpline->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);

    //
    // Test du son dans Universe
    //
    if(pDevice != null)
    {
        Ptr<Universe::NodeListener> pListener = _pWorld->getNodeListener();
        pListener->setGain(5);

        pPointLight->addChild(pListener);

    
        Ptr<Universe::NodeSoundSource> pSoundSource = _pWorld->createSoundSource();
        pSoundSource->setGain(1.0f);
        pSoundSource->enqueue( L"/TUDataRaw/Sound/Footsteps.wav", true);

        pInst3->addChild(pSoundSource);
    }

    // Trail

    Ptr<Universe::NodeTrail> pTrail = _pWorld->createTrail();
    {
        pTrail->setWorldPosition(Core::Vector3f(10.0f, 0.0f, 0.0f));

        //pTrail->setDivisions(5);
        pTrail->setHeadWidth(1.0f);
        pTrail->setTailWidth(0.25f);

        pTrail->setHeadColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        pTrail->setTailColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));

        pTrail->setHeadGlow(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        pTrail->setTailGlow(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));

        pTrail->setBoundingBoxVisible(VISIBLE_BOUNDING_BOXES);
        pTrail->addTag(L"TRAIL");
    }


    {
        Core::XMLDocument xmlDocument;
        _pWorld->exportWorldXML(xmlDocument);
        if(!xmlDocument.saveDocument(L"./Local/tv-universe00.world.xml"))
            LM_ASSERT(false);

        Core::XMLDocument xmlDocument2;
        if(!xmlDocument2.loadDocument(L"./Local/tv-universe00.world.xml"))
            LM_ASSERT(false);

        _pWorld->importWorldXML(xmlDocument2);
    }


    // Caméra

    _camQuake.setPosition(Core::Vector3f(0.0, 0.0f, -5.0f));
    _pCamera = _pWorld->createPerspectiveCamera(f_PI * 0.25f, 1.0f, 0.1f, 100.0f);

    pTrail = LM_DEBUG_PTR_CAST<Universe::NodeTrail>(_pWorld->getFirstNodeWithTag(L"TRAIL"));

    Ptr<Universe::ITask> pTask(new Universe::TaskTurnAroundLoop(pTrail, Core::Vector3f(0.0f, 0.0f, 0.0f), 0.0f, 2.0f, 0.0f));
    _pWorld->registerTask(pTask);
    _pWorld->startWorldScripts();
}

void WorldTestNodesTypes::update(double elapsed)
{
    static float alpha = 0;
    alpha += 0.005f;

    Ptr<Universe::Node> pInstance = _pWorld->getFirstNodeWithTag(L"INSTANCE3");

    if(pInstance != null && pInstance->isAlive())
    {
        pInstance->beginMatrixUpdate();
        pInstance->setWorldPosition(Core::Vector3f(1.0f * cos(alpha), 0.0f, 1.0f * sin(alpha)));
        pInstance->endMatrixUpdate();
        if(alpha > 12)
            pInstance->kill();
    }

    if(_pInput->isKeyTyped('X'))
    {
        _pWorld->runInline(L"world:getFirstNodeWithTag('TRAIL'):resetTail()");
    }
        
    IWorldTest::update(elapsed);
}
//-----------------------------------------------------------------------------
