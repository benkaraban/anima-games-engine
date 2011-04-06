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
#ifndef UNIVERSE_TASKLOOKAT_H_
#define UNIVERSE_TASKLOOKAT_H_

#include <Core/Math/Interpolate.h>
#include <Universe/Node.h>
#include <Universe/Tasks/ITask.h>

#include <Core/Math/MathTools.h>

namespace Universe
{
LM_ENUM_2(ETaskMoveMode, 
          TM_DURATION_DEPENDANT,
          TM_SPEED_DEPENDANT);

/**
 * Fait pivoter un Node de manière à ce qu'il regarde vers un point donné.
 * La rotation est définie au lancement de la tâche. Elle sera donc incohérente
 * si le Node se déplace pendant la rotation.
 *
 * La tâche se termine quand le Node est tourné vers ce point.
 */
class LM_API_UNI TaskLookAt : public Core::Object, public ITask
{
public:
    /**
     * Avec ce constructeur, la rotation prend un temps donné.
     * La vitesse de la rotation est adaptée pour obtenir la durée souhaitée.
     *
     * @param pNode Le Node affecté.
     * @param target Le point vers lequel doit se tourner le Node dans le repère du monde.
     * @param duration Le temps, en secondes, que doit mettre le Node à effectuer la rotation.
     * @param func Le type d'interpolation utilisée pour la rotation.
     */
    TaskLookAt(DurationDependant_t mode,
               const Ptr<Node> & pNode,
               const Core::Vector3f & target,
               float duration,
               Core::EInterpolateFunc func = Core::INTER_LINEAR);
    
    /**
     * Avec ce constructeur, la rotation s'effectue à une vitesse donnée.
     * La durée de la rotation dépend de la vitesse de la rotation.
     *
     * @param pNode Le Node affecté.
     * @param target Le point vers lequel doit se tourner le Node dans le repère du monde.
     * @param radialSpeed Vitesse de rotation du Node, en radians par secondes.
     * @param func Le type d'interpolation utilisée pour la rotation.
     */
    TaskLookAt(SpeedDependant_t mode,
               const Ptr<Node> & pNode,
               const Core::Vector3f & target,
               float radialSpeed,
               Core::EInterpolateFunc func = Core::INTER_LINEAR);
    
    virtual void update(double elapsed);

private:
    void init(const Core::Vector3f & target);
protected:
    virtual bool internalIsFinished() const;

    Ptr<Node>       _pNode;
    double          _elapsed;
    double          _duration;
    Core::EInterpolateFunc _func;
    Core::Quaternionf      _q1;
    Core::Quaternionf      _q2;
};

/**
 * Pendant toute la durée spécifiée, la tâche oriente le Node vers le point
 * ou le Node passé en argument du constructeur.
 *
 * La tâche reste efficace même si le Node et/ou sa cible se déplace.
 *
 * La vitesse de rotation du Node peut être limitée. Dans ce cas, il peut y avoir
 * un "lag" entre les mouvements du Node (ou de la cible) et l'alignement mais
 * ça évite les changements d'orientation trop brusque du Node.
 */
class LM_API_UNI TaskLockLookAt : public Core::Object, public ITask
{
public:
    /**
     * Le Node cible un point fixe, mais ça ne l'empêche pas de bouger.
     *
     * @param pNode Le Node affecté.
     * @param target La position, dans le repère monde, vers laquelle le Node doit se tourner.
     * @param duration La durée, en secondes, pendant laquelle le Node doit se tourner vers la cible.
     * @param radialSpeed Vitesse de rotation du Node.
     */
    TaskLockLookAt(const Ptr<Node> & pNode,
                   const Core::Vector3f & target,
                   float duration = INFINITY,
                   float radialSpeed = INFINITY);
    
    /**
     * Le Node cible un autre Node, et tient compte de ses déplacements.
     *
     * @param pNode Le Node affecté.
     * @param pTarget Le node à observer.
     * @param duration La durée, en secondes, pendant laquelle le Node doit se tourner vers la cible.
     * @param radialSpeed Vitesse de rotation du Node.
     */
    TaskLockLookAt(const Ptr<Node> & pNode,
                   const Ptr<Node> & pTarget,
                   float duration = INFINITY,
                   float radialSpeed = INFINITY);
    
    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;
    
    Ptr<Node>       _pNode;
    Ptr<Node>       _pTarget;
    Core::Vector3f  _target;
    double          _duration;
    float           _speed;
};

}


#endif
