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
#ifndef UNIVERSE_TASKSEQUENCE_H_
#define UNIVERSE_TASKSEQUENCE_H_

#include <Core/List.h>
#include <Universe/Tasks/ITask.h>

namespace Universe
{
class World;

/**
 * Cette tâche ne fait rien en elle même mais enchaîne une liste de tâches
 * séquentiellement.
 * 
 * Attention : toutes les tâches sont créées (et donc initialisées) dès le départ, 
 * ce qui limite les possibilités. Par exemple, la deuxième
 * tâche ne peut pas être un déplacement (fiable) vers la position d'un Node si le Node se déplace
 * pendant l'exécution de la première tâche. La deuxième tâche entraînera un déplacement
 * vers la position initiale du Node.
 *
 * On ne peut donc pas utiliser cette tâche comme un substitut de script. Elle permet
 * juste d'enchaîner des tâches simplement.
 */
class LM_API_UNI TaskSequence : public Core::Object, public ITask
{
public:
    TaskSequence(const Ptr<World> & pWorld,
                 const Core::List<Ptr<ITask> > & tasks);
    
    virtual void update(double elapsed);
    virtual void kill();
    
protected:
    virtual bool internalIsFinished() const;
    
    Ptr<World>                _pWorld;
    Core::List<Ptr<ITask> >   _tasks;
    Ptr<ITask>                _pTask;
    int32                     _iNextTask;
};

}

#endif
