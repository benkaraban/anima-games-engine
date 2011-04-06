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
#include <EPI/Document/DocumentTest.h>

#include <EPI/Document/Properties/PtyTerrain.moc.h>
#include <EPI/Document/Properties/PtyFog.moc.h>
#include <EPI/Document/Properties/PtySun.moc.h>
#include <EPI/Document/Properties/PtyNodeInstance.h>

#include <EPI/Document/Properties/PtyNodeInstance.h>

#include <Assets/geometricPrimitive.h>
#include <Renderer/Common/Box.h>
#include <Renderer/SM2/MeshSM2.h>

#include <EPI/Document/Properties/PtySun.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
DocumentTest::DocumentTest(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr) :
    DocumentBase(guiGoc, DOC_TEST, pDocRdr)
{/*
    Ptr<Property> pPty (new PtyNodeInstance(_pWorld->createInstance(L"/Tests/sam1.mdl")));
    getPropertyDocumentContent()->addChild(pPty);*/
/*

    _pNodeTerrain = _pWorld->createTerrain(L"/Tests/HeightMap.tmdl");

    _pNodeTerrain->beginMatrixUpdate();
    _pNodeTerrain->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    _pNodeTerrain->endMatrixUpdate();

    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (getPropertyDocumentContent());
    Ptr<PtyTerrain> pPropertyTerrain(new PtyTerrain(_pNodeTerrain, _pWorld->getRessourcesPool(), getDocumentRenderer(), L"/Tests/HeightMap.tmdl", L"Terrain"));
    pPtyContent->addChild(pPropertyTerrain);

    Ptr<PtyFog> pPropertyFog(new PtyFog(_pWorld));
    Ptr<PtySun> pPropertySun(new PtySun(_pWorld));
    pPtyContent->addChild(pPropertyFog);
    pPtyContent->addChild(pPropertySun);
    
    setGenerationProperty(pPropertyTerrain);*/
/*
    Ptr<Universe::NodeInstance> pI1 = _pWorld->createInstance(L"/Tests/sam1.mdl");
    Ptr<Universe::NodeInstance> pI2 = _pWorld->createInstance(L"/Tests/sam1.mdl");

    pI1->beginMatrixUpdate();
    pI1->localPitch(Core::deg2rad(25));
    pI1->localYaw(Core::deg2rad(25));
    pI1->localRoll(Core::deg2rad(25));
    pI1->endMatrixUpdate();

    pI2->beginMatrixUpdate();
    pI2->setLocalPosition(Core::Vector3f(5,0,0));
    pI2->endMatrixUpdate();

    Ptr<PtyNodeInstance> pPtyI1(new PtyNodeInstance(pI1, L"mesh1"));
    Ptr<PtyNodeInstance> pPtyI2(new PtyNodeInstance(pI2, L"mesh2"));

    Ptr<Property>       pPtySun (new PtySun(_pWorld));
    getPropertyDocumentContent()->addChild(pPtyI1);
    getPropertyDocumentContent()->addChild(pPtyI2);
    getPropertyDocumentContent()->addChild(pPtySun);*/
    
}
//-----------------------------------------------------------------------------
DocumentTest::~DocumentTest()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentTest::getDefaultProperty() const
{
    return getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
}  // namespace EPI