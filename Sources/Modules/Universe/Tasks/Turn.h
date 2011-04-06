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
#ifndef UNIVERSE_TASKTURN_H_
#define UNIVERSE_TASKTURN_H_

#include <Core/Math/Interpolate.h>
#include <Universe/Node.h>
#include <Universe/Tasks/ITask.h>

namespace Universe
{
class LM_API_UNI TaskTurn : public Core::Object, public ITask
{
public:
    /**
     *
     * @param pNode Le Node affecté.
     * @param startPos Le point de départ du Node.
     * @param endPos Le point d'arrivée du Node.
     * @param pivot Le point autour duquel tourne le Node.
     * @param duration Le temps, en secondes, que doit mettre le Node à effectuer le mouvement.
     * @param func Le type d'interpolation utilisée pour le mouvement.
     */
    TaskTurn(const Ptr<Node> & pNode,
             const Core::Quaternionf & orient,
             float duration,
             Core::EInterpolateFunc func = Core::INTER_LINEAR);
    
    virtual void update(double elapsed);

private:
    void init(const Core::Vector3f & target);
protected:
    virtual bool internalIsFinished() const;

    Ptr<Node>              _pNode;
    double                 _elapsed;
    double                 _duration;
    Core::EInterpolateFunc _func;
    Core::Quaternionf      _q1;
    Core::Quaternionf      _q2;
};

}


#endif
