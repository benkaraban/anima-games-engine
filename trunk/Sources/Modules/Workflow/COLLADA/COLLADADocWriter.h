/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef WORKFLOW_COLLADADOCWRITER_H_
#define WORKFLOW_COLLADADOCWRITER_H_

#include <Core/Standard.h>
#include <Assets/Model.h>
#include <Workflow/COLLADA/COLLADAMeshCvt.h>
#include <COLLADAFWIWriter.h>
#include <COLLADAFWUniqueID.h>
#include <COLLADAFWNode.h>
#include <COLLADAFWGeometry.h>
#include <COLLADAFWMesh.h>
#include <COLLADAFWVisualScene.h>
#include <COLLADAFWScene.h>
#include <COLLADAFWSkinControllerData.h>
#include <COLLADAFWSkinController.h>
#include <COLLADAFWColorOrTexture.h>

#include <COLLADASaxFWLIExtraDataCallbackHandler.h>

namespace Workflow
{
class COLLADADocWriter : public Core::Object, public COLLADAFW::IWriter, public COLLADASaxFWL::IExtraDataCallbackHandler
{
public:
    COLLADADocWriter();

    // Implémentation COLLADAFW::IWriter {

    virtual void cancel(const COLLADAFW::String & errorMessage);
    virtual void start();
    virtual void finish();
    virtual bool writeGlobalAsset(const COLLADAFW::FileInfo* asset);
    virtual bool writeScene(const COLLADAFW::Scene* scene);
    virtual bool writeVisualScene(const COLLADAFW::VisualScene* visualScene);
    virtual bool writeLibraryNodes(const COLLADAFW::LibraryNodes* libraryNodes);
    virtual bool writeGeometry(const COLLADAFW::Geometry* geometry);
    virtual bool writeMaterial(const COLLADAFW::Material* material);
    virtual bool writeEffect(const COLLADAFW::Effect* effect);
    virtual bool writeCamera(const COLLADAFW::Camera* camera);
    virtual bool writeImage(const COLLADAFW::Image* image);
    virtual bool writeLight(const COLLADAFW::Light* light);
    virtual bool writeAnimation(const COLLADAFW::Animation* animation);
    virtual bool writeAnimationList(const COLLADAFW::AnimationList* animationList);
    virtual bool writeSkinControllerData(const COLLADAFW::SkinControllerData* skinControllerData);
    virtual bool writeController(const COLLADAFW::Controller* controller);
    virtual bool writeFormulas(const COLLADAFW::Formulas* formulas);
    virtual bool writeKinematicsScene(const COLLADAFW::KinematicsScene* kinematicsScene);

    // }


    // Implémentation COLLADASaxFWL::IExtraDataCallbackHandler {

    virtual bool elementBegin(const COLLADASaxFWL::ParserChar * elementName, const GeneratedSaxParser::xmlChar ** attributes);
    virtual bool elementEnd(const COLLADASaxFWL::ParserChar * elementName);
    virtual bool textData(const COLLADASaxFWL::ParserChar * text, size_t textLength);
    virtual bool parseElement (const COLLADASaxFWL::ParserChar * profileName, const COLLADASaxFWL::StringHash & elementHash, const COLLADAFW::UniqueId& uniqueId);

    // }


    Ptr<Assets::Model> getModel() const { return _pModel; };
    const String & getWarnings() const { return _warnings; };


protected:
    void trace(const String & text);
    void addNode(const COLLADAFW::Node * pNode, int32 parentID);
    void setMeshMaterial(int32 meshId, const COLLADAFW::MaterialBindingArray & mat);

    int32   _traceDepth;
    bool    _traceOn;

    bool                 _swapYZ;
    float                _scale;     // Facteur pour obtenir des mètres

    String               _warnings;

    Ptr<Assets::Model>   _pModel;
    Assets::Animation    _anim;
    int32                _animStartIndex;

    struct MaterialInfos
    {
        Assets::Material    material;
    };

    struct Property
    {
        Property(const String & name, const String & value) : name(name), value(value)
        {}

        String  name;
        String  value;
    };

    struct AnimationSplit
    {
        String      name;
        int32       start;
        int32       end;
    };

    bool                                       _isParsingProperty;
    Core::List<Property>                       _properties;

    Core::Map<COLLADAFW::UniqueId, int32>      _uidToMeshId;
    Core::Map<COLLADAFW::UniqueId, int32>      _uidToNodeId;
    Core::Map<COLLADAFW::UniqueId, int32>      _uidToBoneId;

    Core::Map<COLLADAFW::UniqueId, Ptr<MeshData> >          _uidToMeshData;
    Core::Map<COLLADAFW::UniqueId, Ptr<SkinData> >          _uidToSkinData;
    Core::Map<COLLADAFW::UniqueId, Ptr<SkeletonData> >      _uidToSkeletonData;
    Core::Map<COLLADAFW::UniqueId, Ptr<AnimationData> >     _uidToAnimationData;
    Core::Map<COLLADAFW::UniqueId, int32 >                  _uidAnimToNodeId;
    Core::Map<COLLADAFW::UniqueId, String >                 _uidImageToSourceFileName;
    Core::Map<COLLADAFW::UniqueId, COLLADAFW::UniqueId >    _uidMaterialToEffectId;
    Core::Map<COLLADAFW::UniqueId, Ptr<MaterialInfos> >     _uidEffectToMaterial;

    Core::Map<int32, Ptr<MaterialInfos> >                   _meshIdToMaterial;

    Core::List<String>      _nodesStack;

    void warning(const String & w);
    String getPath() const;
};
}
#endif /*WORKFLOW_MODELIMPORTERX_H_*/

