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
#include "SplineRepresentation.h"

#include <Universe/World.h>
#include <EPI/Document/Properties/PtyTrajectory/PtyTrajectory.moc.h>
#include <Core/Math/Splines/CatmullRom.h>
#include <Core/List.h>
#include <Core/Math/Splines/Trajectory.h>


#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>

#include <EPI/Document/ContentInfo/SplineRepresentation.h>

#include <EPI/Constants.h>

namespace EPI
{
const String tagNodeSpline       = L"SplineRepresentationTrajectory";
//const String tagNodeGroupDecal   = L"GroupDecalRepresentationTrajectory";
const String tagNodeLocalAxis               = L"CheckPointLocalAxis";
const String    SPLINE_ARROW_MODEL_NAME            = L"$SplineArrow";
const String    SPLINE_ARROW_MESH_NAME             = L"$SplineArrowMesh";
const float     SPLINE_ARROW_SIZE_FACTOR           = 2.6f;

//-----------------------------------------------------------------------------
void createSplineModels(Ptr<Universe::RessourcePool> pPool)
{
    //arrow
    Ptr<Assets::ModelMesh> arrow (new Assets::ModelMesh());
    Assets::VerticeSet & vsetArrow = arrow->getLODByID(0);
    Assets::createArrow(vsetArrow, SPLINE_ARROW_SIZE_FACTOR);
    Assets::makeTBNBasis(vsetArrow, false);
    arrow->cleanup();

    Ptr<Assets::Model> pArrowModel (new Assets::Model());
    pArrowModel->addMesh(SPLINE_ARROW_MESH_NAME, *arrow);
    pPool->addModel(SPLINE_ARROW_MODEL_NAME, pArrowModel);
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeGroup> createSplineUniverseRepresentation(const Ptr<Universe::World>& pWorld)
{
    Ptr<Universe::NodeGroup>    pRet;

    pRet = pWorld->createGroup();

    Ptr<Universe::NodeSpline> pNodeSpline = pWorld->createSpline();
        pNodeSpline->addTag(tagNodeSpline);
        pNodeSpline->setColor(Core::Vector4f(0.8f,0.8f,0.8f,1.f));



    Ptr<Universe::NodeGroup> pGroup = pWorld->createGroup();
        pGroup->setFixedSize(true, 1/20.f);
        pGroup->addTag(tagNodeLocalAxis);

    Ptr<Universe::NodeMesh> pNodeArrowAxeX = pWorld->createMesh(SPLINE_ARROW_MODEL_NAME, SPLINE_ARROW_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeArrowAxeY = pWorld->createMesh(SPLINE_ARROW_MODEL_NAME, SPLINE_ARROW_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeArrowAxeZ = pWorld->createMesh(SPLINE_ARROW_MODEL_NAME, SPLINE_ARROW_MESH_NAME);

    pNodeArrowAxeX->getMeshInstance()->setMaterial(RdrConst::sMatAxisX);
    pNodeArrowAxeY->getMeshInstance()->setMaterial(RdrConst::sMatAxisY);
    pNodeArrowAxeZ->getMeshInstance()->setMaterial(RdrConst::sMatAxisZ);

    pNodeArrowAxeX->beginMatrixUpdate();
        pNodeArrowAxeX->localRoll(-f_PI_DIV_2);
        pNodeArrowAxeX->setLocalPosition(Core::Vector3f(SPLINE_ARROW_SIZE_FACTOR,0,0));
    pNodeArrowAxeX->endMatrixUpdate();

    pNodeArrowAxeY->beginMatrixUpdate();
        pNodeArrowAxeY->setLocalPosition(Core::Vector3f(0,SPLINE_ARROW_SIZE_FACTOR,0));
    pNodeArrowAxeY->endMatrixUpdate();

    pNodeArrowAxeZ->beginMatrixUpdate();
        pNodeArrowAxeZ->localPitch(f_PI_DIV_2);
        pNodeArrowAxeZ->setLocalPosition(Core::Vector3f(0,0,SPLINE_ARROW_SIZE_FACTOR));
    pNodeArrowAxeZ->endMatrixUpdate();

    pGroup->addChild(pNodeArrowAxeX);
    pGroup->addChild(pNodeArrowAxeY);
    pGroup->addChild(pNodeArrowAxeZ);

    pRet->addChild(pNodeSpline);
    pRet->addChild(pGroup);

    return pRet;
}
//-----------------------------------------------------------------------------
SplineRepresentation::SplineRepresentation(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNodeToRepresent):
    IContentRepresentation(),
    _mode(SELECTION_REPRESENTATION),
    _pWorld(pWorld)
{
    createSplineModels(_pWorld->getRessourcesPool());

    _pNodeRepresentation = createSplineUniverseRepresentation(_pWorld);

    pNodeToRepresent->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
SplineRepresentation::SplineRepresentation(const SplineRepresentation& other):
    IContentRepresentation(other),
    _pWorld(other._pWorld)
{
    //TODO
    LM_ASSERT(false);
}
//-----------------------------------------------------------------------------
SplineRepresentation::~SplineRepresentation()
{
    _pNodeRepresentation->kill();
}
//-----------------------------------------------------------------------------
void SplineRepresentation::updateObjectRepresentation(const Property& pPty)
{
    const PtyTrajectory& pPtyT = static_cast<const PtyTrajectory&>(pPty);

    Ptr<Universe::NodeSpline> pNodeSpline = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSpline));

    const Core::Trajectory& traj = pPtyT.getTrajectory();
    const Core::List<Core::CheckPoint> & cp = traj.getCheckPoints();
/*
    Ptr<Universe::Node> pNode = ((const PtyNode&)(pPty)).getUniverseNode();

    _pNodeRepresentation->setWorldPosition(pNode->getWorldPosition());
    _pNodeRepresentation->setLocalOrientation(pNode->getLocalOrient());*/
    //Spline
    Core::List<Core::Vector3f>  pos;
    for (int32 ii=0; ii<cp.size(); ++ii)
    {
        pos.push_back(cp[ii].position);

        if (ii==0 || ii==cp.size()-1)
        {
            pos.push_back(cp[ii].position);
            pos.push_back(cp[ii].position);
        }
    }

    Core::CRSpline  sp(pos);
    pNodeSpline->setSpline(sp);
    pNodeSpline->setResolution(10*pos.size());
/*

    //Decal
    Ptr<Universe::NodeGroup > pNodeGroupDecal = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNodeRepresentation->getChildWithTag(tagNodeGroupDecal));

    if (pNodeGroupDecal->getChildCount() < cp.size())
    {
        int32 nbNewDecal = cp.size()-pNodeGroupDecal->getChildCount();
        for (int32 ii=0; ii<nbNewDecal; ++ii)
        {
            pNodeGroupDecal->addChild(_pWorld->createDecal());
        }
    }
    else
    {
        int32 nbDecalToDelete = pNodeGroupDecal->getChildCount()-cp.size();
        for (int32 ii=0; ii<nbDecalToDelete; ++ii)
        {
            pNodeGroupDecal->removeChild(pNodeGroupDecal->getChildren().back());
        }
    }


    for (int32 ii=0; ii<cp.size(); ++ii)
    {
        LM_DEBUG_PTR_CAST<Universe::NodeDecal>(pNodeGroupDecal->getChild(ii))->setBillboard(true);
        LM_DEBUG_PTR_CAST<Universe::NodeDecal>(pNodeGroupDecal->getChild(ii))->setSize(0.1f);
        pNodeGroupDecal->getChild(ii)->setWorldPosition(cp[ii].position);
        pNodeGroupDecal->getChild(ii)->setFixedSize(true);
    }*/
}
//-----------------------------------------------------------------------------
void SplineRepresentation::setRepresentationMode(ECRMode mode)
{
    _mode = mode;

    Ptr<Universe::NodeSpline> pNodeSpline = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSpline));
   // Ptr<Universe::NodeGroup > pNodeGroupDecal = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNodeRepresentation->getChildWithTag(tagNodeGroupDecal));

    if (_mode==OBJECT_REPRESENTATION)
    {
        pNodeSpline->setVisible(true);
   //     pNodeGroupDecal->setVisible(true);
    }
    else
    {
        pNodeSpline->setVisible(true);
     //   pNodeGroupDecal->setVisible(false);
    }
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeGroup>& SplineRepresentation::getNodeRepresentation()
{
    return _pNodeRepresentation;
}
//-----------------------------------------------------------------------------
bool SplineRepresentation::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (pNode==_pNodeRepresentation)    return true;
    return false;
}
//-----------------------------------------------------------------------------
Ptr<IContentRepresentation> SplineRepresentation::clone()
{
    Ptr<SplineRepresentation>   pRet = Ptr<SplineRepresentation>(new SplineRepresentation (*this));

    return pRet;
}
//-----------------------------------------------------------------------------
void SplineRepresentation::setNodeToRepresent(const Ptr<Universe::Node>& pNode)
{
    pNode->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI