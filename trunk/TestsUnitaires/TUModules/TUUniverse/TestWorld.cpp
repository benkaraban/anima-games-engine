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
#include <Core/Logger.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/VFS/VFSXMLTranslator.h>
#include <Renderer/Stub/RendererStub.h>
#include <Universe/World.h>
#include <Universe/Particles/EmitterShapeCone.h>
#include <Universe/Particles/PartEffectFade.h>
#include <Universe/Particles/PartEffectForce.h>
#include <Universe/Particles/PartRepresentationBillboard.h>

void testSky(Universe::World & world)
{
    UTEST_START_TEST(L"Sky");
    String skyTexName = L"/TUData/Textures/FullskiesSunset0024_1_S.tex";
    world.setSkyTexture(skyTexName);
    UTEST_EQU(world.getSkyTexture(), skyTexName);

    Core::Vector4f skyColor(1.0f, 0.8f, 0.5f, 0.4f);
    world.setSkyColor(skyColor);
    UTEST_EQU(world.getSkyColor(), skyColor);

    float horizon = 120.0;
    world.setSkyHorizon(horizon);
    UTEST_EQU(world.getSkyHorizon(), horizon);

    UTEST_END_TEST;
}

void testFog(Universe::World & world)
{
    UTEST_START_TEST(L"Fog");
    Core::Vector4f fogColor(1.0f, 0.8f, 0.5f, 0.4f);
    world.setFogColor(fogColor);
    UTEST_EQU(world.getFogColor(), fogColor);

    float start;
    float end;
    world.setFogRange(0.0f, 100.0f);
    world.getFogRange(start, end);
    UTEST_EQU(start == 0.0f && end == 100.0f, true);

    world.setFogSkyOpacity(0.4f);
    float opacity = world.getFogSkyOpacity();
    UTEST_EQU(opacity, 0.4f);

    UTEST_END_TEST;
}


class TestTask : public Universe::ITask
{
public:
    TestTask(Universe::World & world)
    :   _world(world),
        _taskFinished(false)
    {}

    virtual ~TestTask() {}

    virtual void update(double elapsed)
    {
        Renderer::MainLightSettings light = _world.getMainLightSettings();
        light.sunAngleToNorth = 1.9f;
        _world.setMainLightSettings(light);
        _taskFinished = true;
    }

protected:
    virtual bool internalIsFinished() const
    {
        return _taskFinished;
    }

    Universe::World & _world;
    bool _taskFinished;
};

void testTask(Universe::World & world)
{
    UTEST_START_TEST(L"Task");

    Renderer::MainLightSettings light = world.getMainLightSettings();
    light.sunAngleToNorth = 0.9f;
    world.setMainLightSettings(light);

    UTEST_EQU(world.getMainLightSettings().sunAngleToNorth, 0.9f);
    Ptr<Universe::ITask> pTask(new TestTask(world));
    world.registerTask(pTask);
    world.update(1.0);
    UTEST_EQU(pTask->isFinished(), true);
    UTEST_EQU(world.getMainLightSettings().sunAngleToNorth, 1.9f);

    UTEST_END_TEST;
}

void testMesh(Universe::World & world)
{
    UTEST_START_TEST(L"Mesh");
    String modelName = L"/TUData/Models/teapot.mdl";
    String meshName = L"Teapot01";

    Ptr<Universe::NodeMesh> pMesh(world.createMesh(modelName, meshName));
    pMesh->addTag(L"MY-MESH");

    UTEST_EQU(pMesh->getModelName(), modelName);
    UTEST_EQU(pMesh->getMeshName(), meshName);

    UTEST_END_TEST;
}

void testSkeleton(Universe::World & world)
{
    UTEST_START_TEST(L"Skeleton");
    String skelName = L"/TUData/Models/skinned.mdl";
    String skinName = L"/TUData/Models/skinned.mdl";
    String meshName = L"CylinderThing";
    Ptr<Universe::NodeSkeleton> pSkeleton(world.createSkeleton(skelName));
    Ptr<Universe::SkeletonSkin> pSkin(pSkeleton->createSkin(skinName, meshName));

    pSkeleton->addTag(L"MY-SKEL");

    UTEST_EQU(pSkeleton->getModelName(), skelName);
    UTEST_EQU(pSkin->getModelName(), skinName);
    UTEST_EQU(pSkin->getMeshName(), meshName);

    UTEST_END_TEST;
}

void testDecal(Universe::World & world)
{
    UTEST_START_TEST(L"Decal");
    String textureName = L"/TUData/Textures/decal03.tex";
    Ptr<Universe::NodeDecal> pDecal(world.createDecal(textureName, Renderer::DECAL_LERP));

    pDecal->addTag(L"MY-DECAL");

    UTEST_EQU(pDecal->getTexture(), textureName);
    UTEST_EQU(pDecal->getMode(), Renderer::DECAL_LERP);
    pDecal->setMode(Renderer::DECAL_ADD);
    UTEST_EQU(pDecal->getMode(), Renderer::DECAL_ADD);

    UTEST_END_TEST;
}

void testBillboardEmitter(Universe::World & world)
{
    UTEST_START_TEST(L"Billboard Emitter");
    Ptr<Universe::NodeEmitter> pEmitter(world.createBillboardEmitter());

    Ptr<Universe::IPartRepresentation> pRep(pEmitter->getRepresentation());
    UTEST_EQU(pRep->getType(), Universe::PR_BILLBOARD);
    Ptr<Universe::PartRepresentationBillboard> pRepBill = LM_DEBUG_PTR_CAST<Universe::PartRepresentationBillboard>(pRep);
    String textureName = L"/TUData/Textures/part00.tex";
    pRepBill->setTexture(textureName);
    pRepBill->setMode(Renderer::BILLBOARD_ADD);

    UTEST_EQU(pRepBill->getTexture(), textureName);
    UTEST_EQU(pRepBill->getMode(), Renderer::BILLBOARD_ADD);

    float floatValue1;
    float floatValue2;
    Core::Vector4f vect4f1;
    Core::Vector4f vect4f2;
    Core::Vector4f vect4f3;

    pEmitter->setPeriod(0.05f, 0.10f);
    pEmitter->getPeriod(floatValue1, floatValue2);
    UTEST_EQU(floatValue1, 0.05f);
    UTEST_EQU(floatValue2, 0.10f);
    pEmitter->setStartColor(Core::Vector4f(1.0, 1.0, 0.0, 1.0), Core::Vector4f(1.0, 0.0, 0.0, 1.0));
    pEmitter->getStartColor(vect4f1, vect4f2);
    UTEST_EQU(Core::Vector4f(1.0, 1.0, 0.0, 1.0), vect4f1);
    UTEST_EQU(Core::Vector4f(1.0, 0.0, 0.0, 1.0), vect4f2);
    pEmitter->setStartWidth(50.0f, 45.0f);
    pEmitter->getStartWidth(floatValue1, floatValue2);
    UTEST_EQU(50.0f, floatValue1);
    UTEST_EQU(45.0f, floatValue2);
    pEmitter->setStartHeight(45.0, 50.0);
    pEmitter->getStartHeight(floatValue1, floatValue2);
    UTEST_EQU(45.0f, floatValue1);
    UTEST_EQU(50.0f, floatValue2);

    Ptr<Universe::EmitterShapeCone> pShape(new Universe::EmitterShapeCone());
    pShape->setRadius(0.0f, 5.0f);
    pShape->setAngle(0.0, f_PI);
    pShape->setSpeed(1.0f, 1.0f);
    pEmitter->setShape(pShape);

    UTEST_EQU(pEmitter->getShape()->getType(), Universe::SHAPE_CONE);
    Ptr<Universe::EmitterShapeCone> pShape2 = LM_DEBUG_PTR_CAST<Universe::EmitterShapeCone>(pEmitter->getShape());
    pShape2->getRadius(floatValue1, floatValue2);
    UTEST_EQU(0.0f, floatValue1);
    UTEST_EQU(5.0f, floatValue2);
    pShape2->getAngle(floatValue1, floatValue2);
    UTEST_EQU(0.0f, floatValue1);
    UTEST_EQU(f_PI, floatValue2);
    pShape2->getSpeed(floatValue1, floatValue2);
    UTEST_EQU(1.0f, floatValue1);
    UTEST_EQU(1.0f, floatValue2);

    Ptr<Universe::PartEffectFade2> pEffectFade(new Universe::PartEffectFade2(
            Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f),
            Core::Vector4f(0.5f, 0.5f, 0.0f, 1.0f),
            Core::Vector4f(0.3f, 0.0f, 0.0f, 0.0f),
            0.5f,
            2.0f,
            true,
            false));

    Ptr<Universe::PartEffectForce> pEffectForce(new Universe::PartEffectForce(
            Core::Vector3f(0.0f, 1.0f, 0.0f),
            50.0f));

    pEmitter->addEffect(pEffectFade);
    pEmitter->addEffect(pEffectForce);

    const Core::List<Ptr<Universe::IPartEffect> > & effects = pEmitter->getEffects();
    UTEST_EQU(effects.size(), 2);
    for(Core::List<Ptr<Universe::IPartEffect> >::const_iterator iEffect = effects.begin();
    iEffect != effects.end();
    ++iEffect)
    {
        if((*iEffect)->getType() == Universe::EFFECT_FADE2)
        {
            Ptr<Universe::PartEffectFade2> pFade2 = LM_DEBUG_PTR_CAST<Universe::PartEffectFade2>(*iEffect);
            UTEST_EQU(pFade2->getColor0(), Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
            UTEST_EQU(pFade2->getColor1(), Core::Vector4f(0.5f, 0.5f, 0.0f, 1.0f));
            UTEST_EQU(pFade2->getColor2(), Core::Vector4f(0.3f, 0.0f, 0.0f, 0.0f));
            UTEST_EQU(pFade2->getTime1(), 0.5f);
            UTEST_EQU(pFade2->getTime2(), 2.0f);
            UTEST_EQU(pFade2->killPartsAtEnd(), true);
        }
        else if((*iEffect)->getType() == Universe::EFFECT_FORCE)
        {
            Ptr<Universe::PartEffectForce> pForce = LM_DEBUG_PTR_CAST<Universe::PartEffectForce>(*iEffect);
            UTEST_EQU(pForce->getDirection(), Core::Vector3f(0.0f, 1.0f, 0.0f));
            UTEST_EQU(pForce->getAcceleration(), 50.0f);
        }
        else
            throw Core::Exception(L"Unexpected effect type.");
    }

    pEmitter->addTag(L"MY-EMITTER");

    UTEST_END_TEST;
}

void testPerspectiveCamera(Universe::World & world)
{
    UTEST_START_TEST(L"Perspective Camera");

    Ptr<Universe::NodePerspectiveCamera> pCamera(world.createPerspectiveCamera(f_PI * 0.25f, 1.0f, 10.0f, 1000.0f));
    UTEST_EQU(pCamera->getCamera()->getFOV(), f_PI * 0.25f);
    UTEST_EQU(pCamera->getCamera()->getAspectRatio(), 1.0f);
    UTEST_EQU(pCamera->getCamera()->getNearPlane(), 10.0f);
    UTEST_EQU(pCamera->getCamera()->getFarPlane(), 1000.0f);

    pCamera->addTag(L"MY-CAMERA");

    UTEST_END_TEST;
}

void testWaterPatch(Universe::World & world)
{
    UTEST_START_TEST(L"Water patch");
    Ptr<Universe::NodeWaterPatch> pWaterPatch(world.createWaterPatch(1000.0f, 750.0f));
    UTEST_EQU(pWaterPatch->getWidth(), 1000.0f);
    UTEST_EQU(pWaterPatch->getHeight(), 750.0f);

    String texture0 = L"/TUData/Textures/decal00.tex";
    String texture1 = L"/TUData/Textures/decal00.tex";
    String texture2 = L"/TUData/Textures/part00.tex";
    String texture3 = L"/TUData/Textures/stones01-diffuse.tex";

    pWaterPatch->setLayerTexture(0, texture0);
    pWaterPatch->setLayerScaleU(0, 1.5f);
    pWaterPatch->setLayerScaleV(0, 1.6f);
    pWaterPatch->setLayerSpeedV(0, 0.01f);
    pWaterPatch->setLayerSpeedV(0, -0.01f);
    pWaterPatch->setLayerAmplitude(0, 0.1f);
    pWaterPatch->setLayerTexture(1, texture1);
    pWaterPatch->setLayerScaleU(1, 1.5f);
    pWaterPatch->setLayerScaleV(1, 1.6f);
    pWaterPatch->setLayerSpeedU(1, -0.01f);
    pWaterPatch->setLayerSpeedV(1, 0.01f);
    pWaterPatch->setLayerAmplitude(1, 0.1f);
    pWaterPatch->setLayerTexture(2, texture2);
    pWaterPatch->setLayerScaleU(2, 1.0f);
    pWaterPatch->setLayerScaleV(2, 1.0f);
    pWaterPatch->setLayerSpeedU(2, 0.1f);
    pWaterPatch->setLayerSpeedV(2, 0.0f);
    pWaterPatch->setLayerAmplitude(2, 0.0f);
    pWaterPatch->setLayerTexture(3, texture3);
    pWaterPatch->setLayerScaleU(3, 1.0f);
    pWaterPatch->setLayerScaleV(3, 1.0f);
    pWaterPatch->setLayerSpeedU(3, -0.1f);
    pWaterPatch->setLayerSpeedV(3, 0.0f);
    pWaterPatch->setLayerAmplitude(3, 0.0f);

    pWaterPatch->setDiffuse(Core::Vector4f(0.0f, 0.10f, 0.15f, 1.0f));
    pWaterPatch->setSpecular(Core::Vector4f(0.04f, 0.04f, 0.04f, 1.0f));
    pWaterPatch->setShininess(32.0f);

    pWaterPatch->setFresnelMin(0.3f);
    pWaterPatch->setFresnelPower(7.0f);

    UTEST_EQU(pWaterPatch->getLayerTexture(1), texture1);
    UTEST_EQU(pWaterPatch->getLayerScaleU(1), 1.5f);
    UTEST_EQU(pWaterPatch->getLayerScaleV(1), 1.6f);
    UTEST_EQU(pWaterPatch->getLayerSpeedU(1), -0.01f);
    UTEST_EQU(pWaterPatch->getLayerSpeedV(1), 0.01f);
    UTEST_EQU(pWaterPatch->getLayerAmplitude(1), 0.1f);

    UTEST_EQU(pWaterPatch->getDiffuse(), Core::Vector4f(0.0f, 0.10f, 0.15f, 1.0f));
    UTEST_EQU(pWaterPatch->getSpecular(), Core::Vector4f(0.04f, 0.04f, 0.04f, 1.0f));
    UTEST_EQU(pWaterPatch->getShininess(), 32.0f);

    UTEST_EQU(pWaterPatch->getFresnelMin(), 0.3f);
    UTEST_EQU(pWaterPatch->getFresnelPower(), 7.0f);
    pWaterPatch->addTag(L"MY-WATER-PATCH");

    UTEST_END_TEST;
}

void testPointLight(Universe::World & world)
{
    UTEST_START_TEST(L"Point Light");

    Ptr<Universe::NodePointLight> pPointLight(world.createPointLight());
    pPointLight->setShadowCasting(true);
    pPointLight->setAmbientColor(Core::Vector4f(0.8f, 0.4f, 0.3f, 0.9f));
    pPointLight->setDiffuseColor(Core::Vector4f(0.7f, 0.3f, 0.2f, 0.8f));
    pPointLight->setSpecularColor(Core::Vector4f(0.9f, 0.5f, 0.4f, 1.0f));
    pPointLight->setRadius(150.0f);

    UTEST_EQU(pPointLight->getShadowCasting(), true);
    UTEST_EQU(pPointLight->getAmbientColor(), Core::Vector4f(0.8f, 0.4f, 0.3f, 0.9f));
    UTEST_EQU(pPointLight->getDiffuseColor(), Core::Vector4f(0.7f, 0.3f, 0.2f, 0.8f));
    UTEST_EQU(pPointLight->getSpecularColor(), Core::Vector4f(0.9f, 0.5f, 0.4f, 1.0f));
    UTEST_EQU(pPointLight->getRadius(), 150.0f);

    pPointLight->addTag(L"MY-POINT-LIGHT");

    UTEST_END_TEST;
}

void singleFunctionValueTest(Universe::World & world)
{
    //Mesh
    Ptr<Universe::Node> node = world.getFirstNodeWithTag(L"MY-MESH");

    UTEST_EQU(node->getNodeType(), Universe::NODE_MESH);
    Ptr<Universe::NodeMesh> pMesh = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(node);
    UTEST_EQU(pMesh->getModelName(), L"/TUData/Models/teapot.mdl");
    UTEST_EQU(pMesh->getMeshName(), L"Teapot01");

    //Skeleton
    node = world.getFirstNodeWithTag(L"MY-SKEL");
    UTEST_EQU(node->getNodeType(), Universe::NODE_SKELETON);
    Ptr<Universe::NodeSkeleton> pSkeleton = LM_DEBUG_PTR_CAST<Universe::NodeSkeleton>(node);
    UTEST_EQU(pSkeleton->getModelName(), L"/TUData/Models/skinned.mdl");

    //Decal
    node = world.getFirstNodeWithTag(L"MY-DECAL");
    UTEST_EQU(node->getNodeType(), Universe::NODE_DECAL);
    Ptr<Universe::NodeDecal> pDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(node);
    UTEST_EQU(pDecal->getTexture(), L"/TUData/Textures/decal03.tex");
    UTEST_EQU(pDecal->getMode(), Renderer::DECAL_ADD);

    //NodeEmitter
    node = world.getFirstNodeWithTag(L"MY-EMITTER");
    UTEST_EQU(node->getNodeType(), Universe::NODE_EMITTER);
    Ptr<Universe::NodeEmitter> pEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(node);

    Ptr<Universe::IPartRepresentation> pRep(pEmitter->getRepresentation());
    UTEST_EQU(pRep->getType(), Universe::PR_BILLBOARD);
    Ptr<Universe::PartRepresentationBillboard> pRepBill = LM_DEBUG_PTR_CAST<Universe::PartRepresentationBillboard>(pRep);
    UTEST_EQU(pRepBill->getTexture(), L"/TUData/Textures/part00.tex");
    UTEST_EQU(pRepBill->getMode(), Renderer::BILLBOARD_ADD);

    float floatValue1;
    float floatValue2;
    Core::Vector4f vect4f1;
    Core::Vector4f vect4f2;
    Core::Vector4f vect4f3;

    pEmitter->getPeriod(floatValue1, floatValue2);
    UTEST_EQU(floatValue1, 0.05f);
    UTEST_EQU(floatValue2, 0.10f);
    pEmitter->getStartColor(vect4f1, vect4f2);
    UTEST_EQU(Core::Vector4f(1.0, 1.0, 0.0, 1.0), vect4f1);
    UTEST_EQU(Core::Vector4f(1.0, 0.0, 0.0, 1.0), vect4f2);
    pEmitter->getStartWidth(floatValue1, floatValue2);
    UTEST_EQU(50.0f, floatValue1);
    UTEST_EQU(45.0f, floatValue2);
    pEmitter->getStartHeight(floatValue1, floatValue2);
    UTEST_EQU(45.0f, floatValue1);
    UTEST_EQU(50.0f, floatValue2);

    UTEST_EQU(pEmitter->getShape()->getType(), Universe::SHAPE_CONE);
    Ptr<Universe::EmitterShapeCone> pShape = LM_DEBUG_PTR_CAST<Universe::EmitterShapeCone>(pEmitter->getShape());
    pShape->getRadius(floatValue1, floatValue2);
    UTEST_EQU(0.0f, floatValue1);
    UTEST_EQU(5.0f, floatValue2);
    pShape->getAngle(floatValue1, floatValue2);
    UTEST_EQU(0.0f, floatValue1);
    UTEST_EQUE(f_PI, floatValue2, 0.000001f);
    pShape->getSpeed(floatValue1, floatValue2);
    UTEST_EQU(1.0f, floatValue1);
    UTEST_EQU(1.0f, floatValue2);

    const Core::List<Ptr<Universe::IPartEffect> > & effects = pEmitter->getEffects();
    UTEST_EQU(effects.size(), 2);
    for(Core::List<Ptr<Universe::IPartEffect> >::const_iterator iEffect = effects.begin();
        iEffect != effects.end();
        ++iEffect)
    {
        if((*iEffect)->getType() == Universe::EFFECT_FADE2)
        {
            Ptr<Universe::PartEffectFade2> pFade2 = LM_DEBUG_PTR_CAST<Universe::PartEffectFade2>(*iEffect);
            UTEST_EQU(pFade2->getColor0(), Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
            UTEST_EQU(pFade2->getColor1(), Core::Vector4f(0.5f, 0.5f, 0.0f, 1.0f));
            UTEST_EQU(pFade2->getColor2(), Core::Vector4f(0.3f, 0.0f, 0.0f, 0.0f));
            UTEST_EQU(pFade2->getTime1(), 0.5f);
            UTEST_EQU(pFade2->getTime2(), 2.0f);
            UTEST_EQU(pFade2->killPartsAtEnd(), true);
        }
        else if((*iEffect)->getType() == Universe::EFFECT_FORCE)
        {
            Ptr<Universe::PartEffectForce> pForce = LM_DEBUG_PTR_CAST<Universe::PartEffectForce>(*iEffect);
            UTEST_EQU(pForce->getDirection(), Core::Vector3f(0.0f, 1.0f, 0.0f));
            UTEST_EQU(pForce->getAcceleration(), 50.0f);
        }
        else
            throw Core::Exception(L"Unexpected effect type.");
    }

    //Perspective Camera
    node = world.getFirstNodeWithTag(L"MY-CAMERA");
    UTEST_EQU(node->getNodeType(), Universe::NODE_CAMERA);
    Ptr<Universe::NodePerspectiveCamera> pCamera = LM_DEBUG_PTR_CAST<Universe::NodePerspectiveCamera>(node);

    UTEST_EQUE(pCamera->getCamera()->getFOV(), f_PI * 0.25f, 0.000001f);
    UTEST_EQU(pCamera->getCamera()->getAspectRatio(), 1.0f);
    UTEST_EQU(pCamera->getCamera()->getNearPlane(), 10.0f);
    UTEST_EQU(pCamera->getCamera()->getFarPlane(), 1000.0f);

    //Water Patch
    node = world.getFirstNodeWithTag(L"MY-WATER-PATCH");
    UTEST_EQU(node->getNodeType(), Universe::NODE_WATER_PATCH);
    Ptr<Universe::NodeWaterPatch> pWaterPatch = LM_DEBUG_PTR_CAST<Universe::NodeWaterPatch>(node);
    UTEST_EQU(pWaterPatch->getWidth(), 1000.0f);
    UTEST_EQU(pWaterPatch->getHeight(), 750.0f);

    UTEST_EQU(pWaterPatch->getLayerTexture(1), L"/TUData/Textures/decal00.tex");
    UTEST_EQU(pWaterPatch->getLayerScaleU(1), 1.5f);
    UTEST_EQU(pWaterPatch->getLayerScaleV(1), 1.6f);
    UTEST_EQU(pWaterPatch->getLayerSpeedU(1), -0.01f);
    UTEST_EQU(pWaterPatch->getLayerSpeedV(1), 0.01f);
    UTEST_EQU(pWaterPatch->getLayerAmplitude(1), 0.1f);

    UTEST_EQU(pWaterPatch->getDiffuse(), Core::Vector4f(0.0f, 0.10f, 0.15f, 1.0f));
    UTEST_EQU(pWaterPatch->getSpecular(), Core::Vector4f(0.04f, 0.04f, 0.04f, 1.0f));
    UTEST_EQU(pWaterPatch->getShininess(), 32.0f);

    UTEST_EQU(pWaterPatch->getFresnelMin(), 0.3f);
    UTEST_EQU(pWaterPatch->getFresnelPower(), 7.0f);

    //Point Light
    node = world.getFirstNodeWithTag(L"MY-POINT-LIGHT");
    UTEST_EQU(node->getNodeType(), Universe::NODE_POINT_LIGHT);
    Ptr<Universe::NodePointLight> pPointLight = LM_DEBUG_PTR_CAST<Universe::NodePointLight>(node);

    UTEST_EQU(pPointLight->getShadowCasting(), true);
    UTEST_EQU(pPointLight->getAmbientColor(), Core::Vector4f(0.8f, 0.4f, 0.3f, 0.9f));
    UTEST_EQU(pPointLight->getDiffuseColor(), Core::Vector4f(0.7f, 0.3f, 0.2f, 0.8f));
    UTEST_EQU(pPointLight->getSpecularColor(), Core::Vector4f(0.9f, 0.5f, 0.4f, 1.0f));
    UTEST_EQU(pPointLight->getRadius(), 150.0f);

}

void testTags(Universe::World & world)
{
    UTEST_START_TEST(L"Tags");

    singleFunctionValueTest(world);

    UTEST_END_TEST;
}

void testImportExportXML(Universe::World & world, Universe::World & world2)
{
    UTEST_START_TEST(L"Import/Export XML");

    Core::XMLDocument xmlDocument;
    world.exportWorldXML(xmlDocument);
    UTEST_EQU(xmlDocument.saveDocument(L"./Data/TUData/Tmp/World.xml"), true);

    Core::XMLDocument xmlDocument2;
    UTEST_EQU(xmlDocument2.loadDocument(L"./Data/TUData/Tmp/World.xml"), true);
    world2.importWorldXML(xmlDocument2);

    singleFunctionValueTest(world2);

    UTEST_END_TEST;
}

void testWorld()
{
    UTEST_PACKAGE("World");
    Ptr<Renderer::IRenderer> pRenderer(new Renderer::RendererStub());
    Ptr<Core::VirtualFileSystem> pVFS;
    {
        Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());

        if(pXmlDocument->loadDocument(L"./Data/VFS-TUUniverse.xml"))
            pVFS = Core::VFSFromXMLNode(pXmlDocument->firstChildNode(L"vfs"));
        else
            throw Core::Exception(L"Unable to mount VFS.");

    }

    Universe::World world(pRenderer, pVFS);

    testSky(world);
    testFog(world);
    testTask(world);
    testMesh(world);
    testSkeleton(world);
    testDecal(world);
    testBillboardEmitter(world);
    testPerspectiveCamera(world);
    testWaterPatch(world);
    testPointLight(world);
    testTags(world);
    

    Universe::World world2(pRenderer, pVFS);
    testImportExportXML(world, world2);
}
