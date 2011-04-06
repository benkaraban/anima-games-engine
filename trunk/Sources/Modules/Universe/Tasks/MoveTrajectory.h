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
#ifndef UNIVERSE_MOVETRAJECTORY_H_
#define UNIVERSE_MOVETRAJECTORY_H_

#include <Core/Math/Splines/Trajectory.h>
#include <Universe/Node.h>
#include <Universe/Tasks/ITask.h>

namespace Universe
{
class LM_API_UNI TaskMoveTrajectory : public Core::Object, public ITask
{
public:
    /**
     * Avec ce constructeur, le déplacement prend un temps donné.
     * La vitesse de déplacement est adaptée pour obtenir la durée souhaitée.
     * La position de départ est celle du Node à la création de la tâche.
     *
     * @param pNode Le Node affecté.
     * @param target La position cible, dans le repère du monde.
     * @param duration Durée de la rotation, en secondes.
     * @param func Le type d'interpolation utilisée pour la rotation.
     */
    TaskMoveTrajectory(const Ptr<Node> & pNode,
                       const Core::List<Core::CheckPoint> & points,
                       float durationMul = 1.0,
                       double step = 0.05);

    TaskMoveTrajectory(const Ptr<Node> & pNode,
                       const Core::Trajectory & traj,
                       float durationMul = 1.0);

    TaskMoveTrajectory(const Ptr<Node> & pNode,
                       const Ptr<Node> & pPivot,
                       const Core::Trajectory & traj,
                       float durationMul = 1.0,
                       float key1 = 0.0f,
                       float key2 = 1.0f);

    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<Node>              _pNode;
    Core::Matrix4f         _baseMat;
    Core::Orientationf     _baseOri;
    Core::Trajectory       _trajectory;
    float                  _durationMul;
    double                 _time2;
    double                 _elapsed;
    double                 _duration;
    bool                   _localStart;
    bool                   _reorthoNeeded;
    Core::ERotationAxis    _reorthoAxis;
};

}


#endif /* MOVETRAJECTORY_H_ */
