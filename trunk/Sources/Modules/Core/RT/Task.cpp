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

#include "Task.h"
#include <Core/System/System.h>

namespace Core
{
//-----------------------------------------------------------------------------
Queue<Task * > * Task::sTaskQueue = NULL;
ConditionMutex Task::sMutex;
ConditionVar   Task::sCondition;
volatile bool  Task::sThreadsStarted = false;
//-----------------------------------------------------------------------------
void Task::taskThread()
{
    while(true)
    {
        Task * pTask = NULL;
        {
            ScopedCondLock lock(sMutex);

            while(sTaskQueue->empty())
                sCondition.wait(lock);

            pTask = sTaskQueue->front();
            sTaskQueue->pop();
        }

        pTask->process();
        pTask->_isCompleted = true;
        {
            ScopedCondLock lock(sMutex);
            sCondition.notifyAll();
        }
    }
}
//-----------------------------------------------------------------------------
Task::Task() : _isCompleted(false)
{}
//-----------------------------------------------------------------------------
void Task::waitCompletion()
{
    ScopedCondLock lock(sMutex);
    while(!_isCompleted)
    {
        sCondition.wait(lock);
    }
}
//-----------------------------------------------------------------------------
void Task::startTask(Task * pTask)
{
    //pTask->process();
    //pTask->_isCompleted = true;
    //return;

    ScopedCondLock lock(sMutex);

    if(!sThreadsStarted)
    {
        if(!sThreadsStarted)
        {
            sTaskQueue = new Queue<Task * >();
            for(int32 ii=0; ii < getTaskThreadsCount(); ii++)
            {
                new Thread(taskThread);
            }
            sThreadsStarted = true;
        }
    }
    
    pTask->_isCompleted = false;
    sTaskQueue->push(pTask);
    sCondition.notifyAll();
}
//-----------------------------------------------------------------------------
int32 Task::getTaskThreadsCount()
{
    return Core::System::getCPUCount();
}
//-----------------------------------------------------------------------------
}
