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
#include <Core/VFS/VirtualFileSystem.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Universe/World.h>
#include <Universe/Tasks/Wait.h>
#include <Renderer/Stub/RendererStub.h>

using namespace Core;
using namespace Renderer;
using namespace Universe;

void testTaskWait(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Task wait");

    Ptr<ITask> pWaitTask(new TaskWait(2.0));
    pWorld->registerTask(pWaitTask);

    UTEST_EQU(pWaitTask->isFinished(), false);
    pWorld->update(1.0);
    UTEST_EQU(pWaitTask->isFinished(), false);
    pWorld->update(0.9);
    UTEST_EQU(pWaitTask->isFinished(), false);
    UTEST_EQU(pWaitTask.getCount(), 2);

    pWorld->update(1.1);
    UTEST_EQU(pWaitTask->isFinished(), true);
    UTEST_EQU(pWaitTask.getCount(), 1);

    UTEST_END_TEST;
}

void testTasks()
{
    UTEST_PACKAGE("Tasks");

    Ptr<VirtualFileSystem> pVFS(new VFSDirectMapping());
    Ptr<IRenderer> pRenderer(new RendererStub());
    Ptr<World> pWorld(new World(pRenderer, pVFS));

    testTaskWait(pWorld);
}

