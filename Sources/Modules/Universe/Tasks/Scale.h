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
#ifndef UNIVERSE_TASKSCALE_H_
#define UNIVERSE_TASKSCALE_H_

#include <Core/Math/Interpolate.h>
#include <Universe/Node.h>
#include <Universe/Tasks/ITask.h>

namespace Universe
{
/**
 * Modifie l'échelle d'un Node.
 *
 * Cas particuliers :
 * NODE_POINT_LIGHT : le X du scale modifie le rayon de la lumière.
 *
 * La tâche se termine à la fin du changement d'échelle.
 */
class LM_API_UNI TaskScale : public Core::Object, public ITask
{
public:
    /**
     * @param pNode Le Node affecté.
     * @param scale Facteur de changement d'échelle en XYZ.
     * @param duration Durée du changement d'échelle, en secondes.
     * @param relative Booléen à true si le changement d'échelle est relatif à l'échelle actuelle du Node, à false s'il est absolu.
     * @param func Le type d'interpolation utilisée pour la rotation.
     */
    TaskScale(const Ptr<Node> & pNode,
              const Core::Vector3f & scale,
              float duration,
              bool relative = true,
              Core::EInterpolateFunc func = Core::INTER_LINEAR);
    
    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<Node>       _pNode;
    Core::Vector3f  _scale1;
    Core::Vector3f  _scale2;
    double          _elapsed;
    double          _duration;
    Core::EInterpolateFunc _func;
};

/**
 * Modifie périodiquement l'échelle d'un Node.
 */
class LM_API_UNI TaskScaleLoop : public Core::Object, public ITask
{
public:
    /**
     * @param pNode Le Node affecté.
     * @param scale Facteur de changement d'échelle en XYZ.
     * @param duration Durée du changement d'échelle, en secondes.
     * @param relative Booléen à true si le changement d'échelle est relatif à l'échelle actuelle du Node, à false s'il est absolu.
     * @param func Le type d'interpolation utilisée pour la rotation.
     */
    TaskScaleLoop(const Ptr<Node> & pNode,
                  const Core::Vector3f & scaleMin,
                  const Core::Vector3f & scaleMax,
                  float period,
                  float duration = INFINITY);
    
    virtual void update(double elapsed);

protected:
    virtual bool internalIsFinished() const;

    Ptr<Node>       _pNode;
    Core::Vector3f  _scaleMin;
    Core::Vector3f  _scaleMax;
    float           _period;
    double          _phase;
    double          _duration;
};

}

#endif
