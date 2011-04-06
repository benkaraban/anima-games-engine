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
#include <Core/Debug/PerfLogger.h>
#include <Core/XML/XMLMath.h>
#include <Universe/NodeMesh.h>
#include <Universe/World.h>
#include <Universe/Particles/EmitterShapeCone.h>
#include <Universe/Particles/PartRepresentationBillboard.h>
#include <Universe/Particles/PartRepresentationGroup.h>
#include <Universe/Tools.h>
#include <Universe/Tasks/Script.h>
#include <Universe/Lua/LuaWorld.h>
#include <Universe/Lua/LuaNode.h>

namespace Universe
{
const String TAG_NODE_LISTENER = L"TAG_NODE_SOUND_LISTENER";
//-----------------------------------------------------------------------------
World::World(const Ptr<Renderer::IRenderer> & pRenderer,
             const Ptr<Core::VirtualFileSystem> & pVFS,
             const Ptr<RessourcePool> & pMasterPool,
             const Ptr<Audio::IDevice> & pAudioDevice) :
    _pRenderer(pRenderer),
    _pRessourcesPool(new RessourcePool(pVFS, pRenderer)),
    _pAudioDevice(pAudioDevice),
    _pScriptOutput(new ScriptOutput()),
    _timeMultiplier(1.0)
{
    _pRessourcesPool->setMasterPool(pMasterPool);
    initWorld();
}
//-----------------------------------------------------------------------------
World::World(const Ptr<Renderer::IRenderer> & pRenderer,
             const Ptr<Core::VirtualFileSystem> & pVFS,
             const Core::XMLDocument & doc,
             const Ptr<RessourcePool> & pMasterPool,
             const Ptr<Audio::IDevice> & pAudioDevice) :
    _pRenderer(pRenderer),
    _pRessourcesPool(new RessourcePool(pVFS, pRenderer)),
    _pAudioDevice(pAudioDevice),
    _pScriptOutput(new ScriptOutput()),
    _timeMultiplier(1.0)
{
    _pRessourcesPool->setMasterPool(pMasterPool);
    initWorld();
    internalImportWorldXML(doc);
}
//-----------------------------------------------------------------------------
World::~World()
{
    freeWorld();

    _pRessourcesPool = null;

    _pScene = null;
    _pRenderer = null;
}
//-----------------------------------------------------------------------------
void World::initWorld()
{
    _pScene = _pRenderer->createScene();
    _timeMultiplier = 1.0;
    _skyTexture = L"";

    setFogColor(Core::Vector4f(0.2f, 0.2f, 0.25f, 0.0f));
    
    //creation du node sound listener
    if(_pAudioDevice != null)
    {
        Ptr<Audio::IListener>       pListener     = _pAudioDevice->createListener();
        Ptr<Universe::NodeListener> pNodeListener = Ptr<NodeListener>(new NodeListener(_pScene->createAABox(), pListener, this));
        pNodeListener->addTag(TAG_NODE_LISTENER);
        _nodes.push_back(pNodeListener);
    }


    // Création state Lua
    _L = LuaWorld::newState(this);
    luat_setoutput(_L, _pScriptOutput.get());
}
//-----------------------------------------------------------------------------
void World::freeWorld()
{
    _worldScripts.clear();

    for(int32 ii=0; ii < _tasks.size(); ii++)
        _tasks[ii]->kill();

    for(int32 ii=0; ii < _nodes.size(); ii++)
        _nodes[ii]->kill();

    _tasks.clear();
    _nodes.clear();

    lua_close(_L);
}
//-----------------------------------------------------------------------------
void World::startWorldScripts()
{
    if(!_worldScripts.empty())
        stopWorldScripts();

    for(int32 ii=0; ii < _nodes.size(); ii++)
    {
        const String8 & source = _nodes[ii]->getNodeScript();

        if(!source.empty())
        {
            ScriptContext context;
            context.fileName = L"<node>";
            context.pNode    = _nodes[ii];
            runInline(source, context);
        }
    }
}
//-----------------------------------------------------------------------------
void World::stopWorldScripts()
{
    for(int32 ii=0; ii < _worldScripts.size(); ii++)
        _worldScripts[ii]->kill();

    _worldScripts.clear();
}
//-----------------------------------------------------------------------------
Ptr<ITask> World::runNodeScript(const Ptr<Node> & pNode, bool fromScript)
{
    ScriptContext context;
    context.fileName = L"<node>";
    context.pNode    = pNode;
    context.fromScript = fromScript;
    return runInline(pNode->getNodeScript(), context);
}
//-----------------------------------------------------------------------------
void World::setScriptOutput(const Ptr<ScriptOutput> & pScriptOutput)
{ 
    _pScriptOutput = pScriptOutput; 
    luat_setoutput(_L, _pScriptOutput.get());
}
//-----------------------------------------------------------------------------
Ptr<ITask> World::runScript(const String & scriptFileName, const ScriptContext & context)
{
    ScriptContext context2(context);
    context2.fileName = Core::System::getFileName(scriptFileName);
    return runInline(_pRessourcesPool->getScript(scriptFileName), context2);
}
//-----------------------------------------------------------------------------
Ptr<ITask> World::runInline(const String & source, const ScriptContext & context)
{
    return runInline(String8(source), context);
}
//-----------------------------------------------------------------------------
Ptr<ITask> World::runInline(const String8 & source, const ScriptContext & context)
{
    Ptr<TaskScript> pTask(new TaskScript(context, _L, source, _pScriptOutput));

    if(context.fromScript)
        _tasks.push_back(pTask);
    else
        registerTask(pTask);

    return pTask;
}
//-----------------------------------------------------------------------------
void World::registerTask(const Ptr<ITask> & pTask)
{
    _tasks.push_back(pTask);
    pTask->update(0.0);
}
//-----------------------------------------------------------------------------
Ptr<Node> World::createNode()
{
    Ptr<Node> pNode(new Node(_pScene->createAABox(), this));
    _nodes.push_back(pNode);
    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<NodeGroup> World::createGroup()
{
    Ptr<NodeGroup> pNode(new NodeGroup(_pScene->createAABox(), this));
    _nodes.push_back(pNode);
    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<NodeCameraPO> World::createPOCamera()
{
    const float near = 1.f;
    const float far = 1000.f;

    Ptr<Renderer::IPerspectiveCamera> pPerspectiveCamera(_pScene->createPerspectiveCamera(f_PI_DIV_2, 1.0, near, far));
    Ptr<Renderer::IOrthoCamera>       pOrthogonalCamera(_pScene->createOrthoCamera(-10, 10, -10, 10, near, far));

    Ptr<NodeCameraPO> pNode(new NodeCameraPO(_pScene->createAABox(), pPerspectiveCamera, pOrthogonalCamera, this));

    _nodes.push_back(pNode);
    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<Renderer::IAABox> World::createBox()
{
    return _pScene->createAABox();
}
//-----------------------------------------------------------------------------
Ptr<NodeOrthogonalCamera> World::createOrthogonal(float left, float right, float down, float top, float near, float far)
{
    Ptr<Renderer::IOrthoCamera> pOrthogonalCamera(_pScene->createOrthoCamera(left, right, down, top, near, far));

    Ptr<NodeOrthogonalCamera> pNode(new NodeOrthogonalCamera(_pScene->createAABox(), pOrthogonalCamera, this));

    _nodes.push_back(pNode);
    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<NodePerspectiveCamera> World::createPerspectiveCamera(float fov, float ratio, float near, float far)
{
    Ptr<Renderer::IPerspectiveCamera> pPerspectiveCamera(_pScene->createPerspectiveCamera(fov, 1.0, near, far));

    Ptr<NodePerspectiveCamera> pNode(new NodePerspectiveCamera(_pScene->createAABox(), pPerspectiveCamera, this));

    _nodes.push_back(pNode);
    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<NodePerspectiveCamera> World::createPerspectiveCamera()
{
    Ptr<Renderer::IPerspectiveCamera> pCamera(_pScene->createPerspectiveCamera(f_PI_DIV_4, 1.0, 1.0, 1000.0));
    Ptr<NodePerspectiveCamera> pNode(new NodePerspectiveCamera(_pScene->createAABox(), pCamera, this));
    _nodes.push_back(pNode);
    return pNode;
}
//-----------------------------------------------------------------------------
void World::update(double elapsed)
{
    LM_START_PERF_LOG(L"update");

    elapsed *= _timeMultiplier;

    // Update / Nettoyage NODES

    int32 ii = 0;

    while(ii < _nodes.size())
    {
        Node & node = *_nodes[ii];

        if(node.isAlive())
        {
            node.update(elapsed * node.getTimeMultiplier());
            ii++;
        }
        else
            _nodes.qerase(ii);
    }

    // Update / Nettoyage TASKS

    std::sort(_tasks.begin(), _tasks.end(), ITaskPtrSorter());

    for(int32 iTask=0; iTask < _tasks.size(); iTask++)
        if(!_tasks[iTask]->isFinished())
            _tasks[iTask]->update(elapsed);

    ii = 0;

    while(ii < _tasks.size())
    {
        if(_tasks[ii]->isFinished())
            _tasks.qerase(ii);
        else
            ii++;
    }

    _pScene->update(elapsed);

    LM_END_PERF_LOG(L"update");
}
//-----------------------------------------------------------------------------
Ptr<NodeMesh> World::createMesh()
{
    Ptr<Renderer::IMesh> pMesh(_pRessourcesPool->getDefaultMesh());
    Ptr<Renderer::IMeshInstance> pMeshInst(_pScene->createMeshInstance(pMesh));

    Ptr<NodeMesh> pNode(new NodeMesh(_pScene->createAABox(), pMeshInst, _pRessourcesPool, this));
    _nodes.push_back(pNode);

    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<NodeMesh> World::createMesh(const String & modelName, const String & meshName)
{
    Ptr<NodeMesh> pNode(createMesh());
    pNode->setMesh(modelName, meshName);

    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<NodeSkeleton> World::createSkeleton()
{
    Ptr<NodeSkeleton> pNode(new NodeSkeleton(_pScene->createAABox(), _pScene, _pRessourcesPool, this));
    _nodes.push_back(pNode);

    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<NodeTerrain> World::createTerrain()
{
    Ptr<NodeTerrain> pNode(new NodeTerrain(_pScene->createAABox(), _pScene, _pRessourcesPool, this));
    _nodes.push_back(pNode);

    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<NodeTerrain> World::createTerrain(const String & terrainName)
{
    Ptr<NodeTerrain> pNode(createTerrain());
    pNode->setTerrain(terrainName);

    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<Node> World::createInstance(const String & resourceName)
{
    Ptr<Node> pInstance;
    String name(resourceName);
    name.toLower();

    if(name.endsWith(L".mdl"))
    {
        pInstance = createModel(resourceName);
    }
    else
    {
        pInstance = importXML(resourceName);
    }

    pInstance->setInstance(resourceName);

    return pInstance;
}
//-----------------------------------------------------------------------------
Ptr<NodeSkeleton> World::createModel(const String & modelName)
{
    Ptr<Assets::Model> pModel = _pRessourcesPool->getModel(modelName);
    Ptr<NodeSkeleton> pSkeleton(createSkeleton(modelName));

    Assets::ModelPose pose;
    pModel->getDefaultPose(pose);

    for(int32 iNode=0; iNode < pModel->getNodeCount(); iNode++)
    {
        const Assets::ModelNode & node = pModel->getNode(iNode);

        if(node.getMeshId() != Assets::UNDEFINED_MESH_ID)
        {
            const Assets::ModelMesh & mesh = pModel->getMesh(node.getMeshId());

            if(mesh.isSkin())
            {
                pSkeleton->createSkin(modelName, mesh.getName());
            }
            else
            {
                Ptr<NodeMesh> pMeshNode = createMesh(modelName, mesh.getName());

                if(pModel->getAnimationCount() >= 1)
                {
                    pSkeleton->addModelNode(iNode, pMeshNode);
                }
                else
                {
                    pMeshNode->setLocalMatrix(pose.getNodeMatrix(iNode));
                    pSkeleton->addChild(pMeshNode);
                }
            }
        }
    }

    pSkeleton->setDefaultPose();

    return pSkeleton;
}
//-----------------------------------------------------------------------------
void World::getMaterial(Assets::Material & mat, const String & model, const String & mesh)
{
    Ptr<Assets::Model> pModel = _pRessourcesPool->getModel(model);

    if(pModel->getMeshCount() > 0)
    {
        int32 iMesh = (mesh != String::EMPTY) ? pModel->getMeshId(mesh) : 0;
        mat = pModel->getMesh(iMesh).getMaterial();
    }
}
//-----------------------------------------------------------------------------
Ptr<NodeSkeleton> World::createSkeleton(const String & modelName)
{
    Ptr<NodeSkeleton> pNode(createSkeleton());
    pNode->setModel(modelName);

    return pNode;
}
//-----------------------------------------------------------------------------
Ptr<NodeSpline> World::createSpline()
{
    Ptr<Renderer::ISpline> pRenderSpline(_pScene->createSpline());
    Ptr<NodeSpline> pSpline(new NodeSpline(_pScene->createAABox(), pRenderSpline, _pRessourcesPool, this));
    _nodes.push_back(pSpline);

    return pSpline;
}
//-----------------------------------------------------------------------------
Ptr<NodeDecal> World::createDecal()
{
    Ptr<Renderer::ITextureMap> pTexture(_pRessourcesPool->getDefaultTexture());
    Ptr<Renderer::IDecal> pRenderDecal(_pScene->createDecal(pTexture, Renderer::DECAL_ADD));
    Ptr<NodeDecal> pDecal(new NodeDecal(_pScene->createAABox(), pRenderDecal, _pRessourcesPool, this));
    _nodes.push_back(pDecal);

    return pDecal;
}
//-----------------------------------------------------------------------------
Ptr<NodeDecal> World::createDecal(const String & textureName, Renderer::EDecalMode mode)
{
    Ptr<NodeDecal> pDecal(createDecal());
    pDecal->setTexture(textureName);
    pDecal->setMode(mode);

    return pDecal;
}
//-----------------------------------------------------------------------------
Ptr<NodeWaterPatch> World::createWaterPatch(float width, float height)
{
    Ptr<Renderer::IWaterPatch> pRenderWaterPatch(_pScene->createWaterPatch(width, height));
    Ptr<NodeWaterPatch> pWaterPatch(new NodeWaterPatch( _pScene->createAABox(),
                                                        pRenderWaterPatch,
                                                        _pRessourcesPool, this));
    _nodes.push_back(pWaterPatch);
    return pWaterPatch;
}
//-----------------------------------------------------------------------------
float World::getWaterLevel() const
{
    return _pScene->getWaterLevel();
}
//-----------------------------------------------------------------------------
void World::setWaterLevel(float level)
{
    _pScene->setWaterLevel(level);
}
//-----------------------------------------------------------------------------
Ptr<NodePointLight> World::createPointLight()
{
    Ptr<Renderer::IPointLight> pRenderPointLight(_pScene->createPointLight());
    Ptr<NodePointLight> pPointLight(new NodePointLight(_pScene->createAABox(), pRenderPointLight, this));
    _nodes.push_back(pPointLight);
    return pPointLight;
}
//-----------------------------------------------------------------------------
Ptr<NodeListener> World::getNodeListener()
{
    Ptr<NodeListener> pNodeListener= null;
    
    Ptr<Node> node = getFirstNodeWithTag(TAG_NODE_LISTENER);
    if (node->getNodeType() == NODE_LISTENER)
    {
        pNodeListener = LM_DEBUG_PTR_CAST<Universe::NodeListener>(node);
    }

    return pNodeListener;
}
//-----------------------------------------------------------------------------
Ptr<NodeSoundSource> World::createSoundSource()
{
    Ptr<NodeSoundSource> pNodeSoundSource = null;
    if(_pAudioDevice != null)
    {
        Ptr<Audio::ISoundSource> pSoundSource = _pAudioDevice->createSoundSource();
        if(pSoundSource != null)
        {
            pNodeSoundSource = Ptr<NodeSoundSource>(new NodeSoundSource(_pScene->createAABox(), _pRessourcesPool, pSoundSource, this));
            _nodes.push_back(pNodeSoundSource);
        }
    }
    return pNodeSoundSource;
}
//-----------------------------------------------------------------------------
Ptr<NodeEmitter> World::createBillboardEmitter()
{
    Ptr<Renderer::ITextureMap> pTexture(_pRessourcesPool->getDefaultTexture());
    Ptr<Renderer::IBillboardGroup> pBill(_pScene->createBillboardGroup(1000, pTexture, Renderer::BILLBOARD_ADD));
    Ptr<IPartRepresentation> pRepresentation(new PartRepresentationBillboard(_pRessourcesPool, pBill));
    Ptr<IEmitterShape> pShape(new EmitterShapeCone());
    Ptr<NodeEmitter> pEmitter(new NodeEmitter(_pScene->createAABox(), _pRessourcesPool, pRepresentation, pShape, 1000, this));
    _nodes.push_back(pEmitter);

    return pEmitter;
}
//-----------------------------------------------------------------------------
Ptr<NodeEmitter> World::createBillboardEmitter(const String & textureName, Renderer::EBillboardMode mode)
{
    Ptr<Renderer::ITextureMap> pTexture(_pRessourcesPool->getDefaultTexture());
    Ptr<Renderer::IBillboardGroup> pBill(_pScene->createBillboardGroup(1000, pTexture, Renderer::BILLBOARD_ADD));
    Ptr<PartRepresentationBillboard> pRepresentation(new PartRepresentationBillboard(_pRessourcesPool, pBill));
    Ptr<IEmitterShape> pShape(new EmitterShapeCone());
    Ptr<NodeEmitter> pEmitter(new NodeEmitter(_pScene->createAABox(), _pRessourcesPool, pRepresentation, pShape, 1000, this));
    _nodes.push_back(pEmitter);

    pRepresentation->setTexture(textureName);
    pRepresentation->setMode(mode);

    return pEmitter;
}
//-----------------------------------------------------------------------------
Ptr<NodeEmitter> World::createGroupEmitter()
{
    Ptr<Renderer::ITextureMap> pTexture(_pRessourcesPool->getDefaultTexture());
    Ptr<IPartRepresentation> pRepresentation(new PartRepresentationGroup(this, L""));
    Ptr<IEmitterShape> pShape(new EmitterShapeCone());
    Ptr<NodeEmitter> pEmitter(new NodeEmitter(_pScene->createAABox(), _pRessourcesPool, pRepresentation, pShape, 100, this));
    _nodes.push_back(pEmitter);

    return pEmitter;
}
//-----------------------------------------------------------------------------
Ptr<NodeEmitter> World::createGroupEmitter(const String & groupName)
{
    Ptr<Renderer::ITextureMap> pTexture(_pRessourcesPool->getDefaultTexture());
    Ptr<IPartRepresentation> pRepresentation(new PartRepresentationGroup(this, groupName));
    Ptr<IEmitterShape> pShape(new EmitterShapeCone());
    Ptr<NodeEmitter> pEmitter(new NodeEmitter(_pScene->createAABox(), _pRessourcesPool, pRepresentation, pShape, 100, this));
    _nodes.push_back(pEmitter);

    return pEmitter;
}
//-----------------------------------------------------------------------------
Ptr<NodeTrail> World::createTrail()
{
    Ptr<NodeTrail> pTrail(new NodeTrail(_pScene->createAABox(), _pScene->createFreeForm(null, Renderer::FREE_FORM_ADD), _pRessourcesPool, this));
    _nodes.push_back(pTrail);
    return pTrail;
}
//-----------------------------------------------------------------------------
Ptr<NodeTrail> World::createTrail(const String & textureName, Renderer::EFreeFormMode mode)
{
    Ptr<NodeTrail> pTrail(createTrail());
    pTrail->setTexture(textureName);
    pTrail->setMode(mode);
    return pTrail;
}
//-----------------------------------------------------------------------------
Ptr<NodeStorm> World::createStorm()
{
    Ptr<NodeStorm> pStorm(new NodeStorm(_pScene->createAABox(), _pRessourcesPool, _pScene, 100, this));
    _nodes.push_back(pStorm);
    return pStorm;
}
//-----------------------------------------------------------------------------
void World::render(const Ptr<Renderer::IRenderView> & pView, const Ptr<NodeCamera> & pCamera)
{
    prepareForRendering(pView, pCamera);
    _pRenderer->renderScene(pView, _pScene, pCamera->getBaseCamera());
}
//-----------------------------------------------------------------------------
void World::prepareForRendering(const Ptr<Renderer::IRenderView> & pView, const Ptr<NodeCamera> & pCamera)
{
    for(int32 ii=0; ii < _nodes.size(); ii++)
        _nodes[ii]->prepareForRendering(pCamera);
}
//-----------------------------------------------------------------------------
void World::setSkyTexture(const String & textureName)
{
    if(textureName.endsWith(L".ctex"))
    {
        Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
        settings.pSkyBox = _pRessourcesPool->getCubeTexture(textureName);
        settings.pTexture = null;
        _pScene->setSkysphereSettings(settings);
    }
    else
    {
        Ptr<Renderer::ITextureMap> pTexture(null);

        if(textureName != String::EMPTY)
            pTexture = _pRessourcesPool->getTexture(textureName);

        Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
        settings.pSkyBox = null;
        settings.pTexture = pTexture;
        _pScene->setSkysphereSettings(settings);
    }

    _skyTexture = textureName;
}
//-----------------------------------------------------------------------------
void World::setSkyColor(const Core::Vector4f & color)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.color = color;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkyModel(Renderer::ELightingModel model)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.model = model;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkyProcedural(bool isProcedural)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.isProcedural = isProcedural;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkyGlow(const Core::Vector4f & color)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.glow = color;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkyBackColor(const Core::Vector4f & color)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.skyColor = color;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkySunColor(const Core::Vector4f & color)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.sunColor = color;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkySunIntensity(float intensity)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.intensity = intensity;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkyHorizon(float horizon)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.horizon = horizon;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkyAngle(float angle)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.yaw = angle;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkyRoof(float roof)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.skyRoof = roof;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkySphericity(float s)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.sphericity = s;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
void World::setSkyAtInfinity(bool flag)
{
    Renderer::SkysphereSettings settings(_pScene->getSkysphereSettings());
    settings.isAtInfinity = flag;
    _pScene->setSkysphereSettings(settings);
}
//-----------------------------------------------------------------------------
const String & World::getSkyTexture() const
{
    return _skyTexture;
}
//-----------------------------------------------------------------------------
const Core::Vector4f & World::getSkyColor() const
{
    return _pScene->getSkysphereSettings().color;
}
//-----------------------------------------------------------------------------
const Core::Vector4f & World::getSkyGlow() const
{
    return _pScene->getSkysphereSettings().glow;
}
//-----------------------------------------------------------------------------
const Core::Vector4f & World::getSkyBackColor() const
{
    return _pScene->getSkysphereSettings().skyColor;
}
//-----------------------------------------------------------------------------
const Core::Vector4f & World::getSkySunColor() const
{
    return _pScene->getSkysphereSettings().sunColor;
}
//-----------------------------------------------------------------------------
float World::getSkySunIntensity() const
{
    return _pScene->getSkysphereSettings().intensity;
}
//-----------------------------------------------------------------------------
float World::getSkyHorizon() const
{
    return _pScene->getSkysphereSettings().horizon;
}
//-----------------------------------------------------------------------------
float World::getSkyAngle() const
{
    return _pScene->getSkysphereSettings().yaw;
}
//-----------------------------------------------------------------------------
float World::getSkyRoof() const
{
    return _pScene->getSkysphereSettings().skyRoof;
}
//-----------------------------------------------------------------------------
float World::getSkySphericity() const
{
    return _pScene->getSkysphereSettings().sphericity;
}
//-----------------------------------------------------------------------------
bool World::getSkyAtInfinity() const
{
    return _pScene->getSkysphereSettings().isAtInfinity;
}
//-----------------------------------------------------------------------------
Renderer::ELightingModel World::getSkyModel() const
{
    return _pScene->getSkysphereSettings().model;
}
//-----------------------------------------------------------------------------
bool World::isSkyProcedural() const
{
    return _pScene->getSkysphereSettings().isProcedural;
}
//-----------------------------------------------------------------------------
void World::setFogColor(const Core::Vector4f & color)
{
    Renderer::FogSettings fog(_pScene->getFogSettings());
    fog.color = color;
    _pScene->setFogSettings(fog);
}
//-----------------------------------------------------------------------------
void World::setFogRange(float start, float end)
{
    Renderer::FogSettings fog(_pScene->getFogSettings());
    fog.start = start;
    fog.end = end;
    _pScene->setFogSettings(fog);
}
//-----------------------------------------------------------------------------
void World::setFogSkyOpacity(float opacity)
{
    Renderer::FogSettings fog(_pScene->getFogSettings());
    fog.skyFogOpacity = opacity;
    _pScene->setFogSettings(fog);
}
//-----------------------------------------------------------------------------
const Core::Vector4f World::getFogColor() const
{
    return _pScene->getFogSettings().color;
}
//-----------------------------------------------------------------------------
void World::getFogRange(float & start, float & end) const
{
    start   = _pScene->getFogSettings().start;
    end     = _pScene->getFogSettings().end;
}
//-----------------------------------------------------------------------------
float World::getFogSkyOpacity() const
{
    return _pScene->getFogSettings().skyFogOpacity;
}
//-----------------------------------------------------------------------------
const Renderer::MainLightSettings & World::getMainLightSettings() const
{
    return _pScene->getLightSettings();
}
//-----------------------------------------------------------------------------
void World::setMainLightSettings(const Renderer::MainLightSettings & settings)
{
    _pScene->setLightSettings(settings);
}
//-----------------------------------------------------------------------------
void World::getNodesWithTag(const String & tag, Core::List<Ptr<Node> > & nodes)
{
    nodes.clear();
    for(Core::List<Ptr<Node> >::const_iterator ipNode = _nodes.begin(); ipNode != _nodes.end(); ++ipNode)
        if((*ipNode)->hasTag(tag))
            nodes.push_back(*ipNode);
}
//-----------------------------------------------------------------------------
Ptr<Node> World::getFirstNodeWithTag(const String & tag)
{
    Ptr<Node> pResult = null;
    int32 ii = 0;

    while(ii < _nodes.size() && pResult == null)
    {
        if(_nodes[ii]->hasTag(tag))
            pResult = _nodes[ii];
        ii++;
    }

    return pResult;
}
//-----------------------------------------------------------------------------
Ptr<Node> World::importXML(const String & fileName)
{
    Ptr<Core::XMLDocument> pDoc(_pRessourcesPool->getDocument(fileName));
    Core::List<Ptr<Core::XMLNode> > nodes;

    // TODO : à faire correctement (un seul node root père des autres)

    Ptr<Core::XMLNode> pRootNode(pDoc->firstChildNode(L"root"));

    if(pRootNode == null)
        throw Core::Exception(L"File '" + fileName + L"' doesn not contain a root node.");

    pRootNode->getNodes(L"node", nodes);

    if(nodes.size() == 0)
    {
        pRootNode->getNodes(L"group", nodes);

        if(nodes.size() == 0)
        {
            throw Core::Exception(L"File '" + fileName + L"' doesn not contain any node.");
        }
    }

    return importXML(nodes[0]);
}
//-----------------------------------------------------------------------------
Ptr<Node> World::importXML(const Ptr<Core::XMLNode> & pXML)
{
    bool instance = false;
    Ptr<Node> pNode;
    String type;

    pXML->getAttribute(L"", L"type", type);

    if(type == Universe::toString(NODE))                    pNode = createNode();
    else if(type == Universe::toString(NODE_GROUP))         pNode = createGroup();
    else if(type == Universe::toString(NODE_MESH))          pNode = createMesh();
    else if(type == Universe::toString(NODE_SKELETON))      pNode = createSkeleton();
    else if(type == Universe::toString(NODE_DECAL))         pNode = createDecal();
    else if(type == Universe::toString(NODE_CAMERA))        pNode = createPerspectiveCamera();
    else if(type == Universe::toString(NODE_WATER_PATCH))   pNode = createWaterPatch(0, 0);
    else if(type == Universe::toString(NODE_POINT_LIGHT))   pNode = createPointLight();
    else if(type == Universe::toString(NODE_TERRAIN))       pNode = createTerrain();
    else if(type == Universe::toString(NODE_SPLINE))        pNode = createSpline();
    else if(type == Universe::toString(NODE_TRAIL))         pNode = createTrail();
    else if(type == Universe::toString(NODE_STORM))         pNode = createStorm();
    else if(type == Universe::toString(NODE_SOUND))         pNode = createSoundSource();
    else if(type == Universe::toString(NODE_EMITTER))
    {
        String representation;
        pXML->getValue(L"representation.type", representation);
        if(representation == Universe::toString(PR_BILLBOARD))
            pNode = createBillboardEmitter();
        else if(representation == Universe::toString(PR_GROUP))
            pNode = createGroupEmitter();
        else
            throw Core::Exception(L"Unknown NodeEmitter representation :" + representation);
    }
    else if(type == L"NODE_INSTANCE")
    {
        String resourceName;
        pXML->getValue(L"resource-name", resourceName);
        pNode = createInstance(resourceName);
        instance = true;
    }
    else
        throw Core::Exception(L"Unknown Node ID :" + type);

    Core::List<Ptr<Core::XMLNode> > XMLChildren;
    pXML->getNodes(L"children", L"node", XMLChildren);

    for(int32 ii=0; ii < XMLChildren.size(); ii++)
    {
        Ptr<Node> pChildNode(importXML(XMLChildren[ii]));
        pNode->addChild(pChildNode);
    }

    if(instance)
        pNode->importInstanceXML(pXML);
    else
        pNode->importXML(pXML);

    return pNode;
}
//-----------------------------------------------------------------------------
void World::exportXML(const Ptr<Core::XMLNode> & pXML, const Ptr<Node> & pNode, bool exportChildren)
{
    if(pNode->isInstance())
    {
        pXML->setAttribute(L"type", L"NODE_INSTANCE");
        pNode->exportInstanceXML(pXML);
    }
    else
    {
        pXML->setAttribute(L"type", Universe::toString(pNode->getNodeType()));
        pNode->exportXML(pXML);
    }

    if(exportChildren)
    {
        pXML->setValue(L"children", L"");
        for(int32 ii=0; ii < pNode->getChildCount(); ii++)
        {
            Ptr<Node> pChild(pNode->getChild(ii));
            
            if(pChild->isExportable() && !pChild->isInstanceChild())
            {
                Ptr<Core::XMLNode> pXMLChild(new Core::XMLNode(L"node"));
                exportXML(pXMLChild, pChild);
                pXML->addNode(L"children", pXMLChild);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void World::importWorldXML(const Core::XMLDocument & doc)
{
    freeWorld();
    initWorld();
    internalImportWorldXML(doc);
    restart();
}
//-----------------------------------------------------------------------------
void World::internalImportWorldXML(const Core::XMLDocument & doc)
{
    bool success = true;

    Ptr<Core::XMLNode> pXML = doc.firstChildNode(L"world");

    if(pXML == null)
        throw Core::Exception(L"Error importing World from XML : missing world node.");

    // MAIN LIGHT ------
    Renderer::MainLightSettings mainLight;

    pXML->getValue(L"sun.angle-to-zenith", mainLight.sunAngleToZenith);
    pXML->getValue(L"sun.angle-to-north", mainLight.sunAngleToNorth);

    Core::getXMLValue(*pXML, L"sun.ambient", mainLight.ambient);
    Core::getXMLValue(*pXML, L"sun.diffuse0", mainLight.diffuse0);
    Core::getXMLValue(*pXML, L"sun.diffuse1", mainLight.diffuse1);
    Core::getXMLValue(*pXML, L"sun.diffuse2", mainLight.diffuse2);
    Core::getXMLValue(*pXML, L"sun.specular", mainLight.specular);
    Core::getXMLValue(*pXML, L"sun.mul-intensity", mainLight.mulIntensity);

    pXML->getValue(L"sun.static-shadow-bias", mainLight.staticShadowBias);
    pXML->getValue(L"sun.static-shadow-opacity", mainLight.staticShadowOpacity);
    pXML->getValue(L"sun.static-shadow-diffusion", mainLight.staticShadowDiffusion);

    pXML->getValue(L"sun.dynamic-shadow-opacity", mainLight.dynamicShadowOpacity);
    pXML->getValue(L"sun.dynamic-shadow-end", mainLight.dynamicShadowEnd);
    pXML->getValue(L"sun.dynamic-shadow-end-blend", mainLight.dynamicShadowEndBlend);
    pXML->getValue(L"sun.dynamic-shadow-middle", mainLight.dynamicShadowMiddle);
    pXML->getValue(L"sun.dynamic-shadow-middle-blend", mainLight.dynamicShadowMiddleBlend);
    
    pXML->getValue(L"sun.bloom-threshold", mainLight.bloomThreshold);
    pXML->getValue(L"sun.bloom-factor", mainLight.bloomFactor);

    pXML->getValue(L"sun.occlusion-contrast", mainLight.occlusionContrast);
    pXML->getValue(L"sun.occlusion-luminosity", mainLight.occlusionLuminosity);

    _pScene->setLightSettings(mainLight);

    // SKY --------

    String skyTexture;
    String strSkyColor;
    String strSkyModel;
    Renderer::SkysphereSettings sky;

    Core::getXMLValue(*pXML, L"sky.texture", skyTexture);
    Core::getXMLValue(*pXML, L"sky.color", sky.color);
    Core::getXMLValue(*pXML, L"sky.horizon", sky.horizon);
    Core::getXMLValue(*pXML, L"sky.glow", sky.glow);
    Core::getXMLValue(*pXML, L"sky.sky-roof", sky.skyRoof);
    Core::getXMLValue(*pXML, L"sky.sphericity", sky.sphericity);
    Core::getXMLValue(*pXML, L"sky.at-infinity", sky.isAtInfinity);
    Core::getXMLValue(*pXML, L"sky.yaw", sky.yaw);
    Core::getXMLValue(*pXML, L"sky.sky-color", sky.skyColor);
    Core::getXMLValue(*pXML, L"sky.sun-color", sky.sunColor);
    Core::getXMLValue(*pXML, L"sky.sun-intensity", sky.intensity);

    if(!pXML->getValue(L"sky.is-procedural", sky.isProcedural))
        sky.isProcedural = false;

    if(!pXML->getValue(L"sky.model", strSkyModel))
        sky.model = Renderer::LIGHTING_MODEL_CLEAR_SKY;
    else
    {
        if(strSkyModel == Renderer::toString(Renderer::LIGHTING_MODEL_CLEAR_SKY))
            sky.model = Renderer::LIGHTING_MODEL_CLEAR_SKY;
        else if(strSkyModel == Renderer::toString(Renderer::LIGHTING_MODEL_FOGGY))
            sky.model = Renderer::LIGHTING_MODEL_FOGGY;
        else if(strSkyModel == Renderer::toString(Renderer::LIGHTING_MODEL_CLOUDY))
            sky.model = Renderer::LIGHTING_MODEL_CLOUDY;
    }

    _pScene->setSkysphereSettings(sky);
    setSkyTexture(skyTexture);


    // FOG --------

    String strFogColor;
    Renderer::FogSettings fog;

    success &= pXML->getValue(L"fog.color", strFogColor);
    success &= pXML->getValue(L"fog.start", fog.start);
    success &= pXML->getValue(L"fog.end", fog.end);
    success &= pXML->getValue(L"fog.sky-fog-opacity", fog.skyFogOpacity);

    Core::fromXMLString(strFogColor, fog.color);

    _pScene->setFogSettings(fog);

    Core::List<Ptr<Core::XMLNode> > nodes;
    pXML->getNodes(L"root-nodes", L"node", nodes);


    // WATER LEVEL ---
    float waterLevel = 0.0f;
    success &= pXML->getValue(L"water-level", waterLevel);
    _pScene->setWaterLevel(waterLevel);

    // ROOT NODES ---
    for(Core::List<Ptr<Core::XMLNode> >::const_iterator ipNode = nodes.begin(); ipNode != nodes.end(); ++ipNode)
        importXML(*ipNode);

    if(!success)
        throw Core::Exception(L"Error importing World from XML.");
}
//-----------------------------------------------------------------------------
void World::exportWorldXML(Core::XMLDocument & doc)
{
    Ptr<Core::XMLNode> pXML(new Core::XMLNode(L"world"));
    doc.addNode(pXML);

    // MAIN LIGHT ------
    Renderer::MainLightSettings mainLight(_pScene->getLightSettings());

    pXML->setValue(L"sun.angle-to-zenith", mainLight.sunAngleToZenith);
    pXML->setValue(L"sun.angle-to-north", mainLight.sunAngleToNorth);
    pXML->setValue(L"sun.ambient", Core::toXMLString(mainLight.ambient));
    pXML->setValue(L"sun.diffuse0", Core::toXMLString(mainLight.diffuse0));
    pXML->setValue(L"sun.diffuse1", Core::toXMLString(mainLight.diffuse1));
    pXML->setValue(L"sun.diffuse2", Core::toXMLString(mainLight.diffuse2));
    pXML->setValue(L"sun.specular", Core::toXMLString(mainLight.specular));
    pXML->setValue(L"sun.mul-intensity", mainLight.mulIntensity);

    pXML->setValue(L"sun.static-shadow-bias", mainLight.staticShadowBias);
    pXML->setValue(L"sun.static-shadow-opacity", mainLight.staticShadowOpacity);
    pXML->setValue(L"sun.static-shadow-diffusion", mainLight.staticShadowDiffusion);

    pXML->setValue(L"sun.dynamic-shadow-opacity", mainLight.dynamicShadowOpacity);
    pXML->setValue(L"sun.dynamic-shadow-end", mainLight.dynamicShadowEnd);
    pXML->setValue(L"sun.dynamic-shadow-end-blend", mainLight.dynamicShadowEndBlend);
    pXML->setValue(L"sun.dynamic-shadow-middle", mainLight.dynamicShadowMiddle);
    pXML->setValue(L"sun.dynamic-shadow-middle-blend", mainLight.dynamicShadowMiddleBlend);
    
    pXML->setValue(L"sun.bloom-threshold", mainLight.bloomThreshold);
    pXML->setValue(L"sun.bloom-factor", mainLight.bloomFactor);

    pXML->setValue(L"sun.occlusion-contrast", mainLight.occlusionContrast);
    pXML->setValue(L"sun.occlusion-luminosity", mainLight.occlusionLuminosity);

    // SKY --------

    Renderer::SkysphereSettings sky(_pScene->getSkysphereSettings());

    pXML->setValue(L"sky.texture", _skyTexture);
    pXML->setValue(L"sky.color", Core::toXMLString(sky.color));
    pXML->setValue(L"sky.horizon", sky.horizon);
    pXML->setValue(L"sky.glow", Core::toXMLString(sky.glow));
    pXML->setValue(L"sky.sky-roof", sky.skyRoof);
    pXML->setValue(L"sky.sphericity", sky.sphericity);
    pXML->setValue(L"sky.at-infinity", sky.isAtInfinity);
    pXML->setValue(L"sky.yaw", sky.yaw);
    pXML->setValue(L"sky.is-procedural", sky.isProcedural);
    pXML->setValue(L"sky.model", Renderer::toString(sky.model));
    pXML->setValue(L"sky.sky-color", Core::toXMLString(sky.skyColor));
    pXML->setValue(L"sky.sun-color", Core::toXMLString(sky.sunColor));
    pXML->setValue(L"sky.sun-intensity", sky.intensity);

    // FOG --------

    Renderer::FogSettings fog(_pScene->getFogSettings());

    pXML->setValue(L"fog.color", Core::toXMLString(fog.color));
    pXML->setValue(L"fog.start", fog.start);
    pXML->setValue(L"fog.end", fog.end);
    pXML->setValue(L"fog.sky-fog-opacity", fog.skyFogOpacity);

    // WATER LEVEL

    pXML->setValue(L"water-level", _pScene->getWaterLevel());

    // ROOT NODES ---

    for(Core::List<Ptr<Node> >::const_iterator ipNode = _nodes.begin(); ipNode != _nodes.end(); ++ipNode)
    {
        if((*ipNode)->isRoot() && (*ipNode)->isExportable())
        {
            Ptr<Core::XMLNode> pXMLNode(new Core::XMLNode(L"node"));
            exportXML(pXMLNode, *ipNode);
            pXML->addNode(L"root-nodes", pXMLNode);
        }
    }
}
//-----------------------------------------------------------------------------
void World::setTimeMultiplier(float timeMul)
{
    _timeMultiplier = timeMul;
}
//-----------------------------------------------------------------------------
float World::getTimeMultiplier() const
{
    return _timeMultiplier;
}
//-----------------------------------------------------------------------------
void World::render(const Ptr<Renderer::IRenderView> & pView, const Ptr<NodeCamera> & pCamera, const Core::List<Ptr<World> > & worlds)
{
    for(int32 ii=0; ii < worlds.size(); ii++)
        worlds[ii]->prepareForRendering(pView, pCamera);

    Core::List<Ptr<Renderer::IScene> > scenes;

    for(int32 ii=0; ii < worlds.size(); ii++)
        scenes.push_back(worlds[ii]->_pScene);

    worlds[0]->_pRenderer->renderScenes(pView, scenes, pCamera->getBaseCamera());
}
//-----------------------------------------------------------------------------
void World::getAllNodesRoots(Core::List<Ptr<Node> > & nodes)
{
    nodes.clear();

    const Core::List<Ptr<Node> > & allNodes = getAllNodes();

    for (int32 iNode=0; iNode<allNodes.size(); ++iNode)
    {
        if (allNodes[iNode]->isRoot() == true)
            nodes.push_back(allNodes[iNode]);
    }
}
//-----------------------------------------------------------------------------
bool World::rayIntersection(const Core::Rayf& ray, Ptr<Node>& nodeIntersecting, float& distance)
{
    return nodesRayIntersection(_nodes, ray, nodeIntersecting, distance);
}
//-----------------------------------------------------------------------------
bool World::isRayOccluded(const Core::Rayf& ray)
{
    for(int32 iNode=0; iNode < _nodes.size(); iNode++)
        if(_nodes[iNode]->isRayOccluded(ray))
            return true;
    return false;
}
//-----------------------------------------------------------------------------
void World::restart() 
{
    const Core::List<Ptr<Node> > & allNodes = getAllNodes();

    for (int32 iNode=0; iNode<allNodes.size(); ++iNode)
    {
        allNodes[iNode]->restart(true);
    }
}
//-----------------------------------------------------------------------------
void World::prepareForPacking()
{
    Core::List<Ptr<Node> > rootNodes;

    for(int32 ii=0; ii < _nodes.size(); ii++)
        if(_nodes[ii]->isRoot())
            rootNodes.push_back(_nodes[ii]);

    for(int32 ii=0; ii < rootNodes.size(); ii++)
        rootNodes[ii]->prepareForPacking();

    rootNodes.clear();

    int32 ii = 0;

    while(ii < _nodes.size())
    {
        if(!_nodes[ii]->isAlive())
            _nodes.qerase(ii);
        else
            ii ++;
    }
}
//-----------------------------------------------------------------------------
LM_API_UNI void getHierarchyBox(Core::AABoxf & box, const World& world)
{
    box.clear();

    const Core::List<Ptr<Node> >& nodes = world.getAllNodes();

    for (int32 ii=0; ii<nodes.size(); ++ii)
    {
        Core::AABoxf boxNode;
        getHierarchyBox(boxNode, *(nodes[ii]));

        box.setUnion(boxNode);
    }
}
//-----------------------------------------------------------------------------
LM_API_UNI void copySkyParam(World& worldDst, const World& worldSrc)
{
    worldDst.setSkyTexture(worldSrc.getSkyTexture());
    worldDst.setSkyColor(worldSrc.getSkyColor());
    worldDst.setSkyGlow(worldSrc.getSkyGlow());
    worldDst.setSkyHorizon(worldSrc.getSkyHorizon());
    worldDst.setSkyAngle(worldSrc.getSkyAngle());
    worldDst.setSkyRoof(worldSrc.getSkyRoof()); 
    worldDst.setSkySphericity(worldSrc.getSkySphericity());
    worldDst.setSkyAtInfinity(worldSrc.getSkyAtInfinity());
    worldDst.setSkyBackColor(worldSrc.getSkyBackColor());
    worldDst.setSkySunColor(worldSrc.getSkySunColor());
    worldDst.setSkyModel(worldSrc.getSkyModel());
    worldDst.setSkyProcedural(worldSrc.isSkyProcedural());
}
//-----------------------------------------------------------------------------
LM_API_UNI void copyFogParam(World& worldDst, const World& worldSrc)
{
    worldDst.setFogColor(worldSrc.getFogColor());
    float start, end;
    worldSrc.getFogRange(start, end);
    worldDst.setFogRange(start, end);
    worldDst.setFogSkyOpacity(worldSrc.getFogSkyOpacity());
}
//-----------------------------------------------------------------------------
LM_API_UNI void copyEnvironnementParam(World& worldDst, const World& worldSrc)
{
    worldDst.setMainLightSettings(worldSrc.getMainLightSettings());
    copySkyParam(worldDst, worldSrc);
    copyFogParam(worldDst, worldSrc);
    worldDst.setWaterLevel(worldSrc.getWaterLevel());
}
//-----------------------------------------------------------------------------
}
