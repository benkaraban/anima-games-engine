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
#include "TrailRepresentation.h"

#include <Universe/World.h>
#include <EPI/Document/Properties/PtyTrajectory/PtyCheckPoint.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <Core/List.h>

#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>

#include <EPI/Constants.h>

namespace EPI
{
const String tagNodeForSelection       = L"TrailRepresentationSelection";

const String    TORUS_MODEL_NAME            = L"$TorusModel";
const String    TORUS_MESH_NAME             = L"$TorusMesh";

const float     TORUS_MINOR_RADIUS_FACTOR    = 0.03f;
const float     TORUS_MAJOR_RADIUS_FACTOR    = 0.2f;

//-----------------------------------------------------------------------------
void createTrailModels(Ptr<Universe::RessourcePool> pPool)
{
    Ptr<Assets::ModelMesh> torus (new Assets::ModelMesh());
    Assets::VerticeSet & vset = torus->getLODByID(0);
    Assets::createTorus(vset, TORUS_MINOR_RADIUS_FACTOR/2.f, TORUS_MAJOR_RADIUS_FACTOR, 1.0f);
    Assets::makeTBNBasis(vset, false);
    torus->cleanup();

    Ptr<Assets::Model> pTorusModel (new Assets::Model());
    pTorusModel->addMesh(TORUS_MESH_NAME, *torus);
    pPool->addModel(TORUS_MODEL_NAME, pTorusModel);
}

Ptr<Universe::NodeGroup> createTrailUniverseRepresentation(const Ptr<Universe::World>& pWorld)
{
    Renderer::Material  matTorusVertical;
    Renderer::Material  matTorusHorizontal;
    Renderer::Material  matTorusCenter;


    Renderer::Material mat;
        mat.diffuse.r = 1.0f;
        mat.diffuse.g = 0.f;
        mat.diffuse.b = 0.f;
        mat.diffuse.a = 1.f;
        mat.ambient = Core::Vector4f(0.1f);
        mat.ambient.a = 1.f;
        mat.dynamicShadowCaster = false;
        mat.dynamicShadowReceiver = false;
        mat.emissive = Core::Vector4f(0.f);
        mat.emissive.a = 0.f;
        mat.shininess = 12.f;
        mat.specular = Core::Vector4f(0.05f);
        mat.specular.a = 1.0f;
        mat.glowColor = Core::Vector4f(0.f);
        mat.glowColor.a = 1.f;
        mat.glowExtent = Core::Vector3f(0.f);
        mat.reflexions = Core::Vector4f(0.f);
        mat.reflexions.a = 0.f;
        mat.staticLighting = false;
        mat.transDiffuseTex = false;
        

    matTorusVertical = mat;
        matTorusVertical.diffuse.r = 0.1f;
        matTorusVertical.diffuse.g = 0.04f;
        matTorusVertical.diffuse.b = 0.5f;

    matTorusHorizontal = mat;
        matTorusHorizontal.diffuse.r = 0.1f;
        matTorusHorizontal.diffuse.g = 0.04f;
        matTorusHorizontal.diffuse.b = 0.5f;

    matTorusCenter = mat;
        matTorusCenter.diffuse.r = 0.2f;
        matTorusCenter.diffuse.g = 0.2f;
        matTorusCenter.diffuse.b = 0.4f;
        matTorusCenter.staticLighting = true;

    Ptr<Universe::NodeGroup> pRet = pWorld->createGroup();
  
    Ptr<Universe::NodeMesh> pNodeTorusVertical = pWorld->createMesh(TORUS_MODEL_NAME, TORUS_MESH_NAME);
    pRet->addChild(pNodeTorusVertical);

    Ptr<Universe::NodeMesh> pNodeTorusHorizontal = pWorld->createMesh(TORUS_MODEL_NAME, TORUS_MESH_NAME);
    pNodeTorusHorizontal->localPitch(f_PI_DIV_2);
    pRet->addChild(pNodeTorusHorizontal);

    Ptr<Universe::NodeMesh> pNodeCenter  = pWorld->createMesh();
    pNodeCenter->setLocalScale(Core::Vector3f(0.005f,0.005f,0.005f));
    pNodeCenter->addTag(tagNodeForSelection);
    pRet->addChild(pNodeCenter);
    

    pNodeTorusVertical->getMeshInstance()->setMaterial(matTorusVertical);
    pNodeTorusHorizontal->getMeshInstance()->setMaterial(matTorusHorizontal);
    pNodeCenter->getMeshInstance()->setMaterial(matTorusCenter);

    pRet->setLocalScale(Core::Vector3f(2.f,2.f,2.f));

    return pRet;
}
//-----------------------------------------------------------------------------
TrailRepresentation::TrailRepresentation(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNodeToRepresent):
    IContentRepresentation(),
    _mode(SELECTION_REPRESENTATION),
    _pWorld(pWorld)
{
    createTrailModels(_pWorld->getRessourcesPool());

    _pNodeRepresentation = createTrailUniverseRepresentation(pWorld);

    pNodeToRepresent->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
TrailRepresentation::TrailRepresentation(const TrailRepresentation& other):
    IContentRepresentation(other),
    _pWorld(other._pWorld)
{
    _pNodeRepresentation = createTrailUniverseRepresentation(_pWorld);
}
//-----------------------------------------------------------------------------
TrailRepresentation::~TrailRepresentation()
{
    _pNodeRepresentation->kill();
}
//-----------------------------------------------------------------------------
void TrailRepresentation::updateObjectRepresentation(const Property& pPty)
{

}
//-----------------------------------------------------------------------------
void TrailRepresentation::setRepresentationMode(ECRMode mode)
{
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeGroup>& TrailRepresentation::getNodeRepresentation()
{
    return _pNodeRepresentation;
}
//-----------------------------------------------------------------------------
bool TrailRepresentation::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (pNode==_pNodeRepresentation)    return true;
    return false;
}
//-----------------------------------------------------------------------------
Ptr<IContentRepresentation> TrailRepresentation::clone()
{
    Ptr<TrailRepresentation>   pRet = Ptr<TrailRepresentation>(new TrailRepresentation (*this));

    return pRet;
}
//-----------------------------------------------------------------------------
void TrailRepresentation::setNodeToRepresent(const Ptr<Universe::Node>& pNode)
{
    pNode->addGhostChild(pNode);
}
//-----------------------------------------------------------------------------
bool TrailRepresentation::isIntersecting (const Core::Rayf& ray, float& distance)
{
    if (isVisible==false)   return false;

    Ptr<Universe::Node> pNode = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagNodeForSelection));
    return pNode->isIntersecting(ray, distance);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI