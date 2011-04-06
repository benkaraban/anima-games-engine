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
#include <Core/Logger.h>
#include <Core/Strings/Tools.h>
#include <Workflow/COLLADA/COLLADADocWriter.h>
#include <Workflow/COLLADA/COLLADATools.h>

#include <COLLADAFWAnimationCurve.h>
#include <COLLADAFWAnimationList.h>
#include <COLLADAFWFileInfo.h>
#include <COLLADAFWMaterial.h>
#include <COLLADAFWEffect.h>
#include <COLLADAFWImage.h>

namespace Workflow
{
//-----------------------------------------------------------------------------
static const wchar_t * IGNORE_PREFIX    = L"_";
static const wchar_t * ANIM_PREFIX      = L"anim-";
//-----------------------------------------------------------------------------
COLLADADocWriter::COLLADADocWriter() :
    _traceDepth(0),
    _traceOn(false),
    _isParsingProperty(false)
{
    _pModel = Ptr<Assets::Model>(new Assets::Model());
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::elementBegin(const COLLADASaxFWL::ParserChar * elementName, const GeneratedSaxParser::xmlChar ** attributes)
{
    String name(elementName);

    if(name == L"user_properties")
    {
        trace(L"* User property block begin");
        _isParsingProperty = true;
        _traceDepth += 2;
    }

    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::elementEnd(const COLLADASaxFWL::ParserChar * elementName)
{
    String name(elementName);

    if(name == L"user_properties")
    {
        _isParsingProperty = false;
        _traceDepth -= 2;
        trace(L"* User property block end");
    }

    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::textData(const COLLADASaxFWL::ParserChar * text, size_t textLength)
{
    if(_isParsingProperty)
    {
        String data(String8(text, textLength));
        Core::List<String> lines;

        Core::tokenize(data, lines, L"\n");

        for(int32 ii=0; ii < lines.size(); ii++)
        {
            String clean;
            Core::trimAll(lines[ii], clean, L" \n\r\t");

            Core::List<String> tokens;
            Core::tokenize(clean, tokens, L"=");

            if(tokens.size() == 2)
            {
                _properties.push_back(Property(tokens[0], tokens[1]));
                tokens[0].resize(std::max(tokens[0].size(), 25), L' ');
                trace(L" - " + tokens[0] + L" : " + tokens[1]);
            }
        }
    }
    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::parseElement (const COLLADASaxFWL::ParserChar * profileName, const COLLADASaxFWL::StringHash & elementHash, const COLLADAFW::UniqueId& uniqueId)
{
    return true;
}
//-----------------------------------------------------------------------------
void COLLADADocWriter::cancel(const COLLADAFW::String & errorMessage)
{
}
//-----------------------------------------------------------------------------
void COLLADADocWriter::start()
{
}
//-----------------------------------------------------------------------------
void COLLADADocWriter::finish()
{
    Core::List<AnimationSplit> splits;

    for(int32 iProp=0; iProp < _properties.size(); iProp++)
    {
        Property & prop = _properties[iProp];

        if(prop.name.startsWith(ANIM_PREFIX))
        {
            AnimationSplit split;
            Core::List<String> tokens;
            Core::tokenize(prop.value, tokens, L"-");

            if(tokens.size() == 2)
            {
                split.name = prop.name.slice(String(ANIM_PREFIX).size());
                split.start = Core::toInteger(tokens[0]) - _animStartIndex;
                split.end = Core::toInteger(tokens[1]) - _animStartIndex;

                splits.push_back(split);
            }
        }
    }

    if(_anim.getFrameCount() > 0)
    {
        if(splits.empty())
        {
            _pModel->addAnimation(L"anim-00", _anim);
        }
        else
        {
            for(int32 iSplit=0; iSplit < splits.size(); iSplit++)
            {
                trace(L"* Adding animation split '" + splits[iSplit].name + L"' from " + Core::toString(splits[iSplit].start) + L" to " + Core::toString(splits[iSplit].end));
                _pModel->addAnimation(splits[iSplit].name, Assets::Animation(_anim, splits[iSplit].start, splits[iSplit].end));
            }
        }
    }
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeGlobalAsset(const COLLADAFW::FileInfo* pAsset)
{
    _scale = float(pAsset->getUnit().getLinearUnitMeter());

    switch(pAsset->getUpAxisType())
    {
    case COLLADAFW::FileInfo::Z_UP:
        _swapYZ = true;
        break;
    case COLLADAFW::FileInfo::X_UP:
        WAR << L"Importing COLLADA file with X Up which is freaking weird. Result may be incorrect.\n";
        break;
    case COLLADAFW::FileInfo::NONE:
    case COLLADAFW::FileInfo::Y_UP:
    default:
        _swapYZ = false;
        break;
    }

    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeScene(const COLLADAFW::Scene* scene)
{
    return true;
}
//-----------------------------------------------------------------------------
String COLLADADocWriter::getPath() const
{
    String result;

    for(int32 ii=0; ii < _nodesStack.size() - 1; ii++)
        result << _nodesStack[ii] << L"/";

    return result;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeVisualScene(const COLLADAFW::VisualScene* pVisualScene)
{
    const COLLADAFW::NodePointerArray & rootNodes = pVisualScene->getRootNodes();

    for(int32 ii=0; ii < int32(rootNodes.getCount()); ii++)
        addNode(rootNodes[ii], Assets::ROOT_NODE_ID);

    // Skeleton

    LM_ASSERT(_uidToSkeletonData.size() == _uidToSkinData.size());

    for(Core::Map<COLLADAFW::UniqueId, Ptr<SkeletonData> >::const_iterator iSkeleton = _uidToSkeletonData.begin(); iSkeleton != _uidToSkeletonData.end(); ++iSkeleton)
    {
        const SkeletonData & skelData = *iSkeleton->second;
        Core::Map<COLLADAFW::UniqueId, Ptr<SkinData> >::const_iterator iSkinData = _uidToSkinData.find(skelData.dataUID);

        if(iSkinData == _uidToSkinData.end())
            throw Core::Exception(L"COLLADADocWriter error : unknown skin data UID");

        const SkinData & skinData = *iSkinData->second;
        Assets::Skeleton & skeleton = _pModel->getSkeleton();

        skelData.pMesh->setBindShapeMatrix(skinData.bindShapeMatrix);

        if(skinData.offsetMatrices.size() != skelData.bonesUID.size())
            throw Core::Exception(L"Some bones required for skinning cannot be found. All used bones must be visible when exporting to COLLADA.");

        LM_ASSERT(skinData.offsetMatrices.size() == skelData.bonesUID.size());

        for(int32 ii=0; ii < skelData.bonesUID.size(); ii++)
        {
            if(_uidToBoneId.find(skelData.bonesUID[ii]) == _uidToBoneId.end())
            {
                Core::Map<COLLADAFW::UniqueId, int32>::const_iterator iNode = _uidToNodeId.find(skelData.bonesUID[ii]);

                if(iNode == _uidToNodeId.end())
                    throw Core::Exception(L"COLLADADocWriter error : unknown bone node UID");

                _uidToBoneId[skelData.bonesUID[ii]] = skeleton.addBone(iNode->second, skinData.offsetMatrices[ii]);
            }
        }

        // Références locales bones -> références globales bones
        Core::List<Assets::Vertex> & vert = skelData.pMesh->getLODByID(0).getVertices();

        for(int32 iVert=0; iVert < vert.size(); iVert++)
        {
            for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
            {
                int32 inBone = vert[iVert].boneId[iBone];
                Core::Map<COLLADAFW::UniqueId, int32>::const_iterator iBoneId = _uidToBoneId.find(skelData.bonesUID[inBone]);

                if(iBoneId == _uidToBoneId.end())
                    throw Core::Exception(L"COLLADADocWriter error : unknown global bone node UID");

                vert[iVert].boneId[iBone] = iBoneId->second;
            }
        }
    }


    // Animation

    _anim = Assets::Animation(_pModel->getNodeCount());

    return true;
}
//-----------------------------------------------------------------------------
void COLLADADocWriter::addNode(const COLLADAFW::Node * pNode, int32 parentID)
{
    String nodeName(pNode->getName().c_str());
    String nodePath = getPath() + nodeName;

    trace(L"* Node '" + nodeName + L"' ID " + String(pNode->getUniqueId().toAscii().c_str()));

    _nodesStack.push_back(nodeName);
    _traceDepth += 2;

    switch(pNode->getType())
    {
    case COLLADAFW::Node::JOINT: trace(L"- Type JOINT "); break;
    case COLLADAFW::Node::NODE: break;
    }

    for(int32 ii=0; ii < int32(pNode->getInstanceGeometries().getCount()); ii++)
    {
        const COLLADAFW::InstanceGeometry * pInstGeo = pNode->getInstanceGeometries()[ii];
        trace(L"- Instance geometry inst of " + String(pInstGeo->getInstanciatedObjectId().toAscii().c_str()));

        const COLLADAFW::MaterialBindingArray & matBinding = pInstGeo->getMaterialBindings();

        _traceDepth += 2;
        for(int32 iMat=0; iMat < int32(matBinding.getCount()); iMat++)
        {
            const COLLADAFW::MaterialBinding & mat = matBinding[iMat];
           trace(L"- Material " + String(String8(mat.getName().c_str())) + L" from referenced material " + String(mat.getReferencedMaterial().toAscii().c_str()));
        }
        _traceDepth -= 2;
    }
    for(int32 ii=0; ii < int32(pNode->getInstanceNodes().getCount()); ii++)
    {
        const COLLADAFW::InstanceNode * pInstNode = pNode->getInstanceNodes()[ii];
        trace(L"- Instance node inst of " + String(pInstNode->getInstanciatedObjectId().toAscii().c_str()));
    }
    for(int32 ii=0; ii < int32(pNode->getInstanceCameras().getCount()); ii++)
    {
        const COLLADAFW::InstanceCamera * pInstCamera = pNode->getInstanceCameras()[ii];
        trace(L"- Instance camera inst of " + String(pInstCamera->getInstanciatedObjectId().toAscii().c_str()));
    }
    for(int32 ii=0; ii < int32(pNode->getInstanceLights().getCount()); ii++)
    {
        const COLLADAFW::InstanceLight * pInstLight = pNode->getInstanceLights()[ii];
        trace(L"- Instance light inst of " + String(pInstLight->getInstanciatedObjectId().toAscii().c_str()));
    }
    for(int32 ii=0; ii < int32(pNode->getInstanceControllers().getCount()); ii++)
    {
        const COLLADAFW::InstanceController * pInstCtrl = pNode->getInstanceControllers()[ii];
        trace(L"- Instance controller inst of " + String(pInstCtrl->getInstanciatedObjectId().toAscii().c_str()));
    }

    Core::Matrix4f matrix = Workflow::cvtMat4(pNode->getTransformationMatrix(), _swapYZ, _scale);
    int32 nodeID;

    if(pNode->getInstanceControllers().getCount() > 0 && !nodeName.startsWith(IGNORE_PREFIX))
    {
        const COLLADAFW::InstanceController * pInstControl = pNode->getInstanceControllers()[0];
        COLLADAFW::UniqueId uidController = pInstControl->getInstanciatedObjectId();
        Core::Map<COLLADAFW::UniqueId, Ptr<SkeletonData> >::const_iterator iSkelDataID = _uidToSkeletonData.find(uidController);

        if(iSkelDataID == _uidToSkeletonData.end())
            throw Core::Exception(L"COLLADADocWriter : reference to unknown skin controller");

        Core::Map<COLLADAFW::UniqueId, int32>::const_iterator iMeshID = _uidToMeshId.find(iSkelDataID->second->meshUID);

        if(iMeshID == _uidToMeshId.end())
            throw Core::Exception(L"COLLADADocWriter : reference to unknown skinned geometry");

        if(!_pModel->getMesh(iMeshID->second).getName().startsWith(IGNORE_PREFIX))
        {
            setMeshMaterial(iMeshID->second, pInstControl->getMaterialBindings());
            nodeID = _pModel->addMeshNode(String(pNode->getName().c_str()), parentID, iMeshID->second, matrix);
        }
        else
        {
            nodeID = _pModel->addNode(String(pNode->getName().c_str()), parentID, matrix);
        }
    }
    else if(pNode->getInstanceGeometries().getCount() > 0 && !nodeName.startsWith(IGNORE_PREFIX))
    {
        const COLLADAFW::InstanceGeometry * pInstGeo = pNode->getInstanceGeometries()[0];
        COLLADAFW::UniqueId uidGeometry = pInstGeo->getInstanciatedObjectId();
        Core::Map<COLLADAFW::UniqueId, int32>::const_iterator iMeshID = _uidToMeshId.find(uidGeometry);

        if(iMeshID == _uidToMeshId.end())
        {
            String msg;
            msg << L"* Node '" << nodePath << L"' references unknown geometry (not an editable mesh)\n";
            warning(msg);
            nodeID = _pModel->addNode(String(pNode->getName().c_str()), parentID, matrix);
        }
        else if(!_pModel->getMesh(iMeshID->second).getName().startsWith(IGNORE_PREFIX))
        {
            setMeshMaterial(iMeshID->second, pInstGeo->getMaterialBindings());
            nodeID = _pModel->addMeshNode(String(pNode->getName().c_str()), parentID, iMeshID->second, matrix);
        }
        else
        {
            nodeID = _pModel->addNode(String(pNode->getName().c_str()), parentID, matrix);
        }
    }
    else
    {
        nodeID = _pModel->addNode(String(pNode->getName().c_str()), parentID, matrix);
    }

    _uidToNodeId[pNode->getUniqueId()] = nodeID;

    for(int32 ii=0; ii < int32(pNode->getTransformations().getCount()); ii++)
    {
        const COLLADAFW::Transformation * pTrans = pNode->getTransformations()[ii];
        COLLADAFW::UniqueId uid = pTrans->getAnimationList();

        if(uid != COLLADAFW::UniqueId::INVALID)
        {
            LM_ASSERT(_uidAnimToNodeId.find(uid) == _uidAnimToNodeId.end());
            _uidAnimToNodeId[uid] = nodeID;
        }

        switch(pTrans->getTransformationType())
        {
        case COLLADAFW::Transformation::MATRIX:    trace(L"- Transformation MATRIX with anim " + String(pTrans->getAnimationList().toAscii().c_str())); break;
        case COLLADAFW::Transformation::TRANSLATE: trace(L"- Transformation TRANSLATE with anim " + String(pTrans->getAnimationList().toAscii().c_str())); break;
        case COLLADAFW::Transformation::ROTATE:    trace(L"- Transformation ROTATE with anim " + String(pTrans->getAnimationList().toAscii().c_str())); break;
        case COLLADAFW::Transformation::SCALE:     trace(L"- Transformation SCALE with anim " + String(pTrans->getAnimationList().toAscii().c_str())); break;
        case COLLADAFW::Transformation::LOOKAT:    trace(L"- Transformation LOOKAT with anim " + String(pTrans->getAnimationList().toAscii().c_str())); break;
        case COLLADAFW::Transformation::SKEW:      trace(L"- Transformation SKEW with anim " + String(pTrans->getAnimationList().toAscii().c_str())); break;
        default:                                   trace(L"- Transformation ??? with anim " + String(pTrans->getAnimationList().toAscii().c_str())); break;
        }
    }

    for(int32 ii=0; ii < int32(pNode->getChildNodes().getCount()); ii++)
        addNode(pNode->getChildNodes()[ii], nodeID);

    _nodesStack.pop_back();
    _traceDepth -= 2;
}
//-----------------------------------------------------------------------------
void COLLADADocWriter::setMeshMaterial(int32 meshId, const COLLADAFW::MaterialBindingArray & mat)
{
    if(mat.getCount() > 0)
    {
        const COLLADAFW::MaterialBinding & matBinding = mat[0];
        
        Core::Map<COLLADAFW::UniqueId, COLLADAFW::UniqueId >::const_iterator iEffect = _uidMaterialToEffectId.find(matBinding.getReferencedMaterial());

        if(iEffect == _uidMaterialToEffectId.end())
            throw Core::Exception(L"Node geometry instance references unknown effect");

        Core::Map<COLLADAFW::UniqueId, Ptr<MaterialInfos> >::const_iterator iMaterial = _uidEffectToMaterial.find(iEffect->second);

        if(iMaterial == _uidEffectToMaterial.end())
            throw Core::Exception(L"Node geometry instance references unknown material");

        Core::Map<int32, Ptr<MaterialInfos> >::const_iterator iMeshMat = _meshIdToMaterial.find(meshId);

        if(iMeshMat != _meshIdToMaterial.end() && iMeshMat->second != iMaterial->second)
            throw Core::Exception(L"Two different materials are assigned to different mesh instances");

        _meshIdToMaterial[meshId] = iMaterial->second;

        _pModel->getMesh(meshId).getMaterial() = iMaterial->second->material;
    }
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeLibraryNodes(const COLLADAFW::LibraryNodes* libraryNodes)
{
    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeGeometry(const COLLADAFW::Geometry* pGeometry)
{
    String meshType;
    bool isSupported = false;

    switch(pGeometry->getType())
    {
    case COLLADAFW::Geometry::GEO_TYPE_MESH:
    {
        trace(L"* Geometry '" + String(String8(pGeometry->getName().c_str())) + L"' type GEO_TYPE_MESH " + String(pGeometry->getUniqueId().toAscii().c_str()));

        const COLLADAFW::Mesh* pMesh = static_cast<const COLLADAFW::Mesh*>(pGeometry);
        const COLLADAFW::MeshPrimitiveArray & prims = pMesh->getMeshPrimitives();
        const COLLADAFW::MeshVertexData & vd = pMesh->getPositions();

        _traceDepth += 2;

        for(unsigned int ii=0; ii < prims.getCount(); ii++)
        {
            COLLADAFW::MeshPrimitive* pPrim = prims[ii];
            switch(pPrim->getPrimitiveType())
            {
            case COLLADAFW::MeshPrimitive::TRIANGLES:
                trace(L"- " + Core::toString(pPrim->getFaceCount()) + L" triangles with material '" + String(String8(pPrim->getMaterial().c_str())) + L"'");
                break;
            default:
                throw Core::Exception(L"Unsupported mesh primitive");
                break;
            }

            trace(L"- " + Core::toString(pPrim->getPositionIndices().getCount()) + L" vertices");           
        }

        {
            Ptr<MeshData> pMeshData = Workflow::convertMesh(pMesh, _swapYZ, _scale);

            Ptr<Assets::VerticeSet> pVert = pMeshData->pMesh->getLOD(Assets::LOD_HIGH);
            if(pVert->getIndices().size() == 0 || pVert->getVertices().size() == 0)
            {
                warning(L"Empty mesh " + String(pMesh->getName().c_str()));
                pVert->getIndices().push_back(0);
                pVert->getIndices().push_back(1);
                pVert->getIndices().push_back(2);

                Assets::Vertex v;
                v.normale   = Core::Vector3f(0.0f, 0.0f, 1.0f);
                v.texCoords = Core::Vector2f(0.0f, 0.0f);
                v.position  = Core::Vector3f(0.0f, 0.0f, 0.0f);
                pVert->getVertices().push_back(v);

                v.texCoords = Core::Vector2f(1.0f, 0.0f);
                v.position  = Core::Vector3f(1.0f, 0.0f, 0.0f);
                pVert->getVertices().push_back(v);

                v.texCoords = Core::Vector2f(0.0f, 1.0f);
                v.position  = Core::Vector3f(0.0f, 1.0f, 0.0f);
                pVert->getVertices().push_back(v);
            }

            int32 meshID = _pModel->addMesh(String(pMesh->getName().c_str()), *pMeshData->pMesh);
            _uidToMeshId[pMesh->getUniqueId()] = meshID;
            _uidToMeshData[pMesh->getUniqueId()] = pMeshData;
        }

        _traceDepth -= 2;
        break;
    }
    case COLLADAFW::Geometry::GEO_TYPE_SPLINE:
        trace(L"* Geometry type GEO_TYPE_SPLINE " + String(pGeometry->getUniqueId().toAscii().c_str()));
        throw Core::Exception(L"Unsupported mesh type (spline)");
        break;
    case COLLADAFW::Geometry::GEO_TYPE_CONVEX_MESH:
        trace(L"* Geometry type GEO_TYPE_CONVEX_MESH " + String(pGeometry->getUniqueId().toAscii().c_str()));
        throw Core::Exception(L"Unsupported mesh type (convex)");
        break;
    case COLLADAFW::Geometry::GEO_TYPE_UNKNOWN:
        trace(L"* Geometry type GEO_TYPE_UNKNOWN " + String(pGeometry->getUniqueId().toAscii().c_str()));
        throw Core::Exception(L"Unsupported mesh type (unknown)");
        break;
    default:
        trace(L"* Geometry type ??? (" + Core::toString(int32(pGeometry->getType())) + L") " + String(pGeometry->getUniqueId().toAscii().c_str()));
        throw Core::Exception(L"Unsupported mesh type (???)");
        break;
    }

    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeMaterial(const COLLADAFW::Material* pMaterial)
{
    trace(L"* Material '" + String(String8(pMaterial->getName().c_str())) + L"' with ID " + String(pMaterial->getUniqueId().toAscii().c_str()));

    _uidMaterialToEffectId[pMaterial->getUniqueId()] = pMaterial->getInstantiatedEffect();

    _traceDepth += 2;
    trace(L"- Instance of effect " + String(pMaterial->getInstantiatedEffect().toAscii().c_str()));
    _traceDepth -= 2;
    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeEffect(const COLLADAFW::Effect* pEffect)
{
    trace(L"* Effect '" + String(pEffect->getName().c_str()) + L"' with ID " + String(pEffect->getUniqueId().toAscii().c_str()));
    Ptr<MaterialInfos> pMaterialInfos(new MaterialInfos());
    Assets::Material & material = pMaterialInfos->material;

    const COLLADAFW::CommonEffectPointerArray & effects = pEffect->getCommonEffects();

    _traceDepth += 2;

    for(int32 ii=0; ii < int32(effects.getCount()); ii++)
    {
        const COLLADAFW::EffectCommon & effect = *effects[ii];

        switch(effect.getShaderType())
        {
        case COLLADAFW::EffectCommon::SHADER_BLINN:     trace(L"- Blinn"); break;
        case COLLADAFW::EffectCommon::SHADER_PHONG:     trace(L"- Phong"); break;
        case COLLADAFW::EffectCommon::SHADER_LAMBERT:   trace(L"- Lambert"); break;
        case COLLADAFW::EffectCommon::SHADER_CONSTANT:  trace(L"- Constant"); break;
        case COLLADAFW::EffectCommon::SHADER_UNKNOWN:   trace(L"- Unknown"); break;
        }

        _traceDepth += 2;
        {
            const COLLADAFW::SamplerPointerArray & samplers = effect.getSamplerPointerArray();

            const COLLADAFW::ColorOrTexture & emissive = effect.getEmission();
            const COLLADAFW::ColorOrTexture & diffuse = effect.getDiffuse();
            const COLLADAFW::ColorOrTexture & specular = effect.getSpecular();
            const COLLADAFW::ColorOrTexture & opacity = effect.getOpacity();
            const COLLADAFW::FloatOrParam & shininess = effect.getShininess();

            material.emissive = (diffuse.isColor()  ? cvtColor(emissive.getColor()) : Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
            material.diffuse  = (diffuse.isColor()  ? cvtColor(diffuse.getColor())  : Core::Vector4f::ONE);
            material.specular = (specular.isColor() ? cvtColor(specular.getColor()) : Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            material.shininess = (shininess.getType() == COLLADAFW::FloatOrParam::FLOAT) ? shininess.getFloatValue() : 22.0f;

            if(diffuse.isTexture())
            {
                COLLADAFW::UniqueId idDiffuseSource = samplers[diffuse.getTexture().getSamplerId()]->getSourceImage();
                Core::Map<COLLADAFW::UniqueId, String >::const_iterator iSource = _uidImageToSourceFileName.find(idDiffuseSource);

                if(iSource == _uidImageToSourceFileName.end())
                    throw Core::Exception(L"Effect referencing unknown source image");

                material.diffuseTexName = iSource->second;
            }

            if(specular.isTexture())
            {
                COLLADAFW::UniqueId idSpecularSource = samplers[specular.getTexture().getSamplerId()]->getSourceImage();
                Core::Map<COLLADAFW::UniqueId, String >::const_iterator iSource = _uidImageToSourceFileName.find(idSpecularSource);

                if(iSource == _uidImageToSourceFileName.end())
                    throw Core::Exception(L"Effect referencing unknown source image");

                material.specularTexName = iSource->second;
            }

            if(opacity.isTexture())
            {
                COLLADAFW::UniqueId idOpacitySource = samplers[opacity.getTexture().getSamplerId()]->getSourceImage();
                Core::Map<COLLADAFW::UniqueId, String >::const_iterator iSource = _uidImageToSourceFileName.find(idOpacitySource);

                if(iSource == _uidImageToSourceFileName.end())
                    throw Core::Exception(L"Effect referencing unknown source image");

                if(iSource->second != material.diffuseTexName)
                    throw Core::Exception(L"Using an alpha layer texture different than the diffuse layer");

                material.flags |= Assets::MAT_FLAG_TRANSLUCID_TEXTURE;
            }
            else if(opacity.isColor())
            {
                material.diffuse.a = float(opacity.getColor().getAlpha());
            }

            trace(L"- Samplers used :");
            _traceDepth += 2;

            for(int32 iSampler=0; iSampler < int32(samplers.getCount()); iSampler++)
            {
                const COLLADAFW::Sampler & sampler = *samplers[iSampler];

                trace(L"- Sampler '" + String(pEffect->getName().c_str()) + L"' with ID " + String(sampler.getUniqueId().toAscii().c_str()));
                {
                    _traceDepth += 2;
                    trace(L"- Source image ID " + String(sampler.getSourceImage().toAscii().c_str()));
                    _traceDepth -= 2;
                }
            }
            _traceDepth -= 2;
        }
        _traceDepth -= 2;
    }

    _uidEffectToMaterial[pEffect->getUniqueId()] = pMaterialInfos;

    if(effects.getCount() > 1)
        throw Core::Exception(L"Multiple common material effects are not supported");

    _traceDepth -= 2;
    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeCamera(const COLLADAFW::Camera* camera)
{
    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeImage(const COLLADAFW::Image* pImage)
{
    switch(pImage->getSourceType())
    {
    case COLLADAFW::Image::SOURCE_TYPE_URI:
    {
        trace(L"* Image SOURCE_TYPE_URI '" + String(String8(pImage->getName().c_str())) + L"' with ID " + String(pImage->getUniqueId().toAscii().c_str()));
        trace(L"  Source file '" + String(String8(pImage->getImageURI().getPathFileBase().c_str())) + L"'");
        _uidImageToSourceFileName[pImage->getUniqueId()] = String(String8(pImage->getImageURI().getPathFileBase().c_str()));
        break;
    }
    case COLLADAFW::Image::SOURCE_TYPE_DATA:
    {
        trace(L"* Image SOURCE_TYPE_DATA with ID " + String(pImage->getUniqueId().toAscii().c_str()));
        break;
    }
    default:
    case COLLADAFW::Image::SOURCE_TYPE_UNKNOWN:
    {
        trace(L"* Image unknown type with ID " + String(pImage->getUniqueId().toAscii().c_str()));
        break;
    }
    }

    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeLight(const COLLADAFW::Light* light)
{
    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeAnimation(const COLLADAFW::Animation* pAnimation)
{
    switch (pAnimation->getAnimationType())
    {
    case COLLADAFW::Animation::ANIMATION_CURVE:
    {
        const COLLADAFW::AnimationCurve * pAnimCurve = static_cast<const COLLADAFW::AnimationCurve*>(pAnimation);
        trace(L"* Animation type ANIMATION_CURVE '" + String(pAnimCurve->getName().c_str()) + L"' " + String(pAnimCurve->getUniqueId().toAscii().c_str()));

        _traceDepth += 2;

        switch(pAnimCurve->getInterpolationType())
        {
        case COLLADAFW::AnimationCurve::INTERPOLATION_BEZIER:   trace(L"- Inter INTERPOLATION_BEZIER"); break;
        case COLLADAFW::AnimationCurve::INTERPOLATION_BSPLINE:  trace(L"- Inter INTERPOLATION_BSPLINE"); break;
        case COLLADAFW::AnimationCurve::INTERPOLATION_CARDINAL: trace(L"- Inter INTERPOLATION_CARDINAL"); break;
        case COLLADAFW::AnimationCurve::INTERPOLATION_HERMITE:  trace(L"- Inter INTERPOLATION_HERMITE"); break;
        case COLLADAFW::AnimationCurve::INTERPOLATION_LINEAR:   trace(L"- Inter INTERPOLATION_LINEAR"); break;
        case COLLADAFW::AnimationCurve::INTERPOLATION_MIXED:    trace(L"- Inter INTERPOLATION_MIXED"); break;
        case COLLADAFW::AnimationCurve::INTERPOLATION_STEP:     trace(L"- Inter INTERPOLATION_STEP"); break;
        case COLLADAFW::AnimationCurve::INTERPOLATION_UNKNOWN:  trace(L"- Inter INTERPOLATION_UNKNOWN"); break;
        default:                                                trace(L"- Inter ???"); break;
        }

        trace(L"- Key count " + Core::toString(pAnimCurve->getKeyCount()));
        trace(L"- Out dimension " + Core::toString(pAnimCurve->getOutDimension()));

        _uidToAnimationData[pAnimCurve->getUniqueId()] = convertAnimData(pAnimCurve, _swapYZ, _scale);

        _traceDepth -= 2;
        break;
    }
    case COLLADAFW::Animation::ANIMATION_FORMULA:
    {
        trace(L"* Animation type ANIMATION_FORMULA '" + String(pAnimation->getName().c_str()) + L" " + String(pAnimation->getUniqueId().toAscii().c_str()));
        break;
    }
    default:
    {
        trace(L"* Animation type ??? '" + String(pAnimation->getName().c_str()) + L" " + String(pAnimation->getUniqueId().toAscii().c_str()));
        break;
    }
    }

    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeAnimationList(const COLLADAFW::AnimationList* pAnimList)
{
    trace(L"* Animation list " + String(pAnimList->getUniqueId().toAscii().c_str()));

    _traceDepth += 2;
    const COLLADAFW::AnimationList::AnimationBindings & binds = pAnimList->getAnimationBindings();

    for(unsigned int ii=0; ii < binds.getCount(); ii++)
    {
        const COLLADAFW::AnimationList::AnimationBinding & bind = binds[ii];
        trace(L"- Binding " + Core::toString(ii));
        _traceDepth += 2;

        trace(L"- Animation " + String(bind.animation.toAscii().c_str()));

        switch(bind.animationClass)
        {
        case COLLADAFW::AnimationList::UNKNOWN_CLASS: trace(L"- Class UNKNOWN_CLASS"); break;
        case COLLADAFW::AnimationList::TIME: trace(L"- Class TIME"); break;
        case COLLADAFW::AnimationList::POSITION_XYZ: trace(L"- Class POSITION_XYZ"); break;
        case COLLADAFW::AnimationList::POSITION_X: trace(L"- Class POSITION_X"); break;
        case COLLADAFW::AnimationList::POSITION_Y: trace(L"- Class POSITION_Y"); break;
        case COLLADAFW::AnimationList::POSITION_Z: trace(L"- Class POSITION_Z"); break;
        case COLLADAFW::AnimationList::COLOR_RGB: trace(L"- Class COLOR_RGB"); break;
        case COLLADAFW::AnimationList::COLOR_RGBA: trace(L"- Class COLOR_RGBA"); break;
        case COLLADAFW::AnimationList::COLOR_R: trace(L"- Class COLOR_R"); break;
        case COLLADAFW::AnimationList::COLOR_G: trace(L"- Class COLOR_G"); break;
        case COLLADAFW::AnimationList::COLOR_B: trace(L"- Class COLOR_B"); break;
        case COLLADAFW::AnimationList::COLOR_A: trace(L"- Class COLOR_A"); break;
        case COLLADAFW::AnimationList::AXISANGLE: trace(L"- Class AXISANGLE"); break;
        case COLLADAFW::AnimationList::ANGLE: trace(L"- Class ANGLE"); break;
        case COLLADAFW::AnimationList::MATRIX4X4: trace(L"- Class MATRIX4X4"); break;
        case COLLADAFW::AnimationList::ARRAY_ELEMENT_1D: trace(L"- Class ARRAY_ELEMENT_1D"); break;
        case COLLADAFW::AnimationList::ARRAY_ELEMENT_2D: trace(L"- Class ARRAY_ELEMENT_2D"); break;
        case COLLADAFW::AnimationList::FLOAT: trace(L"- Class FLOAT"); break;
        }

        trace(L"- First  index " + Core::toString(bind.firstIndex));
        trace(L"- Second index " + Core::toString(bind.secondIndex));

        Core::Map<COLLADAFW::UniqueId, int32 >::const_iterator iNodeID = _uidAnimToNodeId.find(pAnimList->getUniqueId());
        Core::Map<COLLADAFW::UniqueId, Ptr<AnimationData> >::const_iterator iAnimData = _uidToAnimationData.find(bind.animation);

        if(iNodeID == _uidAnimToNodeId.end()) throw Core::Exception(L"COLLADADocWriter error : Animation/AnimationList binding not found");
        if(iAnimData == _uidToAnimationData.end()) throw Core::Exception(L"COLLADADocWriter error : Node/AnimationList binding not found");

        int32 nodeID = iNodeID->second;
        const AnimationData & data = *iAnimData->second;

        for(int32 iKey=0; iKey < data.keys.size(); iKey++)
            _anim.addKeyFrame(nodeID, Assets::KeyFrame(iKey, data.keys[iKey].transform));

        _anim.setDuration(data.duration);
        _animStartIndex = data.startIndex;

        _traceDepth -= 2;
    }

    _traceDepth -= 2;

    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeSkinControllerData(const COLLADAFW::SkinControllerData* pSkinData)
{
    trace(L"* SkinControllerData " + String(pSkinData->getUniqueId().toAscii().c_str()));
    _traceDepth += 2;

    trace(L"- Joint counts " + Core::toString(pSkinData->getJointsCount()));
    trace(L"- Vertex counts " + Core::toString(pSkinData->getVertexCount()));
    trace(L"- Weights counts " + Core::toString(pSkinData->getWeights().getValuesCount()));
    trace(L"- Joints/Vertex counts " + Core::toString(pSkinData->getJointsPerVertex().getCount()));
    trace(L"- Weights indices counts " + Core::toString(pSkinData->getWeightIndices().getCount()));
    trace(L"- Joint indices counts " + Core::toString(pSkinData->getJointIndices().getCount()));

    const COLLADAFW::FloatOrDoubleArray & weights = pSkinData->getWeights();
    const COLLADAFW::UIntValuesArray & jointsPerVertex = pSkinData->getJointsPerVertex();
    const COLLADAFW::UIntValuesArray & weightInd = pSkinData->getWeightIndices();
    const COLLADAFW::IntValuesArray & jointInd = pSkinData->getJointIndices();

    _uidToSkinData[pSkinData->getUniqueId()] = convertSkin(pSkinData, _swapYZ, _scale);

    _traceDepth -= 2;
    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeController(const COLLADAFW::Controller* pCtrl)
{
    switch(pCtrl->getControllerType())
    {
    case COLLADAFW::Controller::CONTROLLER_TYPE_SKIN:
    {
        const COLLADAFW::SkinController * pSkin = static_cast<const COLLADAFW::SkinController *>(pCtrl);
        const COLLADAFW::UniqueIdArray& joints = pSkin->getJoints();

        trace(L"* Controller type CONTROLLER_TYPE_SKIN " + String(pCtrl->getUniqueId().toAscii().c_str()));
        _traceDepth += 2;

        trace(L"- Source " + String(pSkin->getSource().toAscii().c_str()));
        trace(L"- Data " + String(pSkin->getSkinControllerData().toAscii().c_str()));

        for(int32 ii=0; ii < int32(joints.getCount()); ii++)
            trace(L"- Joint " + String(joints[ii].toAscii().c_str()));

        Core::Map<COLLADAFW::UniqueId, int32>::const_iterator iMeshID = _uidToMeshId.find(pSkin->getSource());
        Core::Map<COLLADAFW::UniqueId, Ptr<MeshData> >::const_iterator iMeshData = _uidToMeshData.find(pSkin->getSource());
        Core::Map<COLLADAFW::UniqueId, Ptr<SkinData> >::const_iterator iSkinData = _uidToSkinData.find(pSkin->getSkinControllerData());

        if(iMeshID == _uidToMeshId.end()) throw Core::Exception(L"COLLADADocWriter error : unknown mesh UID");
        if(iMeshData == _uidToMeshData.end()) throw Core::Exception(L"COLLADADocWriter error : unknown mesh data UID");
        if(iSkinData == _uidToSkinData.end()) throw Core::Exception(L"COLLADADocWriter error : unknown skin data UID");

        _uidToSkeletonData[pCtrl->getUniqueId()] = convertSkeleton(pSkin);
        _uidToSkeletonData[pCtrl->getUniqueId()]->pMesh = _pModel->getMeshPtr(iMeshID->second);

        Assets::VerticeSet & vset = _pModel->getMesh(iMeshID->second).getLODByID(0);
        applySkinToVertices(vset, *iMeshData->second, *iSkinData->second);
        _pModel->getMesh(iMeshID->second).setSkin(true);

        _traceDepth -= 2;
        break;
    }
    case COLLADAFW::Controller::CONTROLLER_TYPE_MORPH:
        trace(L"* Controller type CONTROLLER_TYPE_MORPH " + String(pCtrl->getUniqueId().toAscii().c_str()));
        break;
    }

    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeFormulas(const COLLADAFW::Formulas* formulas)
{
    return true;
}
//-----------------------------------------------------------------------------
bool COLLADADocWriter::writeKinematicsScene(const COLLADAFW::KinematicsScene* kinematicsScene)
{
    return true;
}
//-----------------------------------------------------------------------------
void COLLADADocWriter::trace(const String & text)
{
    if(_traceOn)
    {
        String indent;
        indent.resize(_traceDepth, L' ');
        INF << indent << text << L"\n";
    }
}
//-----------------------------------------------------------------------------
void COLLADADocWriter::warning(const String & w)
{
    trace(w);
    _warnings << w << L"\n";
}
//-----------------------------------------------------------------------------
}
