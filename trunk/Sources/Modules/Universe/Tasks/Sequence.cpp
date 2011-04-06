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
#include <Universe/Tasks/Sequence.h>
#include <Universe/World.h>

namespace Universe
{
//------------------------------------------------------------------------------
TaskSequence::TaskSequence(const Ptr<World> & pWorld,
                           const Core::List<Ptr<ITask> > & tasks) :
    _pWorld(pWorld),
    _tasks(tasks),
    _iNextTask(0)
{
    LM_ASSERT(pWorld != null);
}
//------------------------------------------------------------------------------
void TaskSequence::update(double elapsed)
{
    if(_tasks.size() >= 1)
    {
        if(_pTask == null) // Démarrage
        {
            _pTask = _tasks[0];
            _pWorld->registerTask(_pTask);
            _iNextTask = 1;
        }
        
        while(_pTask->isFinished() && _iNextTask < (int32)_tasks.size())
        {
            _pTask = _tasks[_iNextTask];
            _pWorld->registerTask(_pTask);
            
            _iNextTask++;
            // La tâche peut terminer de suite
        }
    }
}
//------------------------------------------------------------------------------
bool TaskSequence::internalIsFinished() const
{
    return _tasks.empty() || 
        (_iNextTask == _tasks.size() && _pTask != null && _pTask->isFinished());
}
//------------------------------------------------------------------------------
void TaskSequence::kill()
{
    ITask::kill();

    for(Core::List<Ptr<ITask> >::const_iterator iTask = _tasks.begin(); iTask != _tasks.end(); ++iTask)
        (*iTask)->kill();
}
//------------------------------------------------------------------------------
}
