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
#ifndef UNIVERSE_TASKROTATE_H_
#define UNIVERSE_TASKROTATE_H_

#include <Core/Math/Interpolate.h>
#include <Universe/Node.h>
#include <Universe/Tasks/ITask.h>

#include <Core/Math/MathTools.h>

namespace Universe
{
/**
 * Effectue une rotation ponctuelle sur un Node.
 *
 * La tâche se termine à la fin de la rotation.
 */
class LM_API_UNI TaskRotate : public Core::Object, public ITask
{
public:
    /**
     * Avec ce constructeur, la rotation prend un temps donné.
     * La vitesse de la rotation est adaptée pour obtenir la durée souhaitée.
     *
     * @param pNode Le Node affecté.
     * @param pitch Angle pour la rotation sur l'axe X, en radians.
     * @param yaw Angle pour la rotation sur l'axe Y, en radians.
     * @param roll Angle pour la rotation sur l'axe Z, en radians.
     * @param duration Durée de la rotation, en secondes.
     * @param func Le type d'interpolation utilisée pour la rotation.
     * @param referentiel Le référentiel des axes de rotation (référentiel du Node ou du monde).
     */
    TaskRotate(DurationDependant_t mode,
               const Ptr<Node> & pNode,
               float pitch,
               float yaw,
               float roll,
               double duration,
               Core::EInterpolateFunc func = Core::INTER_LINEAR,
               EReferential referentiel = LOCAL_COORD);

    /**
     * Avec ce constructeur, la rotation s'effectue à une vitesse donnée.
     * La durée de la rotation dépend de la vitesse de la rotation.
     *
     * @param pNode Le Node affecté.
     * @param pitch Angle pour la rotation sur l'axe X, en radians.
     * @param yaw Angle pour la rotation sur l'axe Y, en radians.
     * @param roll Angle pour la rotation sur l'axe Z, en radians.
     * @param radialSpeed Vitesse de rotation du Node, en radians par secondes.
     * @param func Le type d'interpolation utilisée pour la rotation.
     * @param referentiel Le référentiel des axes de rotation (référentiel du Node ou du monde).
     */
    TaskRotate(SpeedDependant_t mode,
               const Ptr<Node> & pNode,
               float pitch,
               float yaw,
               float roll,
               double radialSpeed,
               Core::EInterpolateFunc func = Core::INTER_LINEAR,
               EReferential referentiel = LOCAL_COORD);

    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<Node>              _pNode;
    Core::Quaternionf      _q1;
    float                  _pitch;
    float                  _yaw;
    float                  _roll;
    double                 _elapsed;
    double                 _duration;
    EReferential           _referentiel;
    Core::EInterpolateFunc _func;
};

/**
 * Effectue une rotation continue sur un Node.
 * Le Node pivote sur un ou plusieurs axes à une certaine vitesse donnée pendant une durée donnée.
 */
class LM_API_UNI TaskRotateLoop : public Core::Object, public ITask
{
public:
    /**
     * @param pitchSpeed Vitesse de rotation du Node sur l'axe X, en radians par secondes.
     * @param yawSpeed Vitesse de rotation du Node sur l'axe Y, en radians par secondes.
     * @param rollSpeed Vitesse de rotation du Node sur l'axe Z, en radians par secondes.
     * @param duration Durée de la tâche, en secondes.
     * @param referentiel Le référentiel des axes de rotation (référentiel du Node ou du monde).
     */
    TaskRotateLoop(const Ptr<Node> & pNode,
                   float pitchSpeed,
                   float yawSpeed,
                   float rollSpeed,
                   double duration = INFINITY,
                   EReferential referentiel = LOCAL_COORD);

    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<Node>    _pNode;
    float        _pitchSpeed;
    float        _yawSpeed;
    float        _rollSpeed;
    double       _duration;
    EReferential _referentiel;
};
}

#endif /* TASKROTATE_H_ */
