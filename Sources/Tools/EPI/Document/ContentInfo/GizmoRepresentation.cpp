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
#include "GizmoRepresentation.h"

#include <Universe/World.h>
#include <EPI/Document/Properties/PtyGizmo.h>

#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>
#include <EPI/Constants.h>

namespace EPI
{
const String    ARROW_MODEL_NAME            = L"$GizmoArrow";
const String    ARROW_MESH_NAME             = L"$GizmoArrowMesh";

const float     ARROW_SIZE_FACTOR           = 0.5f;

const String tagNodeLocalAxis               = L"GizmoLocalAxis";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void createGizmoModels(Ptr<Universe::RessourcePool> pPool)
{
    //arrow
    Ptr<Assets::ModelMesh> arrow (new Assets::ModelMesh());
    Assets::VerticeSet & vsetArrow = arrow->getLODByID(0);
    Assets::createArrow(vsetArrow, ARROW_SIZE_FACTOR);
    Assets::makeTBNBasis(vsetArrow, false);
    arrow->cleanup();

    Ptr<Assets::Model> pArrowModel (new Assets::Model());
    pArrowModel->addMesh(ARROW_MESH_NAME, *arrow);
    pPool->addModel(ARROW_MODEL_NAME, pArrowModel);
}
Ptr<Universe::NodeGroup> GizmoRepresentation::createGizmoUniverseRepresentation(const Ptr<Universe::World>& pWorld)
{
    createGizmoModels(pWorld->getRessourcesPool());

    Ptr<Universe::NodeGroup> pRet = pWorld->createGroup();

    Ptr<Universe::NodeGroup> pGroup = pWorld->createGroup();
  //  pGroup->setFixedSize(true, 1/20.f);
    pGroup->addTag(tagNodeLocalAxis);

    Ptr<Universe::NodeMesh> pNodeArrowAxeX = pWorld->createMesh(ARROW_MODEL_NAME, ARROW_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeArrowAxeY = pWorld->createMesh(ARROW_MODEL_NAME, ARROW_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeArrowAxeZ = pWorld->createMesh(ARROW_MODEL_NAME, ARROW_MESH_NAME);

    pNodeArrowAxeX->getMeshInstance()->setMaterial(RdrConst::sMatAxisX);
    pNodeArrowAxeY->getMeshInstance()->setMaterial(RdrConst::sMatAxisY);
    pNodeArrowAxeZ->getMeshInstance()->setMaterial(RdrConst::sMatAxisZ);

    pNodeArrowAxeX->beginMatrixUpdate();
        pNodeArrowAxeX->localRoll(-f_PI_DIV_2);
        pNodeArrowAxeX->setLocalPosition(Core::Vector3f(ARROW_SIZE_FACTOR,0,0));
    pNodeArrowAxeX->endMatrixUpdate();

    pNodeArrowAxeY->beginMatrixUpdate();
        pNodeArrowAxeY->setLocalPosition(Core::Vector3f(0,ARROW_SIZE_FACTOR,0));
    pNodeArrowAxeY->endMatrixUpdate();

    pNodeArrowAxeZ->beginMatrixUpdate();
        pNodeArrowAxeZ->localPitch(f_PI_DIV_2);
        pNodeArrowAxeZ->setLocalPosition(Core::Vector3f(0,0,ARROW_SIZE_FACTOR));
    pNodeArrowAxeZ->endMatrixUpdate();

    pGroup->addChild(pNodeArrowAxeX);
    pGroup->addChild(pNodeArrowAxeY);
    pGroup->addChild(pNodeArrowAxeZ);

    pRet->addChild(pGroup);

    return pRet;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GizmoRepresentation::GizmoRepresentation(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNodeToRepresent):
    IContentRepresentation(),
    _mode(SELECTION_REPRESENTATION),
    _pWorld(pWorld)
{
    _pNodeRepresentation = createGizmoUniverseRepresentation(_pWorld);

    pNodeToRepresent->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
GizmoRepresentation::GizmoRepresentation(const GizmoRepresentation& other):
    IContentRepresentation(other),
    _pWorld(other._pWorld)
{
    _pNodeRepresentation = createGizmoUniverseRepresentation(_pWorld);
}
//-----------------------------------------------------------------------------
GizmoRepresentation::~GizmoRepresentation()
{
    _pNodeRepresentation->kill();
}
//-----------------------------------------------------------------------------
void GizmoRepresentation::updateObjectRepresentation(const Property& pPty)
{
    const PtyNodeGizmo& pPtyGizmo = static_cast<const PtyNodeGizmo&>(pPty);
}
//-----------------------------------------------------------------------------
void GizmoRepresentation::setRepresentationMode(ECRMode mode)
{
    _mode = mode;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeGroup>& GizmoRepresentation::getNodeRepresentation()
{
    return _pNodeRepresentation;
}
//-----------------------------------------------------------------------------
bool GizmoRepresentation::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (pNode==_pNodeRepresentation)    return true;
    return false;
}
//-----------------------------------------------------------------------------
Ptr<IContentRepresentation> GizmoRepresentation::clone()
{
    Ptr<GizmoRepresentation>   pRet = Ptr<GizmoRepresentation>(new GizmoRepresentation (*this));

    return pRet;
}
//-----------------------------------------------------------------------------
void GizmoRepresentation::setNodeToRepresent(const Ptr<Universe::Node>& pNode)
{
    pNode->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI