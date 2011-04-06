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
#include <UTests.h>
#include <Renderer/Common/Box.h>
#include <Universe/Node.h>
#include <Universe/World.h>

using namespace Core;
using namespace Universe;

//std::cout << Core::toString(pChild2->getLocalMatrix()) << std::endl;
//std::cout << Core::toString(pChild2->getLocalToWorldMatrix()) << std::endl;



void testLife()
{
    Ptr<Node> pParent1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild2(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));

    pParent1->addChild(pChild1);

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Life time");

    UTEST_EQU(pParent1->isAlive(), true);
    UTEST_EQU(pChild1->isAlive(),  true);
    UTEST_EQU(pChild2->isAlive(),  true);

    pChild2->kill();
    UTEST_EQU(pParent1->isAlive(), true);
    UTEST_EQU(pChild1->isAlive(),  true);
    UTEST_EQU(pChild2->isAlive(),  false);

    pParent1->kill();
    UTEST_EQU(pParent1->isAlive(), false);
    UTEST_EQU(pChild1->isAlive(),  false);
    UTEST_EQU(pChild2->isAlive(),  false);

    UTEST_END_TEST;
}

void testPosition()
{
    Ptr<Node> pParent1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Quaternionf defaultOrientation(IDENTITY);

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Local position");

    UTEST_EQU(pParent1->getLocalPosition(), Vector3f(0.0, 0.0, 0.0));
    UTEST_EQU(pParent1->getWorldPosition(), Vector3f(0.0, 0.0, 0.0));
    UTEST_EQU(pParent1->getLocalOrient(), defaultOrientation);

    pParent1->beginMatrixUpdate();
    pParent1->setLocalPosition(Vector3f(1.0, 2.0, 3.0));
    pParent1->endMatrixUpdate();
    UTEST_EQU(pParent1->getLocalPosition(), Vector3f(1.0, 2.0, 3.0));
    UTEST_EQU(pParent1->getWorldPosition(), Vector3f(1.0, 2.0, 3.0));
    UTEST_EQU(pParent1->getLocalOrient(), defaultOrientation);

    AABoxf box = pParent1->getBoundingBox();
    UTEST_EQU(pParent1->getWorldPosition(), box.min);
    UTEST_EQU(pParent1->getWorldPosition(), box.max);

    UTEST_END_TEST;

    pParent1->addChild(pChild1);
    pParent1->beginMatrixUpdate();
    pParent1->setLocalPosition(Vector3f(100.0, 200.0, 0.0));
    pParent1->localRoll(f_PI_DIV_2);
    pParent1->endMatrixUpdate();

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"World position");

    pChild1->beginMatrixUpdate();
    pChild1->setWorldPosition(Vector3f(110.0, 220.0, 0.0));
    pChild1->endMatrixUpdate();
    UTEST_EQU(pChild1->getLocalPosition().isEquivalent(Vector3f(20.0, -10.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(110.0, 220.0, 0.0)), true);

    AABoxf box = pChild1->getBoundingBox();
    UTEST_EQU(pChild1->getWorldPosition(), box.min);
    UTEST_EQU(pChild1->getWorldPosition(), box.max);

    UTEST_END_TEST;
}

void testScale()
{
    Ptr<Node> pParent1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Scale");

    pParent1->setLocalScale(Vector3f(1.0, 2.0, 3.0));

    pParent1->addChild(pChild1);
    pChild1->setLocalPosition(Vector3f(100.0, 100.0, 100.0));

    UTEST_EQU(pChild1->getLocalPosition(), Vector3f(100.0, 100.0, 100.0));
    UTEST_EQU(pChild1->getWorldPosition(), Vector3f(100.0, 200.0, 300.0));

    UTEST_END_TEST;
}

void testSetDirection()
{
    Ptr<Node> pParent1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild2(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));

    pParent1->addChild(pChild1);
    pParent1->addChild(pChild2);

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Set local direction");

    pParent1->beginMatrixUpdate();
    pParent1->setWorldPosition(Vector3f(100.0, 200.0, 0.0));
    pParent1->setLocalDirection(Vector3f(1.0, 0.0, 0.0));
    pParent1->endMatrixUpdate();
    //      Parent1
    //       y| /x
    //        |/
    //        +----- z
    //
    //
    UTEST_EQU(pParent1->getLocalDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pParent1->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pParent1->getLocalMatrix().getXVector().isEquivalent(Vector3f(0.0, 0.0, -1.0)), true);
    UTEST_EQU(pParent1->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);

    pParent1->setUpVector(Vector3f(0.0, 0.0, -1.0));
    pParent1->setLocalDirection(Vector3f(0.0, 1.0, 0.0));
    //      Parent1
    //       z| /y
    //        |/
    //        +----- x
    //
    //
    UTEST_EQU(pParent1->getLocalDirection().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);
    UTEST_EQU(pParent1->getWorldDirection().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);
    UTEST_EQU(pParent1->getLocalMatrix().getXVector().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pParent1->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, 0.0, -1.0)), true);

    pChild1->beginMatrixUpdate();
    pChild1->setLocalDirection(Vector3f(0.0, 0.0, -1.0));
    pChild1->endMatrixUpdate();
    //      Parent1        Child1 local     Child1 world
    //       z| /y             y| /z               /y
    //        |/                |/                /
    //        +----- x   x -----+         x -----+
    //                                           |
    //                                           | z
    UTEST_EQU(pChild1->getLocalDirection().isEquivalent(Vector3f(0.0, 0.0, -1.0)), true);
    UTEST_EQU(pChild1->getWorldDirection().isEquivalent(Vector3f(0.0, -1.0, 0.0)), true);
    UTEST_EQU(pChild1->getLocalMatrix().getXVector().isEquivalent(Vector3f(-1.0, 0.0, 0.0)), true);
    UTEST_EQU(pChild1->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Set world direction");

    pChild2->setWorldDirection(Vector3f(1.0, 0.0, 0.0));
    //      Parent1        Child1 local     Child1 world
    //       z| /y             y| /x               /y
    //        |/                |/                /
    //        +----- x          +----- z         +----- z
    //                                           |
    //                                           |x
    UTEST_EQU(pChild2->getLocalDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pChild2->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pChild2->getLocalMatrix().getXVector().isEquivalent(Vector3f(0.0, 0.0, -1.0)), true);
    UTEST_EQU(pChild2->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);

    UTEST_END_TEST;
}

void testLookAt()
{
    Ptr<Node> pParent1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));

    pParent1->addChild(pChild1);

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Local look at");

    pParent1->setLocalPosition(Vector3f(100.0, 200.0, 0.0));

    pParent1->beginMatrixUpdate();
    pParent1->localLookAt(Vector3f(200.0, 200.0, 0.0));
    pParent1->endMatrixUpdate();
    //      Parent1
    //       y| /x
    //        |/
    //        +----- z
    //
    //
    UTEST_EQU(pParent1->getLocalDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pParent1->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"World look at");

    pChild1->beginMatrixUpdate();
    pChild1->setLocalPosition(Vector3f(10.0, 0.0, 0.0));
    pChild1->endMatrixUpdate();

    pChild1->beginMatrixUpdate();
    pChild1->worldLookAt(Vector3f(100.0, 200.0, 0.0));
    pChild1->endMatrixUpdate();

    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(100.0, 200.0, -10.0)), true);
    UTEST_EQU(pChild1->getLocalDirection().isEquivalent(Vector3f(-1.0, 0.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldDirection().isEquivalent(Vector3f(0.0, 0.0, 1.0)), true);

    UTEST_END_TEST;
}

void testAddRemoveSamePos()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Add/remove child same pos");

    Ptr<Node> pParent1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));

    pParent1->setLocalPosition(Core::Vector3f(0.0f, 1.0f, 0.0f));
    pChild1->setLocalPosition(Core::Vector3f(2.0f, 0.0f, 0.0f));
    pChild1->setLocalDirection(Core::Vector3f(1.0f, 0.0f, 0.0f));

    pParent1->addChild(pChild1, true);
    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(2.0, 0.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    pParent1->removeChild(pChild1, true);

    pParent1->localYaw(1.0);
    pParent1->localPitch(2.0);

    pParent1->addChild(pChild1, true);
    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(2.0, 0.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    pParent1->removeChild(pChild1, true);

    pParent1->setLocalScale(Core::Vector3f(1.5f, 0.5f, 2.0f));

    pParent1->addChild(pChild1, true);
    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(2.0, 0.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    pParent1->removeChild(pChild1, true);

    UTEST_END_TEST;

}

void testHierarchy()
{
    Ptr<Node> pParent1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pChild2(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Add child");

    pParent1->addChild(pChild2);

    pChild2->beginMatrixUpdate();
    pChild2->setLocalPosition(Vector3f(10.0, 20.0, 0.0));
    pChild2->endMatrixUpdate();

    pParent1->setLocalPosition(Vector3f(100.0, 200.0, 0.0));
    pParent1->localRoll(f_PI_DIV_2);
    pParent1->addChild(pChild1, false);

    pChild1->beginMatrixUpdate();
    pChild1->setLocalPosition(Vector3f(10.0, 20.0, 0.0));
    pChild1->localYaw(f_PI_DIV_2);
    pChild1->endMatrixUpdate();

    //      Parent1          Child1 local       Child1 world
    //        |x                 y| /x               z | /x
    //        |                   |/                   |/
    // y -----+                   +----- z      y -----+
    //       /
    //      / z
    UTEST_EQU(pParent1->getLocalPosition().isEquivalent(Vector3f(100.0, 200.0, 0.0)), true);
    UTEST_EQU(pParent1->getWorldPosition().isEquivalent(Vector3f(100.0, 200.0, 0.0)), true);
    UTEST_EQU(pParent1->getLocalDirection().isEquivalent(Vector3f(0.0, 0.0, 1.0)), true);
    UTEST_EQU(pParent1->getWorldDirection().isEquivalent(Vector3f(0.0, 0.0, 1.0)), true);

    // Child
    UTEST_EQU(pChild1->getLocalPosition().isEquivalent(Vector3f(10.0, 20.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(80.0, 210.0, 0.0)), true);

    UTEST_EQU(pChild1->getLocalDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldDirection().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);

    UTEST_EQU(pChild2->getLocalPosition().isEquivalent(Vector3f(10.0, 20.0, 0.0)), true);
    UTEST_EQU(pChild2->getWorldPosition().isEquivalent(Vector3f(80.0, 210.0, 0.0)), true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Update child");

    pChild1->beginMatrixUpdate();
    pChild1->setLocalPosition(Vector3f(20.0, 30.0, 0.0));
    pChild1->endMatrixUpdate();

    UTEST_EQU(pChild1->getLocalPosition().isEquivalent(Vector3f(20.0, 30.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(70.0, 220.0, 0.0)), true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Update father");

    pParent1->beginMatrixUpdate();
    pParent1->setLocalPosition(Vector3f(200.0, 100.0, 0.0));
    pParent1->endMatrixUpdate();

    UTEST_EQU(pParent1->getLocalPosition().isEquivalent(Vector3f(200.0, 100.0, 0.0)), true);
    UTEST_EQU(pParent1->getWorldPosition().isEquivalent(Vector3f(200.0, 100.0, 0.0)), true);
    UTEST_EQU(pChild1->getLocalPosition().isEquivalent(Vector3f(20.0, 30.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(170.0, 120.0, 0.0)), true);
    UTEST_EQU(pChild2->getLocalPosition().isEquivalent(Vector3f(10.0, 20.0, 0.0)), true);
    UTEST_EQU(pChild2->getWorldPosition().isEquivalent(Vector3f(180.0, 110.0, 0.0)), true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Remove child");

    pParent1->removeChild(pChild1, true);
    pParent1->removeChild(pChild2, false);

    UTEST_EQU(pChild1->getLocalPosition().isEquivalent(Vector3f(170.0, 120.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(170.0, 120.0, 0.0)), true);
    UTEST_EQU(pChild2->getLocalPosition().isEquivalent(Vector3f(10.0, 20.0, 0.0)), true);
    UTEST_EQU(pChild2->getWorldPosition().isEquivalent(Vector3f(10.0, 20.0, 0.0)), true);

    pParent1->beginMatrixUpdate();
    pParent1->setLocalPosition(Vector3f(300.0, 100.0, 0.0));
    pParent1->endMatrixUpdate();

    UTEST_EQU(pChild1->getLocalPosition().isEquivalent(Vector3f(170.0, 120.0, 0.0)), true);
    UTEST_EQU(pChild1->getWorldPosition().isEquivalent(Vector3f(170.0, 120.0, 0.0)), true);
    UTEST_EQU(pChild2->getLocalPosition().isEquivalent(Vector3f(10.0, 20.0, 0.0)), true);
    UTEST_EQU(pChild2->getWorldPosition().isEquivalent(Vector3f(10.0, 20.0, 0.0)), true);

    UTEST_END_TEST;
}

void testLocalTurn()
{
    Ptr<Node> pNode1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Quaternionf defaultOrientation(IDENTITY);

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Local turn");
    //     | y
    //     |
    //     +----- x
    //    /
    //   / z
    UTEST_EQU(pNode1->getLocalDirection(), Vector3f(0.0, 0.0, 1.0));
    UTEST_EQU(pNode1->getWorldDirection(), Vector3f(0.0, 0.0, 1.0));
    UTEST_EQU(pNode1->getLocalMatrix().getXVector(), Vector3f(1.0, 0.0, 0.0));
    UTEST_EQU(pNode1->getLocalMatrix().getYVector(), Vector3f(0.0, 1.0, 0.0));
    UTEST_EQU(pNode1->getLocalOrient(), defaultOrientation);

    pNode1->beginMatrixUpdate();
    pNode1->localPitch(f_PI_DIV_2);
    pNode1->endMatrixUpdate();
    //
    //
    //     +----- x
    //    /|
    //   /y|z
    UTEST_NOT_EQU(pNode1->getLocalOrient(), defaultOrientation);
    UTEST_EQU(pNode1->getLocalDirection().isEquivalent(Vector3f(0.0, -1.0, 0.0)), true);
    UTEST_EQU(pNode1->getWorldDirection().isEquivalent(Vector3f(0.0, -1.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getXVector().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, 0.0, 1.0)), true);

    pNode1->localYaw(f_PI_DIV_2);
    //     | x
    //     |
    //     +----- z
    //    /
    //   / y
    UTEST_EQU(pNode1->getLocalDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getXVector().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, 0.0, 1.0)), true);

    pNode1->beginMatrixUpdate();
    pNode1->localRoll(f_PI_DIV_2);
    pNode1->endMatrixUpdate();
    //
    //
    //     +----- z
    //    /|
    //   /x|y
    UTEST_EQU(pNode1->getLocalDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getXVector().isEquivalent(Vector3f(0.0, 0.0, 1.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, -1.0, 0.0)), true);

    pNode1->beginMatrixUpdate();
    pNode1->localPitch(f_PI_DIV_2);
    pNode1->endMatrixUpdate();
    //     | z
    //     |
    //     +----- y
    //    /
    //   / x
    UTEST_EQU(pNode1->getLocalDirection().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);
    UTEST_EQU(pNode1->getWorldDirection().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getXVector().isEquivalent(Vector3f(0.0, 0.0, 1.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getYVector().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);

    UTEST_END_TEST;
}

void testWorldTurn()
{
    Ptr<Node> pNode1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Quaternionf defaultOrientation(IDENTITY);

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"World turn");
    //     | y
    //     |
    //     +----- x
    //    /
    //   / z
    UTEST_EQU(pNode1->getLocalDirection(), Vector3f(0.0, 0.0, 1.0));
    UTEST_EQU(pNode1->getWorldDirection(), Vector3f(0.0, 0.0, 1.0));
    UTEST_EQU(pNode1->getLocalMatrix().getXVector(), Vector3f(1.0, 0.0, 0.0));
    UTEST_EQU(pNode1->getLocalMatrix().getYVector(), Vector3f(0.0, 1.0, 0.0));
    UTEST_EQU(pNode1->getLocalOrient(), defaultOrientation);

    pNode1->beginMatrixUpdate();
    pNode1->worldPitch(f_PI_DIV_2);
    pNode1->endMatrixUpdate();
    //
    //
    //     +----- x
    //    /|
    //   /y|z
    UTEST_NOT_EQU(pNode1->getLocalOrient(), defaultOrientation);
    UTEST_EQU(pNode1->getLocalDirection().isEquivalent(Vector3f(0.0, -1.0, 0.0)), true);
    UTEST_EQU(pNode1->getWorldDirection().isEquivalent(Vector3f(0.0, -1.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getXVector().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, 0.0, 1.0)), true);

    pNode1->beginMatrixUpdate();
    pNode1->worldYaw(f_PI_DIV_2);
    pNode1->endMatrixUpdate();
    //       /x
    //      /
    //     +----- y
    //     |
    //     |z
    UTEST_EQU(pNode1->getLocalDirection().isEquivalent(Vector3f(0.0, -1.0, 0.0)), true);
    UTEST_EQU(pNode1->getWorldDirection().isEquivalent(Vector3f(0.0, -1.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getXVector().isEquivalent(Vector3f(0.0, 0.0, -1.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getYVector().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);

    pNode1->beginMatrixUpdate();
    pNode1->worldRoll(f_PI_DIV_2);
    pNode1->endMatrixUpdate();
    //    y| /x
    //     |/
    //     +----- z
    //
    //
    UTEST_EQU(pNode1->getLocalDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getXVector().isEquivalent(Vector3f(0.0, 0.0, -1.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);

    pNode1->beginMatrixUpdate();
    pNode1->worldPitch(f_PI_DIV_2);
    pNode1->endMatrixUpdate();
    //     | x
    //     |
    //     +----- z
    //    /
    //   / y
    UTEST_EQU(pNode1->getLocalDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getWorldDirection().isEquivalent(Vector3f(1.0, 0.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getXVector().isEquivalent(Vector3f(0.0, 1.0, 0.0)), true);
    UTEST_EQU(pNode1->getLocalMatrix().getYVector().isEquivalent(Vector3f(0.0, 0.0, 1.0)), true);

    UTEST_END_TEST;
}

void testGhosts()
{
    Ptr<Node> pNode1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pNode2(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Quaternionf defaultOrientation(IDENTITY);

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Ghost nodes");

    pNode1->setLocalPosition(Core::Vector3f(1.0f, 0.0f, 0.0f));
    pNode2->setLocalPosition(Core::Vector3f(0.0f, 2.0f, 0.0f));

    pNode1->addGhostChild(pNode2);
    UTEST_EQU(pNode2->getWorldPosition().isEquivalent(Vector3f(1.0f, 2.0f, 0.0f)), true);

    pNode1->setLocalPosition(Core::Vector3f(3.0f, 0.0f, 0.0f));
    UTEST_EQU(pNode2->getWorldPosition().isEquivalent(Vector3f(3.0f, 2.0f, 0.0f)), true);

    pNode1->removeGhostChild(pNode2, true);
    UTEST_EQU(pNode2->getWorldPosition().isEquivalent(Vector3f(3.0f, 2.0f, 0.0f)), true);

    pNode1->setLocalPosition(Core::Vector3f(1.0f, 0.0f, 0.0f));
    UTEST_EQU(pNode2->getWorldPosition().isEquivalent(Vector3f(3.0f, 2.0f, 0.0f)), true);

    pNode1->addGhostChild(pNode2);
    UTEST_EQU(pNode2->getWorldPosition().isEquivalent(Vector3f(4.0f, 2.0f, 0.0f)), true);

    pNode1->removeGhostChild(pNode2, false);
    UTEST_EQU(pNode2->getWorldPosition().isEquivalent(Vector3f(3.0f, 2.0f, 0.0f)), true);

    pNode1->addGhostChild(pNode2);
    UTEST_EQU(pNode2->getWorldPosition().isEquivalent(Vector3f(4.0f, 2.0f, 0.0f)), true);

    pNode1->kill();
    UTEST_EQU(pNode1->isAlive(), false);
    UTEST_EQU(pNode2->isAlive(), true);

    UTEST_END_TEST;
}

void testAddChild()
{
    Ptr<Node> pNode1(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pNode2(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pNode3(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pNode4(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));
    Ptr<Node> pNodeParent(new Node(Ptr<Renderer::IAABox>(new Renderer::AABox()), NULL));

    pNodeParent->addChild(pNode1);
    pNodeParent->addChild(pNode2);
    pNodeParent->addChild(pNode3);

    int32 position = 0;

    position = 0;
    pNodeParent->addChild(pNode4, false, position);
    UTEST_EQU(pNodeParent->getChildren()[position]==pNode4, true);
    pNodeParent->removeChild(pNode4);

    position = 1;
    pNodeParent->addChild(pNode4, false, position);
    UTEST_EQU(pNodeParent->getChildren()[position]==pNode4, true);
    pNodeParent->removeChild(pNode4);

    position = 2;
    pNodeParent->addChild(pNode4, false, position);
    UTEST_EQU(pNodeParent->getChildren()[position]==pNode4, true);
    pNodeParent->removeChild(pNode4);

    //push_back
    position = 3;
    pNodeParent->addChild(pNode4, false, position);
    UTEST_EQU(pNodeParent->getChildren()[position]==pNode4, true);
    pNodeParent->removeChild(pNode4);
}

void testNodes()
{
    UTEST_PACKAGE("Nodes");
    testLife();
    testPosition();
    testScale();
    testLocalTurn();
    testWorldTurn();
    testSetDirection();
    testLookAt();
    testHierarchy();
    testAddRemoveSamePos();
    testGhosts();
    testAddChild();
}
