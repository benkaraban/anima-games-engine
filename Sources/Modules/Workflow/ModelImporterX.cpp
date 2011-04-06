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
#include <Assets/Model.h>
#include <Workflow/XFile.h>
#include "ModelImporterX.h"

namespace Workflow
{
void convertAnim(const XAnimation & xanim, Assets::Animation & anim, int32 nodeId);
void convertAnimSet(const Assets::Model & model, const XAnimationSet & xanimSet, Assets::Animation & anim);
void bindFrameMesh(Assets::Model & model, const XFrame & xframe);
void convertFrame(Assets::Model & model, XFrame & xframe, int32 parentId);
void convertBone(Assets::Model & model, const XBone & xbone, Assets::ModelMesh & mesh);
void convertMesh(Assets::Model & model, const XMesh & xmesh, Assets::ModelMesh & modelMesh);
Ptr<Assets::Model> convertModel(XModel & xmodel);
//-----------------------------------------------------------------------------
void convertAnim(const XAnimation & xanim, Assets::Animation & anim, int32 nodeId)
{
    if(xanim.matKeys.size() != 0)
    {
        LM_ASSERT(xanim.transKeys.size() == 0);
        LM_ASSERT(xanim.orientKeys.size() == 0);
        LM_ASSERT(xanim.scaleKeys.size() == 0);
        for(Core::List<XMatKey>::const_iterator iMatKey = xanim.matKeys.begin(); iMatKey != xanim.matKeys.end(); ++iMatKey)
            anim.addKeyFrame(nodeId, Assets::KeyFrame(iMatKey->keyFrame, iMatKey->matrix));
    }
    else
    {
        int32 it = 0; // Index trans
        int32 io = 0; // Index orient
        int32 is = 0; // Index scale

        Core::Vector3f    position(0.0, 0.0, 0.0);
        Core::Quaternionf orient(Core::IDENTITY);
        Core::Vector3f    scale(1.0, 1.0, 1.0);
        
        while(it < xanim.transKeys.size() || io < xanim.orientKeys.size() || is < xanim.scaleKeys.size())
        {
            int32 keyt = it < xanim.transKeys.size()  ? xanim.transKeys[it].keyFrame  : Core::INT32_MAX;
            int32 keyo = io < xanim.orientKeys.size() ? xanim.orientKeys[io].keyFrame : Core::INT32_MAX;
            int32 keys = is < xanim.scaleKeys.size()  ? xanim.scaleKeys[is].keyFrame  : Core::INT32_MAX;

            int32 keyFrame = std::min(keyt, std::min(keyo, keys));

            if(it < xanim.transKeys.size())
            {
                if(keyFrame == keyt)
                {
                    position = xanim.transKeys[it].position;
                    it ++;
                }
                else
                {
                    int32 lastIT = std::max(0, int32(it - 1));
                    int32 lastKeyt = xanim.transKeys[lastIT].keyFrame;
                    float t = float(keyFrame - lastKeyt) / float(std::max(1, keyt - lastKeyt));
                    position.setLerp(xanim.transKeys[lastIT].position, xanim.transKeys[it].position, t);
                }
            }

            if(io < xanim.orientKeys.size())
            {
                if(keyFrame == keyo)
                {
                    orient = xanim.orientKeys[io].orient;
                    io ++;
                }
                else
                {
                    int32 lastIO = std::max(0, int32(io - 1));
                    int32 lastKeyo = xanim.orientKeys[lastIO].keyFrame;
                    float t = float(keyFrame - lastKeyo) / float(std::max(1, keyo - lastKeyo));
                    orient.setSlerp(xanim.orientKeys[lastIO].orient, xanim.orientKeys[io].orient, t);
                }
            }

            if(is < xanim.scaleKeys.size())
            {
                if(keyFrame == keys)
                {
                    scale = xanim.scaleKeys[is].scale;
                    is ++;
                }
                else
                {
                    int32 lastIS = std::max(0, int32(is - 1));
                    int32 lastKeys = xanim.scaleKeys[lastIS].keyFrame;
                    float t = float(keyFrame - lastKeys) / float(std::max(1, keys - lastKeys));
                    scale.setLerp(xanim.scaleKeys[lastIS].scale, xanim.scaleKeys[is].scale, t);
                }
            }

            Core::Transformf transform(position, orient, scale);
            anim.addKeyFrame(nodeId, Assets::KeyFrame(keyFrame, transform));
        }
    }
}
//-----------------------------------------------------------------------------
void convertAnimSet(const Assets::Model & model, const XAnimationSet & xanimSet, Assets::Animation & anim)
{
    for(Core::List<Ptr<XAnimation> >::const_iterator iAnim = xanimSet.anims.begin(); iAnim != xanimSet.anims.end(); ++iAnim)
    {
        int32 nodeId = model.getNodeId((*iAnim)->frameName);
        LM_ASSERT(nodeId >= Assets::UNDEFINED_NODE_ID);
        convertAnim(**iAnim, anim, nodeId);
    }
}
//-----------------------------------------------------------------------------
void bindFrameMesh(Assets::Model & model, const XFrame & xframe)
{
    if(!xframe.meshes.empty())
    {
        LM_ASSERT(xframe.meshes.size() == 1);
        model.getNode(xframe.id).setMeshId(xframe.meshes[0]->id);
    }

    for(Core::List<Ptr<XFrame> >::const_iterator iFrame = xframe.frames.begin(); iFrame != xframe.frames.end(); ++iFrame)
        bindFrameMesh(model, **iFrame);
}
//-----------------------------------------------------------------------------
void convertFrame(Assets::Model & model, XFrame & xframe, int32 parentId)
{
    int32 idNode = model.addNode(xframe.name, parentId, xframe.matrix);
    xframe.id = idNode;
    for(Core::List<Ptr<XFrame> >::iterator iFrame = xframe.frames.begin(); iFrame != xframe.frames.end(); ++iFrame)
        convertFrame(model, **iFrame, idNode);
}
//-----------------------------------------------------------------------------
void convertBone(Assets::Model & model, const XBone & xbone, Assets::ModelMesh & mesh)
{
    int32 nodeId = model.getNodeId(xbone.frameName);
    if(nodeId == Assets::UNDEFINED_NODE_ID)
    {
        ERR << "ERROR : unknown node " << xbone.frameName << L"\n";
        return;
    }
    int32 boneId = model.getSkeleton().addBone(nodeId, xbone.offsetMatrix);

    for(int32 iLOD=0; iLOD < mesh.getLODCount(); iLOD++)
    {
        Core::List<Assets::Vertex> & vertices = mesh.getLODByID(iLOD).getVertices();
        for(int32 iIndice=0; iIndice < xbone.indices.size(); iIndice++)
        {
            int32 indice = xbone.indices[iIndice];

            int32 iLeastImportantBone = -1;
            float leastImportantWeight = xbone.weight[iIndice];

            for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
            {
                if(vertices[indice].boneWeight[iBone] < leastImportantWeight)
                {
                    iLeastImportantBone = iBone;
                    leastImportantWeight = vertices[indice].boneWeight[iBone];
                }
            }

            if(iLeastImportantBone != -1)
            {
                vertices[indice].boneId[iLeastImportantBone] = boneId;
                vertices[indice].boneWeight[iLeastImportantBone] = xbone.weight[iIndice];
            }
        }
    }

    mesh.setSkin(true);
}
//-----------------------------------------------------------------------------
void convertMesh(Assets::Model & model, const XMesh & xmesh, Assets::ModelMesh & modelMesh)
{
    modelMesh.setName(xmesh.name);
    modelMesh.setLODCount(1);

    Assets::VerticeSet & vset = *modelMesh.getLOD(Assets::LOD_HIGH);
    Core::List<uint16> & ind = vset.getIndices();
    Core::List<Assets::Vertex> & vert = vset.getVertices();

    vert.resize(xmesh.positions.size());
    for(int32 ii=0; ii < xmesh.positions.size(); ii++)
        vert[ii].position = xmesh.positions[ii];

    for(int32 ii=0; ii < xmesh.texCoords.uv.size(); ii++)
        vert[ii].texCoords = Core::Vector2f(xmesh.texCoords.uv[ii].u, xmesh.texCoords.uv[ii].v);

    ind.reserve(3 * xmesh.faces.size());

    for(int32 ii=0; ii < xmesh.faces.size(); ii++)
    {
        ind.push_back(xmesh.faces[ii].i1);
        ind.push_back(xmesh.faces[ii].i2);
        ind.push_back(xmesh.faces[ii].i3);
    }

    if(xmesh.normales.faces.size() > 0)
    {
        Core::List<bool> isNormaleDefined(ind.size(), false);
        const XNormales & xnorm = xmesh.normales;

        Core::List<int32> indNormales;
        indNormales.reserve(3 * xmesh.normales.faces.size());

        for(int32 ii=0; ii < xmesh.normales.faces.size(); ii++)
        {
            indNormales.push_back(xmesh.normales.faces[ii].i1);
            indNormales.push_back(xmesh.normales.faces[ii].i2);
            indNormales.push_back(xmesh.normales.faces[ii].i3);
        }

        for(int32 ii=0; ii < indNormales.size(); ii++)
        {
            const Core::Vector3f & normale = xnorm.normales[indNormales[ii]];
            int32 indiceVertex = ind[ii];

            if(!isNormaleDefined[indiceVertex])
            {
                vert[indiceVertex].normale = normale;
                isNormaleDefined[indiceVertex] = true;
            }
            else if(vert[indiceVertex].normale != normale)
            {
                Assets::Vertex v(vert[indiceVertex]);
                v.normale = normale;

                indiceVertex = vert.size();
                vert.push_back(v);
                ind[ii] = indiceVertex;
            }
        }
    }

    for(Core::List<Ptr<XBone> >::const_iterator iBone = xmesh.bones.begin(); iBone != xmesh.bones.end(); ++iBone)
        convertBone(model, **iBone, modelMesh);
}
//-----------------------------------------------------------------------------
Ptr<Assets::Model> convertModel(XModel & xmodel)
{
    Ptr<Assets::Model> pModel(new Assets::Model());

    for(Core::List<Ptr<XFrame> >::iterator iFrame = xmodel.rootFrames.begin(); iFrame != xmodel.rootFrames.end(); ++iFrame)
        convertFrame(*pModel, **iFrame, Assets::ROOT_NODE_ID);

    for(Core::List<Ptr<XMesh> >::const_iterator iMesh = xmodel.meshes.begin(); iMesh != xmodel.meshes.end(); ++iMesh)
    {
        Assets::ModelMesh mesh;
        convertMesh(*pModel, **iMesh, mesh);
        (*iMesh)->id = pModel->addMesh((*iMesh)->name, mesh);
    }

    for(Core::List<Ptr<XFrame> >::const_iterator iFrame = xmodel.rootFrames.begin(); iFrame != xmodel.rootFrames.end(); ++iFrame)
        bindFrameMesh(*pModel, **iFrame);

    for(Core::List<Ptr<XAnimationSet> >::const_iterator iAnim = xmodel.animSets.begin(); iAnim != xmodel.animSets.end(); ++iAnim)
    {
        Assets::Animation animSet(pModel->getNodeCount());
        convertAnimSet(*pModel, **iAnim, animSet);
        animSet.setDuration(float(animSet.getFrameCount()) / (*iAnim)->ticksPerSecond);
        pModel->addAnimation((*iAnim)->name, animSet);
    }

    pModel->cleanup();

    return pModel;
}
//-----------------------------------------------------------------------------
ModelImporterX::ModelImporterX()
{
}
//-----------------------------------------------------------------------------
bool ModelImporterX::supportFormat(const String & fileExtension) const
{
    return fileExtension == L"x";
}
//-----------------------------------------------------------------------------
Ptr<Assets::Model> ModelImporterX::importModel(Core::InputStream & input)
{
    char magic[5]   = { '\0', '\0', '\0', '\0', '\0' };
    char majorv[3]  = { '\0', '\0', '\0' };
    char minorv[3]  = { '\0', '\0', '\0' };
    char format[5]  = { '\0', '\0', '\0', '\0', '\0' };
    char floatf[5]  = { '\0', '\0', '\0', '\0', '\0' };

    input.readData(magic, 4);
    input.readData(majorv, 2);
    input.readData(minorv, 2);
    input.readData(format, 4);
    input.readData(floatf, 4);

    if(strcmp(magic, "xof ") != 0)
        throw Core::IOException(L"Error importing XFile : bad ID in header");

    bool float64;

    if(strcmp(floatf, "0064") == 0)
        float64 = true;
    else if(strcmp(floatf, "0032") == 0)
        float64 = false;
    else
    {
        String message(L"Error importing XFile : bad float format in header -> ");
        message << String(String8(floatf));
        throw Core::IOException(message);
    }

    XModel xmodel;

    if(strcmp(format, "txt ") == 0)
    {
        XFileParserText parser(input, float64);
        parser.parseXModel(xmodel);
    }
    else
    {
        String message(L"Error importing XFile : unsupported format -> ");
        message << String(String8(format));
        throw Core::IOException(message);
    }

    return convertModel(xmodel);
}
//-----------------------------------------------------------------------------
}
