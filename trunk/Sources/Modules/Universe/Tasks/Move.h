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
#ifndef UNIVERSE_TASKMOVE_H_
#define UNIVERSE_TASKMOVE_H_

#include <Core/Math/Interpolate.h>
#include <Universe/Node.h>
#include <Universe/Tasks/ITask.h>

namespace Universe
{
/**
 * Déplace un Node vers un point.
 *
 * La tâche se termine à la fin du déplacement.
 */
class LM_API_UNI TaskMove : public Core::Object, public ITask
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
    TaskMove(DurationDependant_t mode,
             const Ptr<Node> & pNode,
             const Core::Vector3f & target,
             float duration,
             Core::EInterpolateFunc func = Core::INTER_LINEAR);
    
    /**
     * Avec ce constructeur, le déplacement prend un temps donné.
     * La vitesse de déplacement est adaptée pour obtenir la durée souhaitée.
     *
     * @param pNode Le Node affecté.
     * @param from La position de départ, dans le repère du monde.
     * @param to La position cible, dans le repère du monde.
     * @param duration Durée de la rotation, en secondes.
     * @param func Le type d'interpolation utilisée pour la rotation.
     */
    TaskMove(DurationDependant_t mode,
             const Ptr<Node> & pNode,
             const Core::Vector3f & from,
             const Core::Vector3f & target,
             float duration,
             Core::EInterpolateFunc func = Core::INTER_LINEAR);

    /**
     * Avec ce constructeur, le déplacement s'effectue à une vitesse donnée.
     * La durée du mouvement dépend de la vitesse de déplacement.
     * La position de départ est celle du Node à la création de la tâche.
     *
     * @param pNode Le Node affecté.
     * @param target La position cible, dans le repère du monde.
     * @param speed Vitesse de déplacement, en unités par secondes.
     * @param func Le type d'interpolation utilisée pour la rotation.
     */
    TaskMove(SpeedDependant_t mode,
             const Ptr<Node> & pNode,
             const Core::Vector3f & target,
             float speed,
             Core::EInterpolateFunc func = Core::INTER_LINEAR);
    
    /**
     * Avec ce constructeur, le déplacement s'effectue à une vitesse donnée.
     * La durée du mouvement dépend de la vitesse de déplacement.
     *
     * @param pNode Le Node affecté.
     * @param from La position de départ, dans le repère du monde.
     * @param to La position cible, dans le repère du monde.
     * @param speed Vitesse de déplacement, en unités par secondes.
     * @param func Le type d'interpolation utilisée pour la rotation.
     */
    TaskMove(SpeedDependant_t mode,
             const Ptr<Node> & pNode,
             const Core::Vector3f & from,
             const Core::Vector3f & to,
             float speed,
             Core::EInterpolateFunc func = Core::INTER_LINEAR);

    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<Node>       _pNode;
    Core::Vector3f  _startPoint;
    Core::Vector3f  _endPoint;
    double          _elapsed;
    double          _duration;
    Core::EInterpolateFunc _func;
};

class LM_API_UNI TaskMoveORPO : public Core::Object, public ITask
{
public:
    TaskMoveORPO(const Ptr<Node> & pNode,
                 const Core::Vector3f & position,
                 const Core::Quaternionf & orient,
                 float duration,
                 Core::EInterpolateFunc func = Core::INTER_LINEAR);

    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<Node>              _pNode;
    double                 _elapsed;
    double                 _duration;
    Core::EInterpolateFunc _func;
    Core::Vector3f         _p1;
    Core::Vector3f         _p2;
    Core::Quaternionf      _q1;
    Core::Quaternionf      _q2;
};

}

#endif
