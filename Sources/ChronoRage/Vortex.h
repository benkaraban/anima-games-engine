/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef CHRONORAGE_VORTEX_H_
#define CHRONORAGE_VORTEX_H_

#include <Core/RT/Task.h>

#include <CubeInstancesGroup.h>
#include <Universe/World.h>

#include <TimeWarp.h>

namespace ChronoRage
{

struct LM_API_CHR VortexSettings : public Core::Object
{
    float       cubeSize;
    int32       cubeCount;
    float       cubeGlow;

    int32       armCount;
    float       armRelativeSize; // 1.0 -> les bras se touchent, 0.5 -> les bras sont espacés de la largeur d'un bras...
    bool        gaussianDensity;

    Core::Vector4f  color0;
    Core::Vector4f  color1;
    bool            colorByDepth;

    float       cubeSelfRotationSpeed; // en tour/sec

    float       start;
    float       startWidth;

    float       nearU; // quand U < uThreshold => plan sinon => cylindre
    float       sharpness; // si = 1.0 => passe de near à far linéairement
    float       sharpness2; // si = 1.0 => passe de near à far linéairement

    float       near;
    float       nearMinWidth;
    float       nearMaxWidth;

    float       far;
    float       farMinWidth;
    float       farMaxWidth;

    float       spiralCount;

    float       suckingSpeed;
};

class LM_API_CHR Vortex : public Core::Object, public Core::Task
{
public:
    Vortex(const Ptr<Universe::World> & pWorld, const VortexSettings & settings, const Ptr<TimeWarp> &pTimeWarp, bool optimHack = true);

    virtual void startParallelUpdate(double elapsed);
    virtual void endParallelUpdate();

    virtual void process();
    void update(double elapsed);

protected:
    struct InternalCube
    {
        float   u; // position (0 .. 1) en longueur du vortex
        float   v; // position (0 .. 1) dans l'épaisseur du vortex

        float   baseAlpha; // Rotation par rapport au centre du vortex, en U = 0.0
        
        float   teta; // Rotation self X
        float   dteta;

        float   zeta; // Rotation self Y
        float   dzeta;

        Core::Vector4f      startColor;
        Core::Vector4f      endColor;
    };

    Ptr<Universe::World>            _pWorld;
    Core::List<InternalCube>        _cubes;
    Ptr<CubeInstancesGroup>         _pGroup;
    VortexSettings                  _settings;
    Ptr<TimeWarp>                   _pTimeWarp;
    double                          _elapsed;
    bool                            _updateInProgress;
};


}//namespace ChronoRage


#endif/*CHRONORAGE_Shot_H_*/