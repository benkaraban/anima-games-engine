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
#include "CameraRepresentation.h"

#include <Universe/World.h>
#include <EPI/Document/Properties/PtyNodeCamera.h>
#include <Core/Math/Splines/CatmullRom.h>
#include <Core/List.h>


namespace EPI
{
const String tagNodeDecalForSelection       = L"CameraDecalSelection";
const String tagNodeSplineForFrustrumBottom = L"CameraSplineFrustrumBottom";
const String tagNodeSplineForFrustrumTop    = L"CameraSplineFrustrumTop";
const String tagNodeSplineForFrustrumFar    = L"CameraSplineFrustrumFar";
const String tagNodeSplineForFrustrumNear1   = L"CameraSplineFrustrumNear1";
const String tagNodeSplineForFrustrumNear2   = L"CameraSplineFrustrumNear2";


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Ptr<Universe::NodeGroup> createCameraUniverseRepresentation(const Ptr<Universe::World>& pWorld)
{
    Ptr<Universe::NodeGroup> pRet = pWorld->createGroup();

    Ptr<Universe::NodeDecal> pNodeDecal = pWorld->createDecal();
        pNodeDecal->setTexture(L"/EpiData/iconCamera.tex");
        pNodeDecal->addTag(tagNodeDecalForSelection);
        pNodeDecal->setBillboard(true);
       // pNodeDecal->setFixedSize(true, 0.03);

    Ptr<Universe::NodeSpline> pNodeSplineBottom = pWorld->createSpline();
    Ptr<Universe::NodeSpline> pNodeSplineTop    = pWorld->createSpline();
    Ptr<Universe::NodeSpline> pNodeSplineFar    = pWorld->createSpline();
    Ptr<Universe::NodeSpline> pNodeSplineNear1  = pWorld->createSpline();
    Ptr<Universe::NodeSpline> pNodeSplineNear2  = pWorld->createSpline();
        pNodeSplineBottom->addTag(tagNodeSplineForFrustrumBottom);
        pNodeSplineTop->addTag(tagNodeSplineForFrustrumTop);
        pNodeSplineFar->addTag(tagNodeSplineForFrustrumFar);
        pNodeSplineNear1->addTag(tagNodeSplineForFrustrumNear1);
        pNodeSplineNear2->addTag(tagNodeSplineForFrustrumNear2);

    pRet->addChild(pNodeDecal);
    pRet->addChild(pNodeSplineBottom);
    pRet->addChild(pNodeSplineTop);
    pRet->addChild(pNodeSplineFar);
    pRet->addChild(pNodeSplineNear1);
    pRet->addChild(pNodeSplineNear2);

    return pRet;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CameraRepresentation::CameraRepresentation(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNodeToRepresent):
    IContentRepresentation(),
    _mode(SELECTION_REPRESENTATION),
    _pWorld(pWorld)
{
    _pNodeRepresentation = createCameraUniverseRepresentation(_pWorld);

    pNodeToRepresent->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
CameraRepresentation::CameraRepresentation(const CameraRepresentation& other):
    IContentRepresentation(other),
    _pWorld(other._pWorld)
{
    _pNodeRepresentation = createCameraUniverseRepresentation(_pWorld);
}
//-----------------------------------------------------------------------------
CameraRepresentation::~CameraRepresentation()
{
    _pNodeRepresentation->kill();
}
//-----------------------------------------------------------------------------
void CameraRepresentation::updateObjectRepresentation(const Property& pPty)
{
    const PtyNodeCamera& pPtyCam = static_cast<const PtyNodeCamera&>(pPty);

    Core::Vector3f  center(Core::ZERO);
    Core::Vector3f  ptTL, ptTR, ptBL, ptBR; //Far et Frustrum
    Core::Vector3f  ptNearTL, ptNearTR, ptNearBL, ptNearBR;

    float near = pPtyCam.getNear();
    float ratio = pPtyCam.getRatio();
    float far  = pPtyCam.getFar();
    float fov  = pPtyCam.getFov();
    float invRatio = 1./ratio;

    float lenght = Core::L_tan(fov*0.5);

    float radiusFar  = lenght * far;
    float radiusNear = lenght * near;

    float _nearHeight = near * float(tan( fov / 2.f ));
    float _farHeight    = far * float(tan( fov / 2.f ));

    Core::Matrix4f camMatWorld(Core::IDENTITY);

    ptNearTL = camMatWorld.apply(Core::Vector3f (-1*_nearHeight*ratio, +1*_nearHeight, near));
    ptNearTR = camMatWorld.apply(Core::Vector3f (+1*_nearHeight*ratio, +1*_nearHeight, near));
    ptNearBL = camMatWorld.apply(Core::Vector3f (-1*_nearHeight*ratio, -1*_nearHeight, near));
    ptNearBR = camMatWorld.apply(Core::Vector3f (+1*_nearHeight*ratio, -1*_nearHeight, near));

    ptTL = camMatWorld.apply(Core::Vector3f (-1*_farHeight*ratio, +1*_farHeight, far));
    ptTR = camMatWorld.apply(Core::Vector3f (+1*_farHeight*ratio, +1*_farHeight, far));
    ptBL = camMatWorld.apply(Core::Vector3f (-1*_farHeight*ratio, -1*_farHeight, far));
    ptBR = camMatWorld.apply(Core::Vector3f (+1*_farHeight*ratio, -1*_farHeight, far));

    Core::CRSpline splineBottom;
    Core::List<Core::Vector3f>& ptsBottom = splineBottom.getPoints();
        ptsBottom.push_back(center);
        ptsBottom.push_back(center);
        ptsBottom.push_back(center);
        ptsBottom.push_back(ptBL);
        ptsBottom.push_back(ptBL);
        ptsBottom.push_back(ptBL);
        ptsBottom.push_back(ptBR);
        ptsBottom.push_back(ptBR);
        ptsBottom.push_back(ptBR);
        ptsBottom.push_back(center);
        ptsBottom.push_back(center);
        ptsBottom.push_back(center);

    Core::CRSpline splineTop;
    Core::List<Core::Vector3f>& ptsTop = splineTop.getPoints();
        ptsTop.push_back(center);
        ptsTop.push_back(center);
        ptsTop.push_back(center);
        ptsTop.push_back(ptTL);
        ptsTop.push_back(ptTL);
        ptsTop.push_back(ptTL);
        ptsTop.push_back(ptTR);
        ptsTop.push_back(ptTR);
        ptsTop.push_back(ptTR);
        ptsTop.push_back(center);
        ptsTop.push_back(center);
        ptsTop.push_back(center);

    Core::CRSpline splineFar;
    Core::List<Core::Vector3f>& ptsFar = splineFar.getPoints();
        ptsFar.push_back(ptTR);
        ptsFar.push_back(ptTR);
        ptsFar.push_back(ptTR);
        ptsFar.push_back(ptBR);
        ptsFar.push_back(ptBR);
        ptsFar.push_back(ptBR);
        ptsFar.push_back(ptBL);
        ptsFar.push_back(ptBL);
        ptsFar.push_back(ptBL);
        ptsFar.push_back(ptTL);
        ptsFar.push_back(ptTL);
        ptsFar.push_back(ptTL);

    Core::CRSpline splineNear1;
    Core::List<Core::Vector3f>& ptsNear1 = splineNear1.getPoints();
        ptsNear1.push_back(ptNearTL);
        ptsNear1.push_back(ptNearTL);
        ptsNear1.push_back(ptNearTL);
        ptsNear1.push_back(ptNearTR);
        ptsNear1.push_back(ptNearTR);
        ptsNear1.push_back(ptNearTR);
        ptsNear1.push_back(ptNearBR);
        ptsNear1.push_back(ptNearBR);
        ptsNear1.push_back(ptNearBR);

    Core::CRSpline splineNear2;
    Core::List<Core::Vector3f>& ptsNear2 = splineNear2.getPoints();
        ptsNear2.push_back(ptNearTL);
        ptsNear2.push_back(ptNearTL);
        ptsNear2.push_back(ptNearTL);
        ptsNear2.push_back(ptNearBL);
        ptsNear2.push_back(ptNearBL);
        ptsNear2.push_back(ptNearBL);
        ptsNear2.push_back(ptNearBR);
        ptsNear2.push_back(ptNearBR);
        ptsNear2.push_back(ptNearBR);


    const Core::Vector4f color (0.45f, 0.45f, 0.65f, 1.f);

    Ptr<Universe::NodeSpline> pNodeSplineBottom = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumBottom));
        pNodeSplineBottom->setSpline(splineBottom);
        pNodeSplineBottom->setColor(color);
        pNodeSplineBottom->setResolution(10);

    Ptr<Universe::NodeSpline> pNodeSplineTop = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumTop));
        pNodeSplineTop->setSpline(splineTop);
        pNodeSplineTop->setColor(color);
        pNodeSplineTop->setResolution(10);

    Ptr<Universe::NodeSpline> pNodeSplineFar = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumFar));
        pNodeSplineFar->setSpline(splineFar);
        pNodeSplineFar->setColor(color);
        pNodeSplineFar->setResolution(10);

    Ptr<Universe::NodeSpline> pNodeSplineNear1 = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumNear1));
        pNodeSplineNear1->setSpline(splineNear1);
        pNodeSplineNear1->setColor(color);
        pNodeSplineNear1->setResolution(3);

    Ptr<Universe::NodeSpline> pNodeSplineNear2 = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumNear2));
        pNodeSplineNear2->setSpline(splineNear2);
        pNodeSplineNear2->setColor(color);
        pNodeSplineNear2->setResolution(3);
}
//-----------------------------------------------------------------------------
void CameraRepresentation::setRepresentationMode(ECRMode mode)
{
    _mode = mode;

    Ptr<Universe::NodeSpline> pNodeSplineBottom = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumBottom));
    Ptr<Universe::NodeSpline> pNodeSplineTop = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumTop));
    Ptr<Universe::NodeSpline> pNodeSplineFar = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumFar));
    Ptr<Universe::NodeSpline> pNodeSplineNear1 = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumNear1));
    Ptr<Universe::NodeSpline> pNodeSplineNear2 = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(_pNodeRepresentation->getChildWithTag(tagNodeSplineForFrustrumNear2));

    if (_mode==OBJECT_REPRESENTATION)
    {
        pNodeSplineBottom->setVisible(true);
        pNodeSplineTop->setVisible(true);
        pNodeSplineFar->setVisible(true);
        pNodeSplineNear1->setVisible(true);
        pNodeSplineNear2->setVisible(true);
    }
    else
    {
        pNodeSplineBottom->setVisible(false);
        pNodeSplineTop->setVisible(false);
        pNodeSplineFar->setVisible(false);
        pNodeSplineNear1->setVisible(false);
        pNodeSplineNear2->setVisible(false);
    }
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeGroup>& CameraRepresentation::getNodeRepresentation()
{
    return _pNodeRepresentation;
}
//-----------------------------------------------------------------------------
bool CameraRepresentation::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (pNode==_pNodeRepresentation)    return true;
    return false;
}
//-----------------------------------------------------------------------------
bool CameraRepresentation::isIntersecting (const Core::Rayf& ray, float& distance)
{
    if (isVisible==false)   return false;

    Ptr<Universe::Node> pNode = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagNodeDecalForSelection));
    return pNode->isIntersecting(ray, distance);
}
//-----------------------------------------------------------------------------
Ptr<IContentRepresentation> CameraRepresentation::clone()
{
    Ptr<CameraRepresentation>   pRet = Ptr<CameraRepresentation>(new CameraRepresentation (*this));

    return pRet;
}
//-----------------------------------------------------------------------------
void CameraRepresentation::setNodeToRepresent(const Ptr<Universe::Node>& pNode)
{
    pNode->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
void CameraRepresentation::setVisible(bool flag)
{
    IContentRepresentation::setVisible(flag);

    if (flag == true)
    {
        setRepresentationMode(_mode);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI