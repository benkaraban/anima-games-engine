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
#include "CheckPointRepresentation.h"

#include <Universe/World.h>
#include <EPI/Document/Properties/PtyTrajectory/PtyCheckPoint.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <Core/List.h>

#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>

#include <EPI/Constants.h>

namespace EPI
{
const String tagNodeDecalForSelection       = L"CheckPointDecalSelection";
const String tagNodeLocalAxis               = L"CheckPointLocalAxis";

const String    ARROW_MODEL_NAME            = L"$CheckPointArrow";
const String    ARROW_MESH_NAME             = L"$CheckPointArrowMesh";

const float     ARROW_SIZE_FACTOR           = 2.6f;

const Core::Vector4f colorDecalNoSelected   (0.1f, 0.7f, 1.f, 1.f);
const Core::Vector4f colorDecalSelected     (0.85f, 0.75f, 0.1f, 1.f);

//-----------------------------------------------------------------------------
void createCheckPointModels(Ptr<Universe::RessourcePool> pPool)
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

Ptr<Universe::NodeGroup> createCheckPointUniverseRepresentation(const Ptr<Universe::World>& pWorld)
{
    Ptr<Universe::NodeGroup> pRet = pWorld->createGroup();

    Ptr<Universe::NodeDecal> pDecal = pWorld->createDecal();
    pDecal->setFixedSize(true, 1/125.f);
    pDecal->setBillboard(true);
    pDecal->setDecalNormal(Renderer::DECAL_NORMAL_Z);
    
    pDecal->setColor(colorDecalNoSelected);
    pDecal->addTag(tagNodeDecalForSelection);

    Ptr<Universe::NodeGroup> pGroup = pWorld->createGroup();
    pGroup->setFixedSize(true, 1/20.f);
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
    pRet->addChild(pDecal);

    return pRet;
}
//-----------------------------------------------------------------------------
CheckPointRepresentation::CheckPointRepresentation(const Ptr<Universe::World>& pWorld):
    IContentRepresentation(),
    _mode(SELECTION_REPRESENTATION),
    _pWorld(pWorld)
{
    createCheckPointModels(_pWorld->getRessourcesPool());

    _pNodeRepresentation = createCheckPointUniverseRepresentation(pWorld);
}
//-----------------------------------------------------------------------------
CheckPointRepresentation::CheckPointRepresentation(const CheckPointRepresentation& other):
    IContentRepresentation(other),
    _pWorld(other._pWorld)
{
    _pNodeRepresentation = createCheckPointUniverseRepresentation(_pWorld);
}
//-----------------------------------------------------------------------------
CheckPointRepresentation::~CheckPointRepresentation()
{
    _pNodeRepresentation->kill();
}
//-----------------------------------------------------------------------------
void CheckPointRepresentation::updateObjectRepresentation(const Property& pPty)
{
    
    Ptr<Universe::NodeDecal> pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeRepresentation->getChildWithTag(tagNodeDecalForSelection));

/*
    if (pPty.getCalssName() == PTY_TRANSFORM)
    {
        const PtyTransform& ptyT = static_cast<const PtyTransform&>(pPty);
        pNodeDecal->setWorldPosition(ptyT.getWorldPosition());
    }
    else
    {*/
        const PtyCheckPoint& pPtyCP = static_cast<const PtyCheckPoint&>(pPty);
        Ptr<PtyTransform>   pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(pPtyCP.getPtyTransform());
        //pNodeDecal->setWorldPosition(pPtyT->getWorldPosition());
        _pNodeRepresentation->setLocalPosition(pPtyT->getWorldPosition());
        _pNodeRepresentation->setLocalOrientation(Core::Quaternionf(pPtyT->getWorldAxis()));
  //  }

    
    
}
//-----------------------------------------------------------------------------
void CheckPointRepresentation::setRepresentationMode(ECRMode mode)
{
    Ptr<Universe::NodeDecal> pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeRepresentation->getChildWithTag(tagNodeDecalForSelection));

    _mode = mode;
    
    if (_mode==OBJECT_REPRESENTATION)
    {
        pNodeDecal->setColor(colorDecalSelected);
    }
    else
    {
        pNodeDecal->setColor(colorDecalNoSelected);
    }
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeGroup>& CheckPointRepresentation::getNodeRepresentation()
{
    return _pNodeRepresentation;
}
//-----------------------------------------------------------------------------
bool CheckPointRepresentation::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (pNode==_pNodeRepresentation)    return true;
    return false;
}
//-----------------------------------------------------------------------------
Ptr<IContentRepresentation> CheckPointRepresentation::clone()
{
    Ptr<CheckPointRepresentation>   pRet = Ptr<CheckPointRepresentation>(new CheckPointRepresentation (*this));

    return pRet;
}
//-----------------------------------------------------------------------------
void CheckPointRepresentation::setNodeToRepresent(const Ptr<Universe::Node>& pNode)
{

}
//-----------------------------------------------------------------------------
bool CheckPointRepresentation::isIntersecting (const Core::Rayf& ray, float& distance)
{
    if (isVisible==false)   return false;

    Ptr<Universe::NodeDecal> pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeRepresentation->getChildWithTag(tagNodeDecalForSelection));
    return pNodeDecal->isIntersecting(ray, distance);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI