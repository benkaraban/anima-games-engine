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
#ifndef UNIVERSE_WORLD_H_
#define UNIVERSE_WORLD_H_

#include <Core/XML/XMLDocument.h>

#include <Renderer/IRenderer.h>
#include <Audio/IDevice.h>

#include <Universe/Node.h>
#include <Universe/NodeCamera.h>
#include <Universe/NodeDecal.h>
#include <Universe/NodeEmitter.h>
#include <Universe/NodeMesh.h>
#include <Universe/NodeGroup.h>
#include <Universe/NodeSkeleton.h>
#include <Universe/NodeWaterPatch.h>
#include <Universe/NodePointLight.h>
#include <Universe/NodeListener.h>
#include <Universe/NodeSoundSource.h>
#include <Universe/NodeTerrain.h>
#include <Universe/NodeSpline.h>
#include <Universe/NodeTrail.h>
#include <Universe/NodeStorm.h>
#include <Universe/RessourcePool.h>
#include <Universe/ScriptContext.h>
#include <Universe/ScriptOutput.h>
#include <Universe/Tasks/ITask.h>

#include <Core/Math/GeometryComputations.h>

struct lua_State;

namespace Universe
{
/**
 * La classe World est la classe de base du module. On commence par créer un World
 * et à le peupler d'objets. A chaque fois qu'on charge un niveau, on
 * crée un monde qu'on détruit une fois le niveau inutile.
 */
class LM_API_UNI World : public Core::Object, public INodeCreator
{
public:
    /**
     * A sa création, le World crée une scène associée avec le Renderer
     */
    World(const Ptr<Renderer::IRenderer> & pRenderer,
          const Ptr<Core::VirtualFileSystem> & pVFS,
          const Ptr<RessourcePool> & pMasterPool = Ptr<RessourcePool>(null),
          const Ptr<Audio::IDevice> & pAudioDevice = Ptr<Audio::IDevice>(null));

    /**
     * Crée un world à partir d'un fichier XML
     */
    World(const Ptr<Renderer::IRenderer> & pRenderer,
          const Ptr<Core::VirtualFileSystem> & pVFS,
          const Core::XMLDocument & doc,
          const Ptr<RessourcePool> & pMasterPool = Ptr<RessourcePool>(null),
          const Ptr<Audio::IDevice> & pAudioDevice = Ptr<Audio::IDevice>(null));

    virtual ~World();

    const Ptr<Renderer::IRenderer> & getRenderer() const { return _pRenderer; };
    const Ptr<RessourcePool> & getRessourcesPool() const { return _pRessourcesPool; };
    const Ptr<ScriptOutput> & getScriptOutput() const { return _pScriptOutput; };

    void setScriptOutput(const Ptr<ScriptOutput> & pScriptOutput);

    virtual Ptr<Renderer::IAABox> createBox();

    /**
     * Ajoute une tâche au monde. La tâche sera supprimée une fois qu'elle retournera true
     * à sa méthode isFinished()
     */
    void registerTask(const Ptr<ITask> & pTask);

    // createXXX pour tous les types de noeuds (mesh, émetteur de particules...).
    virtual Ptr<Node> createNode();

    virtual Ptr<NodeGroup> createGroup();

    Ptr<NodeMesh> createMesh();
    Ptr<NodeMesh> createMesh(const String & modelName, const String & meshName);

    Ptr<NodeSkeleton> createSkeleton();
    Ptr<NodeSkeleton> createSkeleton(const String & modelName);

    Ptr<NodeTerrain> createTerrain();
    Ptr<NodeTerrain> createTerrain(const String & terrainName);

    /**
     * Crée un squelette et toutes les skins présentes dans le modèle.
     * A chaque Assets::ModelNode correspont un Universe::Node qui a comme tag le nom de l'Assets::ModelNode.
     * Ces Node suivent les animation du squelette.
     */
    Ptr<NodeSkeleton> createModel(const String & modelName);

    Ptr<NodeDecal> createDecal();
    Ptr<NodeDecal> createDecal(const String & textureName, Renderer::EDecalMode mode);

    Ptr<NodeEmitter> createBillboardEmitter();
    Ptr<NodeEmitter> createBillboardEmitter(const String & textureName, Renderer::EBillboardMode mode);

    Ptr<NodeEmitter> createGroupEmitter();
    Ptr<NodeEmitter> createGroupEmitter(const String & groupName);

    Ptr<NodeSpline> createSpline();

    Ptr<NodeTrail> createTrail();
    Ptr<NodeTrail> createTrail(const String & textureName, Renderer::EFreeFormMode mode);

    Ptr<NodeStorm> createStorm();

    /**
     * Crée une instance à partir d'un fichier du VFS.
     * - Si ce fichier est un mesh avec un squelette, l'instance du NodeInstance est un NodeSqueleton (cf. createSkinnedModel)
     * - Si ce fichier est un mesh avec sans squelette, l'instance du NodeInstance est un Node racine de la hierarchie
     *   du modèle.
     * - Sinon, l'instance du NodeInstance est la racine du chargement d'un import XML du fichier (cf. importXML).
     *   Utilisé pour tout ce qui est émetteur de particules ou élément composé (modèles + particules ou autre).
     */
    Ptr<Node> createInstance(const String & resourceName);

    Ptr<NodeCameraPO> createPOCamera();
    Ptr<NodeOrthogonalCamera> createOrthogonal(float left, float right, float down, float top, float near, float far);
    Ptr<NodePerspectiveCamera> createPerspectiveCamera(float fov, float ratio,
                                                       float near, float far);

    Ptr<NodePerspectiveCamera> createPerspectiveCamera();

    Ptr<NodeWaterPatch> createWaterPatch(float width, float height);
    float getWaterLevel() const;
    void setWaterLevel(float level);

    void getMaterial(Assets::Material & mat, const String & model, const String & mesh = String::EMPTY);

    Ptr<NodePointLight> createPointLight();

    Ptr<NodeListener> getNodeListener();
    Ptr<NodeSoundSource> createSoundSource();

    const Core::List<Ptr<Node> > & getAllNodes() const { return _nodes; };
    void getAllNodesRoots(Core::List<Ptr<Node> > & nodes);

    void setSkyTexture(const String & textureName);
    void setSkyColor(const Core::Vector4f & color);
    void setSkyGlow(const Core::Vector4f & color);
    void setSkyHorizon(float horizon); // -1.0 => +1.0
    void setSkyAngle(float horizon);   //  0.0 => 2 PI
    void setSkyRoof(float roof);       //  0.0 => 10.000
    void setSkySphericity(float s);    //  0.0 => 1.0
    void setSkyAtInfinity(bool flag);
    void setSkyBackColor(const Core::Vector4f & color);
    void setSkySunColor(const Core::Vector4f & color);
    void setSkySunIntensity(float intensity);
    void setSkyModel(Renderer::ELightingModel model);
    void setSkyProcedural(bool isProcedural);

    const String & getSkyTexture() const;
    const Core::Vector4f & getSkyColor() const;
    const Core::Vector4f & getSkyGlow() const;
    float getSkyHorizon() const;
    float getSkyAngle() const;
    float getSkyRoof() const;
    float getSkySphericity() const;
    bool getSkyAtInfinity() const;
    const Core::Vector4f & getSkyBackColor() const;
    const Core::Vector4f & getSkySunColor() const;
    float getSkySunIntensity() const;
    Renderer::ELightingModel getSkyModel() const;
    bool isSkyProcedural() const;

    void setFogColor(const Core::Vector4f & color);
    void setFogRange(float start, float end);
    void setFogSkyOpacity(float opacity);
    const Core::Vector4f getFogColor() const;
    void getFogRange(float & start, float & end) const;
    float getFogSkyOpacity() const;

    const Renderer::MainLightSettings & getMainLightSettings() const;
    void setMainLightSettings(const Renderer::MainLightSettings & settings);

    void getNodesWithTag(const String & tag, Core::List<Ptr<Node> > & nodes);
    Ptr<Node> getFirstNodeWithTag(const String & tag);

    bool rayIntersection(const Core::Rayf& ray, Ptr<Node>& nodeIntersecting, float& distance);
    bool isRayOccluded(const Core::Rayf& ray);

    void  setTimeMultiplier(float timeMul);
    float getTimeMultiplier() const;

    Ptr<ITask> runScript(const String  & scriptFileName, const ScriptContext & context = ScriptContext::DEFAULT);
    Ptr<ITask> runInline(const String  & source, const ScriptContext & context = ScriptContext::DEFAULT);
    Ptr<ITask> runInline(const String8 & source, const ScriptContext & context = ScriptContext::DEFAULT);

    Ptr<ITask> runNodeScript(const Ptr<Node> & pNode, bool fromScript);

    void startWorldScripts();
    void stopWorldScripts();

    void update(double elapsed);
    void render(const Ptr<Renderer::IRenderView> & pView, const Ptr<NodeCamera> & pCamera);

    static void render(const Ptr<Renderer::IRenderView> & pView, const Ptr<NodeCamera> & pCamera, const Core::List<Ptr<World> > & worlds);

    virtual Ptr<Node> importXML(const String & fileName);
    virtual Ptr<Node> importXML(const Ptr<Core::XMLNode> & pXML);
    virtual void      exportXML(const Ptr<Core::XMLNode> & pXML, const Ptr<Node> & pNode, bool exportChildren = true);

    virtual void importWorldXML(const Core::XMLDocument & doc);
    virtual void exportWorldXML(      Core::XMLDocument & doc);
    
    // EDIT

    const Ptr<Renderer::IScene> & getScene() const { return _pScene; };

    virtual void restart();

    void prepareForPacking();

protected:
    void initWorld();
    void freeWorld();
    void internalImportWorldXML(const Core::XMLDocument & doc);
    void prepareForRendering(const Ptr<Renderer::IRenderView> & pView, const Ptr<NodeCamera> & pCamera);

    Ptr<Renderer::IRenderer> _pRenderer;
    Ptr<Renderer::IScene>    _pScene;
    Ptr<RessourcePool>       _pRessourcesPool;
    Ptr<Audio::IDevice>      _pAudioDevice;
    Ptr<ScriptOutput>        _pScriptOutput;
    Core::List<Ptr<ITask> >  _tasks;
    Core::List<Ptr<Node> >   _nodes;
    Core::List<Ptr<ITask> >  _worldScripts;
    String                   _skyTexture;
    float                    _timeMultiplier;
    lua_State *              _L;
};

LM_API_UNI void getHierarchyBox(Core::AABoxf & box, const World& world);
LM_API_UNI void copySkyParam(World& worldDst, const World& worldSrc);
LM_API_UNI void copyFogParam(World& worldDst, const World& worldSrc);
LM_API_UNI void copyEnvironnementParam(World& worldDst, const World& worldSrc);
};
#endif /* WORLD_H_ */
