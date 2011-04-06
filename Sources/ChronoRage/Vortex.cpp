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
#include "Vortex.h"

#include <Assets/GeometricPrimitive.h>
#include <Core/Math/Random.h>
#include <Universe/Tasks/Translate.h>

namespace ChronoRage
{

inline float qsin(float x)
{
    const float B = 4.0f / f_PI;
    const float C = -4.0f / (f_PI * f_PI);

    float y = B * x + C * x * abs(x);

//  const float Q = 0.775;
    const float P = 0.225f;

    //y = P * (y * abs(y) - y) + y;   // Q * y + P * y * abs(y)

    return y;
}

inline float qcos(float x)
{
    x += f_PI_DIV_2;

    if(x > f_PI)
        x -= f_PI_MUL_2;

    return qsin(x);
}

Vortex::Vortex(const Ptr<Universe::World> & pWorld, const VortexSettings & settings, const Ptr<TimeWarp> & pTimeWarp, bool optimHack) :
    _pWorld(pWorld),
    _settings(settings),
    _pTimeWarp(pTimeWarp),
    _updateInProgress(false)
{
    Ptr<Renderer::IFreeForm> pFreeForm(pWorld->getScene()->createFreeForm(null, Renderer::FREE_FORM_LIGHT_MESH, true));
    pFreeForm->setMax(_settings.cubeCount * CUBE_VERTEX_COUNT, _settings.cubeCount * CUBE_INDEX_COUNT);
    pFreeForm->setGlowFlag(_settings.cubeGlow > 0.6f);
    pFreeForm->setGlowFlag(true); // TODO
    pFreeForm->setWorldSpaceCoords(optimHack);
    _pGroup = Ptr<CubeInstancesGroup>(new CubeInstancesGroup(pFreeForm, settings.cubeSize));


    Core::RandomDistributionfData rd;
    rd.gaussian.mean = 0.5f;
    rd.gaussian.sigma = 0.15f;

    Core::RandDistributionf randGauss(Core::RD_GAUSSIAN, 17, 0.0f, 1.0f, &rd);
   
    float armCount = float(settings.armCount);
    float armAngularSize = f_PI_MUL_2 / (armCount / settings.armRelativeSize);

    _cubes.resize(settings.cubeCount);

    for(int32 ii=0; ii < _cubes.size(); ii++)
    {
        _cubes[ii].u = Core::frand();
        _cubes[ii].v = Core::frand();

        float localA = Core::frand();

        if(_settings.gaussianDensity)
        {
            if(Core::frand() > 0.5f)
                localA = 0.5f + 0.5f * localA;
            else
                localA = 0.5f - 0.5f * localA;

            localA = randGauss();
        }

        float localAlpha = localA * armAngularSize;
        _cubes[ii].baseAlpha = localAlpha + f_PI_MUL_2 * float(Core::irand() % settings.armCount) / armCount;

        _cubes[ii].teta = Core::frand(0.0f, f_PI_MUL_2);
        _cubes[ii].zeta = Core::frand(0.0f, f_PI_MUL_2);

        _cubes[ii].dteta = Core::frand(0.0f, settings.cubeSelfRotationSpeed * f_PI_MUL_2);
        _cubes[ii].dzeta = Core::frand(0.0f, settings.cubeSelfRotationSpeed * f_PI_MUL_2);

        if(_settings.colorByDepth)
        {
            _cubes[ii].startColor = _settings.color0;
            _cubes[ii].endColor = _settings.color1;
        }
        else
        {
            _cubes[ii].startColor.setLerp(_settings.color0, _settings.color1, Core::frand());
            _cubes[ii].endColor = _cubes[ii].startColor;
        }
    }
}

void Vortex::startParallelUpdate(double elapsed)
{
    if(_pTimeWarp != null)
        elapsed *= _pTimeWarp->getTimeWarpFactor();
    _elapsed = elapsed;
    _updateInProgress = true;
    Core::Task::startTask(this);
}

void Vortex::endParallelUpdate()
{
    if(_updateInProgress)
    {
        waitCompletion();
        _updateInProgress = false;
    }
}

void Vortex::process()
{
    update(_elapsed);
}

void Vortex::update(double elapsed)
{
    Core::List<CubeInstance> & instances = _pGroup->beginUpdate();

    float cost, sint, z;
    float range = _settings.far - _settings.near;
    float thresholMul = 1.0f / (1.0f - _settings.nearU);
    float underthresholMul = 1.0f / _settings.nearU;
    float alphaFromZ = _settings.spiralCount / range;

    instances.resize(_cubes.size());

    static bool first = true;

    for(int32 ii=0; ii < _cubes.size(); ii++)
    {
        InternalCube & cube = _cubes[ii];
        CubeInstance & instance = instances[ii];

        cube.teta += float(elapsed) * cube.dteta;
        cube.zeta += float(elapsed) * cube.dzeta;

        if(cube.teta > f_PI)
            cube.teta -= f_PI_MUL_2;

        if(cube.zeta > f_PI)
            cube.zeta -= f_PI_MUL_2;

        cube.u += _settings.suckingSpeed * float(elapsed);

        if(cube.u > 1.0f)
            cube.u -= 1.0f;

        float nearDist = Core::lerp(_settings.nearMinWidth, _settings.nearMaxWidth, cube.v);
        float centerDist = 0.0f;

        float localU = (cube.u - _settings.nearU) * thresholMul;
        float farDist = Core::lerp(_settings.farMinWidth, _settings.farMaxWidth, cube.v);
        centerDist = Core::lerp(nearDist, farDist, localU);
        z = Core::lerp(_settings.near, _settings.far, localU);

        float alpha = fmod(cube.baseAlpha + _settings.spiralCount / (1.1f - cube.u), f_PI_MUL_2) - f_PI;

        sint = qsin(alpha);
        cost = qcos(alpha);
        Core::Vector3f pos((cost * centerDist), (sint * centerDist), z);

        float cx = qcos(cube.teta);
        float sx = qsin(cube.teta);

        float cy = qcos(cube.zeta);
        float sy = qsin(cube.zeta);

        instance.worldMat.set(
            cy,     sx*sy,      -sy*cx, 0.0f,
            0.0f,   cx,         sx,     0.0f,
            sy,     -cy*sx,     cy*cx,  0.0f,
            pos.x,  pos.y,      pos.z,  1.0f
            );

        instance.color.setLerp(cube.startColor, cube.endColor, cube.u);
        instance.glow = instance.color;
        instance.glow.a = _settings.cubeGlow;
    }

    first = false;

    _pGroup->endUpdate();
}

}//namespace ChronoRage