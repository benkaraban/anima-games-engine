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
#ifndef UNIVERSE_ITASK_H_
#define UNIVERSE_ITASK_H_

#include <Core/Standard.h>

namespace Universe
{
LM_ENUM_1(DurationDependant_t, DURATION_DEPENDANT);
LM_ENUM_1(SpeedDependant_t, SPEED_DEPENDANT);

/**
 * La Task est la brique de base qui permet d'animer un World. Toutes les Tasks
 * d'un monde sont mises à jour en parallèle.
 */
class LM_API_UNI ITask
{
public:
    ITask();
    virtual ~ITask() {};

    virtual void update(double elapsed) = 0;
    bool isFinished() const;

    int32 getPriority() const { return _priority; };
    void setPriority(int32 p) { _priority = p; };

    virtual void kill();

protected:
    virtual bool internalIsFinished() const = 0;

    bool    _isAlive;
    int32   _priority;
};

struct ITaskPtrSorter
{
    bool operator () (const Ptr<ITask> & pT1, const Ptr<ITask> & pT2)
    {
        return pT1->getPriority() < pT2->getPriority();
    }
};

}

#endif /* ITASK_H_ */
