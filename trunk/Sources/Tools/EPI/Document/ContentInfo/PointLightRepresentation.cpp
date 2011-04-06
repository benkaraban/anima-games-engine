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
#include "PointLightRepresentation.h"

#include <Universe/World.h>
#include <EPI/Document/Properties/PtyTrajectory/PtyCheckPoint.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <Core/List.h>

#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>

#include <EPI/Constants.h>

namespace EPI
{
const String tagNodeForSelection       = L"PoijntLightRepresentationDecalSelection";


//-----------------------------------------------------------------------------
void createPointLightModels(Ptr<Universe::RessourcePool> pPool)
{

}

Ptr<Universe::NodeGroup> createPointUniverseRepresentation(const Ptr<Universe::World>& pWorld)
{
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

    matTorusCenter = mat;
        matTorusCenter.diffuse.r = 0.5f;
        matTorusCenter.diffuse.g = 0.5f;
        matTorusCenter.diffuse.b = 0.5f;


    Ptr<Universe::NodeGroup> pRet = pWorld->createGroup();

    Ptr<Universe::NodeMesh> pNodeCenter  = pWorld->createMesh();
    pNodeCenter->setLocalScale(Core::Vector3f(0.005f,0.005f,0.005f));
    pNodeCenter->addTag(tagNodeForSelection);
    pRet->addChild(pNodeCenter);

    pNodeCenter->getMeshInstance()->setMaterial(matTorusCenter);

    pRet->setLocalScale(Core::Vector3f(2.f,2.f,2.f));

    return pRet;
}
//-----------------------------------------------------------------------------
PointLightRepresentation::PointLightRepresentation(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNodeToRepresent):
    IContentRepresentation(),
    _mode(SELECTION_REPRESENTATION),
    _pWorld(pWorld)
{
    createPointLightModels(_pWorld->getRessourcesPool());

    _pNodeRepresentation = createPointUniverseRepresentation(pWorld);

    pNodeToRepresent->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
PointLightRepresentation::PointLightRepresentation(const PointLightRepresentation& other):
    IContentRepresentation(other),
    _pWorld(other._pWorld)
{
    _pNodeRepresentation = createPointUniverseRepresentation(_pWorld);
}
//-----------------------------------------------------------------------------
PointLightRepresentation::~PointLightRepresentation()
{
    _pNodeRepresentation->kill();
}
//-----------------------------------------------------------------------------
void PointLightRepresentation::updateObjectRepresentation(const Property& pPty)
{

}
//-----------------------------------------------------------------------------
void PointLightRepresentation::setRepresentationMode(ECRMode mode)
{
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeGroup>& PointLightRepresentation::getNodeRepresentation()
{
    return _pNodeRepresentation;
}
//-----------------------------------------------------------------------------
bool PointLightRepresentation::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (pNode==_pNodeRepresentation)    return true;
    return false;
}
//-----------------------------------------------------------------------------
Ptr<IContentRepresentation> PointLightRepresentation::clone()
{
    Ptr<PointLightRepresentation>   pRet = Ptr<PointLightRepresentation>(new PointLightRepresentation (*this));

    return pRet;
}
//-----------------------------------------------------------------------------
void PointLightRepresentation::setNodeToRepresent(const Ptr<Universe::Node>& pNode)
{
    pNode->addGhostChild(pNode);
}
//-----------------------------------------------------------------------------
bool PointLightRepresentation::isIntersecting (const Core::Rayf& ray, float& distance)
{
    if (isVisible==false)   return false;

    Ptr<Universe::Node> pNode = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagNodeForSelection));
    return pNode->isIntersecting(ray, distance);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI