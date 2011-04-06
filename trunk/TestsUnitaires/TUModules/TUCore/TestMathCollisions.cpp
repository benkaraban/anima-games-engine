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
#include <Core/Math/Collisions.h>

using namespace Core;

void testCollisions()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Collisions");

    AABoxf box1(Vector3f(3.0f, -1.0f, -1.0f), Vector3f(5.0f, 1.0f, 1.0f));
    AABoxf box2(Vector3f(4.0f, -0.5f,  0.0f), Vector3f(4.0f, 0.5f, 0.0f));

    CollisionShape s1(Sphere(1.0f));
    s1.addAABox(box1);
    s1.addAABox(box2);

    CollisionShape s2(box1);
    s2.addSphere(Sphere(Vector3f(0.0f, 0.0f, 0.0f), 1.0f));
    {
        Core::Matrix4f world1(Core::TRANSLATION, Core::Vector3f(5.9f, 0.0f, 0.0f));
        s1.setMatrices(world1, world1.getInverted());
        UTEST_EQU(s1.collides(s2), true);
    }
    {
        Core::Matrix4f world2(Core::TRANSLATION, Core::Vector3f(6.1f, 0.0f, 0.0f));
        s1.setMatrices(world2, world2.getInverted());
        UTEST_EQU(s1.collides(s2), false);
    }
    {
        Core::Matrix4f world3(Core::TRANSLATION, Core::Vector3f(-5.0f, 0.0f, 0.0f));
        s1.setMatrices(world3, world3.getInverted());
        UTEST_EQU(s1.collides(s2), true);
    }
    {
        Core::Matrix4f world4(Core::TRANSLATION, Core::Vector3f(-5.0f, 2.1f, 0.0f));
        s1.setMatrices(world4, world4.getInverted());
        UTEST_EQU(s1.collides(s2), false);
    }
    UTEST_END_TEST;
}
