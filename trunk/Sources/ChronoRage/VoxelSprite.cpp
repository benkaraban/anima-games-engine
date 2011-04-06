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
#include <VoxelSprite.h>

#include <Assets/GeometricPrimitive.h>
#include <Core/Math/Random.h>
#include <Universe/Tasks/Translate.h>

namespace ChronoRage
{
VoxelSpriteTemplate::VoxelSpriteTemplate(const Ptr<Core::Bitmap> & pBitmap, float cubeSize, float padding, float glowFactor)
{
    if(pBitmap->getFormat() != Core::ARGB_U8)
        throw Core::Exception(L"Bad voxel sprite format.");

    _cubeSize = cubeSize;

    float paddedSize = cubeSize + padding;
    float halfSize = 0.5f * paddedSize;

    Core::Vector3f start(-halfSize * float(pBitmap->getWidth()),
                          0.0f,
                          halfSize * float(pBitmap->getHeight()));

    byte * pPixel = (byte*)pBitmap->getData();

    _box.clear();

    for(int32 y=0; y < pBitmap->getHeight(); y++)
    {
        for(int32 x=0; x < pBitmap->getWidth(); x++)
        {
            Voxel voxel;
            voxel.position = start + Core::Vector3f(float(x) * paddedSize, 0.0f, float(-y) * paddedSize);
            voxel.color = Assets::unpackColor(pPixel);
            voxel.colorSave = voxel.color;
            voxel.glow = voxel.color;
            voxel.glow.a = glowFactor;
            voxel.orient = Core::Quaternionf(Core::IDENTITY);

            if(voxel.color.a > 0.0f)
            {
                _box.addPoint(voxel.position - Core::Vector3f(halfSize));
                _box.addPoint(voxel.position + Core::Vector3f(halfSize));

                voxel.color.a = 1.0f;
                _voxels.push_back(voxel);
            }

            pPixel += 4;
        }
    }
}


VoxelSprite::VoxelSprite(const Ptr<Universe::World> & pWorld, const Ptr<VoxelSpriteTemplate> & pTemplate,
                         const Core::Vector4f & color, bool optimHack) :
    _pWorld(pWorld),
    _template(pTemplate->getVoxels()),
    _localBox(pTemplate->getBox()),
    _localToWorld(Core::IDENTITY),
    _worldToLocal(Core::IDENTITY),
    _state(SPRITE_ANIM),
    _pixelCount(pTemplate->getVoxels().size())
{
    Ptr<Renderer::IFreeForm> pFreeForm(pWorld->getScene()->createFreeForm(null, Renderer::FREE_FORM_LIGHT_MESH));
    pFreeForm->setMax(std::max(1, _template.size() * CUBE_VERTEX_COUNT), std::max(1, _template.size() * CUBE_INDEX_COUNT));
    pFreeForm->setGlowFlag(false);
    pFreeForm->setWorldSpaceCoords(optimHack);

    _pGroup = Ptr<CubeInstancesGroup>(new CubeInstancesGroup(pFreeForm, pTemplate->getCubeSize()));
    std::random_shuffle(_template.begin(), _template.end());
    _voxels = _template;
    _states.resize(_voxels.size());
    _startPixelCount = _states.size();
    _iExplosion = _startPixelCount;

    if(color != Core::Vector4f::ONE)
    {
        for(int32 ii=0; ii < _template.size(); ii++)
            _template[ii].color = Core::mul(_template[ii].color, color);
    }

    for(int32 iState = 0; iState < _states.size(); ++iState)
        _states[iState].blinking = false;
}

void VoxelSprite::setGlowFlag(bool glow)
{
    _pGroup->setGlowFlag(glow);
}

void VoxelSprite::partialExplosion(const VoxelExplosion & explo, float a)
{
    int32 oldExplo = _iExplosion;
    _explo = explo;
    _iExplosion = int32(a * _startPixelCount);
    Core::Vector3f  axe1(0.0f, 0.0f, 1.0f);

    if(abs(explo.dir.z) > 0.5f)
        axe1 = Core::Vector3f(1.0f, 0.0f, 0.0f);

    Core::Vector3f  axe2(Core::cross(explo.dir, axe1));

    for(int32 ii=_iExplosion; ii < oldExplo; ii++)
    {
        const Voxel & voxel = _template[ii];
        VoxelState & state = _states[ii];
        Voxel & voxel2 = _voxels[ii];

        //voxel2.position = _localToWorld.apply(voxel2.position);
        

        Core::Vector3f dir(explo.dir);

        Core::Matrix4f rot1(Core::ROTATION, Core::frand(-explo.dispAngle, explo.dispAngle), axe1);
        Core::Matrix4f rot2(Core::ROTATION, Core::frand(-explo.dispAngle, explo.dispAngle), axe2);

        dir = rot1.apply3x3(rot2.apply3x3(dir));

        state.t = 0.0f;
        state.speed = Core::frand(explo.minSpeed, explo.maxSpeed) * dir;
        state.dteta = Core::frand(-explo.selfRotSpeed, explo.selfRotSpeed);
        state.dzeta = Core::frand(-explo.selfRotSpeed, explo.selfRotSpeed);
    }  
}

void VoxelSprite::explosion(const VoxelExplosion & explo)
{
    _state = SPRITE_EXPLOSION;
    _explo = explo;
    _animationFinished = false;
    partialExplosion(explo, 0.0f);
    updateBox();
}

bool VoxelSprite::updatePartialExplo(double elapsed, Core::List<CubeInstance> & instances)
{
    bool animationFinished = true;
    float felapsed = float(elapsed);

    for(int32 ii=_iExplosion; ii < _states.size(); ii++)
    {
        //Voxel Animation
        VoxelState & state = _states[ii];
        Voxel & voxel = _voxels[ii];

        //if(voxel.position.z < _explo.gravityHole.z)
        //    continue;

        state.t += felapsed;


        float t = Core::clamp(0.0f, 1.0f, state.t * 2.0f);
        float telapsed = t * float(elapsed);
        state.speed.z += 200.0f * telapsed;

        float speedNorm = state.speed.getLengthSafe();

        if(speedNorm > _explo.clampSpeed)
            state.speed *= (_explo.clampSpeed / speedNorm);

        voxel.position += float(elapsed) * state.speed;

        Core::Matrix4f rot(Core::ROTATION, telapsed, Core::ROT_Z_ROLL);
        voxel.position = rot.apply3x3(voxel.position);

        Core::Quaternionf   selfRot(felapsed * state.dteta, 0.0f, felapsed * state.dzeta);
        voxel.orient = voxel.orient * selfRot;

        if(voxel.position.z < _explo.gravityHole.z)
            animationFinished = false;

        CubeInstance & instance = instances[ii];

        instance.worldMat.setTransposeQT(voxel.orient, voxel.position);
        
        instance.color     = voxel.color;
        instance.glow      = voxel.glow;
    }

    return animationFinished;
}

void VoxelSprite::updateExplo(double elapsed, Core::List<CubeInstance> & instances)
{
    _animationFinished = updatePartialExplo(elapsed, instances);
}

void VoxelSprite::defaultSprite()
{
    _animationFinished = false;

    for(int32 ii=0; ii < _states.size(); ii++)
    {
        const Voxel & voxel = _template[ii];
        VoxelState & state = _states[ii];

        Core::Vector4f color(voxel.color);
        Core::Vector4f glow(voxel.glow);

        state.t = state.t1 = state.t2 = 0.0f;
        //state.p0 = state.p1 = state.p2 = _localToWorld.apply(voxel.position);
        //state.q0 = state.q1 = state.q2 = voxel.orient;
        //state.color0 = state.color1 = state.color2 = voxel.color;
        //state.glow0 = state.glow1 = state.glow2 = voxel.glow;
        //state.f01 = state.f12 = Core::INTER_LINEAR;

        state.blinking = false;
    }

    updateBox();
}

void getPoint(const VoxelCloud & cloud, Core::Vector3f & p, Core::Quaternionf & q)
{
    float u = cloud.cosAlpha + Core::frand() * (1.0f - cloud.cosAlpha);
    float teta = f_PI_MUL_2 * Core::frand();

    float mu = Core::L_sqrt(1 - u*u);
    float cosTeta, sinTeta;
    Core::L_sincos(teta, &cosTeta, &sinTeta);

    Core::Vector3f  unitv(mu * cosTeta, u, mu * sinTeta);
    unitv *= Core::frand() * cloud.dispRadius;
    p = cloud.position - unitv;

    q.setRotation(Core::frand() * cloud.alpha, Core::frand() * cloud.beta, 0.0f);
}

void VoxelSprite::cloudToSprite(const VoxelCloud & cloud, float minDuration, float maxDuration)
{
    _animationFinished = false;

    for(int32 ii=0; ii < _states.size(); ii++)
    {
        const Voxel & voxel = _template[ii];
        VoxelState & state = _states[ii];

        Core::Quaternionf q;
        Core::Vector3f p;
        Core::Vector4f color(voxel.color);
        Core::Vector4f glow(voxel.glow);

        getPoint(cloud, p, q);

        state.t = 0.0f;
        state.t1 = 0.0f;
        state.p0 = state.p1 = p;
        state.q0 = state.q1 = q;
        state.color0 = state.color1 = color;
        state.glow0 = state.glow1 = glow;
        state.f01 = state.f12 = Core::INTER_LINEAR;
        
        state.t2 = Core::frand(minDuration, maxDuration);
        state.p2 = voxel.position;
        state.q2 = voxel.orient;
        state.color2 = voxel.color;
        state.glow2 = voxel.glow;
    }

    updateBox();
}

void VoxelSprite::currentToSprite(float minDuration, float maxDuration)
{
    _animationFinished = false;

    for(int32 ii=0; ii < _states.size(); ii++)
    {
        const Voxel & currentVoxel = _voxels[ii];
        const Voxel & voxel = _template[ii];
        VoxelState & state = _states[ii];

        Core::Quaternionf q(currentVoxel.orient);
        Core::Vector3f p(_worldToLocal.apply(currentVoxel.position));
        Core::Vector4f color(voxel.color);
        Core::Vector4f glow(voxel.glow);

        state.t = 0.0f;
        state.t1 = 0.0f;
        state.p0 = state.p1 = p;
        state.q0 = state.q1 = q;
        state.color0 = state.color1 = color;
        state.glow0 = state.glow1 = glow;
        state.f01 = state.f12 = Core::INTER_LINEAR;
        
        state.t2 = Core::frand(minDuration, maxDuration);
        state.p2 = voxel.position;
        state.q2 = voxel.orient;
        state.color2 = voxel.color;
        state.glow2 = voxel.glow;
    }

    updateBox();
}

void VoxelSprite::teleport(const Core::Matrix4f & cvtMat, float warmupDuration, float teleportDuration)
{
    _animationFinished = false;

    for(int32 ii=0; ii < _states.size(); ii++)
    {
        const Voxel & voxel = _template[ii];
        VoxelState & state = _states[ii];
        const Voxel & curVoxel = _voxels[ii];

        state.t = 0.0f;
        state.t1 = Core::frand(0.0f, warmupDuration);
        state.p0 = state.p1 = cvtMat.apply(curVoxel.position);
        state.q2 = state.q0 = state.q1 = voxel.orient;
        state.color2 = state.color0 = state.color1 = voxel.color;
        state.glow2 = state.glow0 = state.glow1 = voxel.glow;
        state.f01 = Core::INTER_LINEAR;
        state.f12 = Core::INTER_FADE_IN_OUT;
        
        state.t2 = state.t1 + teleportDuration;
        state.p2 = voxel.position;
    }

    updateBox();
}

void VoxelSprite::spriteToCloud(const VoxelCloud & cloud, float minDuration, float maxDuration)
{
    _animationFinished = false;

    for(int32 ii=0; ii < _states.size(); ii++)
    {
        const Voxel & voxel = _template[ii];
        VoxelState & state = _states[ii];

        Core::Quaternionf q;
        Core::Vector3f p;
        Core::Vector4f color(voxel.color);
        Core::Vector4f glow(voxel.glow);
        glow.a = 0.0f;

        getPoint(cloud, p, q);

        state.t = 0.0f;
        state.t1 = 0.0f;
        state.p0 = state.p1 = voxel.position;
        state.q0 = state.q1 = voxel.orient;
        state.color0 = state.color1 = voxel.color;
        state.glow0 = state.glow1 = voxel.glow;
        state.f01 = state.f12 = Core::INTER_LINEAR;
        
        state.t2 = Core::frand(minDuration, maxDuration);
        state.p2 = p;
        state.q2 = q;
        state.color2 = color;
        state.glow2 = glow;
    }

    updateBox();
}

void VoxelSprite::spriteToCloud2(const VoxelCloud & cloud1, const VoxelCloud & cloud2, float minT1, float maxT1, float minT2, float maxT2)
{
    _animationFinished = false;

    for(int32 ii=0; ii < _states.size(); ii++)
    {
        const Voxel & voxel = _template[ii];
        VoxelState & state = _states[ii];

        Core::Quaternionf q;
        Core::Vector3f p;
        Core::Vector4f color(voxel.color);
        Core::Vector4f glow(voxel.glow);
        glow.a = 0.0f;

        state.t = 0.0f;
        state.t1 = Core::frand(minT1, maxT1);
        state.t2 = Core::frand(minT2, maxT2);

        getPoint(cloud1, state.p1, state.q1);
        getPoint(cloud2, state.p2, state.q2);

        state.p0 = voxel.position;
        state.q0 = voxel.orient;
        state.color0 = state.color1 = voxel.color;
        state.glow0 = state.glow1 = voxel.glow;
        state.f01 = Core::INTER_FADE_OUT;
        state.f12 = Core::INTER_FADE_IN;
        
        state.color2 = color;
        state.glow2 = glow;
    }

    updateBox();
}

void VoxelSprite::blink(float blinkTotalTime, int32 blinkNumber)
{
    for(int32 ii=0; ii < _states.size(); ii++)
    {
        _states[ii].blinkNumber = blinkNumber;
        _states[ii].blinkTotalTime = blinkTotalTime;
        _states[ii].blinkTimer = 0.0f;
        _states[ii].blinking = true;

        Voxel & voxel = _voxels[ii];
        voxel.colorSave = voxel.color;
    }
}

void VoxelSprite::update(double elapsed)
{
    Core::List<CubeInstance> & instances = _pGroup->beginUpdate();

    instances.resize(_voxels.size());

    if(_state == SPRITE_ANIM)
    {
        updatePartialExplo(elapsed, instances);
        updateAnim(elapsed, instances);
    }
    else
    {
        updateExplo(elapsed, instances);
    }

    _pGroup->endUpdate();
    updateBox();
}

void VoxelSprite::updateAnim(double elapsed, Core::List<CubeInstance> & instances)
{
    bool animationFinished = true;

    for(int32 ii=0; ii < _iExplosion; ii++)
    {
        //Voxel Animation
        VoxelState & state = _states[ii];
        Voxel & voxel = _voxels[ii];
        CubeInstance & instance = instances[ii];

        state.t = std::min(state.t2, state.t + float(elapsed));

        if(state.t2 == 0.0f)
        {
            const Voxel & templateVoxel = _template[ii];
            voxel.color = templateVoxel.color;
            voxel.position = _localToWorld.apply(templateVoxel.position);
            voxel.orient = templateVoxel.orient;

            instance.worldMat  = _localToWorld * Core::Matrix4f(voxel.orient.toMatrix3(), templateVoxel.position);
            instance.worldMat.transposeSelf();
        }
        else if(state.t >= state.t2) // Sprite standard sans animation
        {
            voxel.color = state.color2;
            voxel.position = _localToWorld.apply(state.p2);

            instance.worldMat       = _localToWorldTrans;
            instance.worldMat._30   = voxel.position.x;
            instance.worldMat._31   = voxel.position.y;
            instance.worldMat._32   = voxel.position.z;
        }
        else if(state.t >= state.t1)
        {
            float u = Core::interpolate((state.t - state.t1) / (state.t2 - state.t1), state.f12);
            voxel.color.setLerp(state.color1, state.color2, u);
            voxel.position.setLerp(state.p1, state.p2, u);
            voxel.orient.setSlerp(state.q1, state.q2, u);

            instance.worldMat  = _localToWorld * Core::Matrix4f(voxel.orient.toMatrix3(), voxel.position);
            instance.worldMat.transposeSelf();
        }
        else
        {
            float u = Core::interpolate(state.t / state.t1, state.f01);
            voxel.color.setLerp(state.color0, state.color1, u);
            voxel.position.setLerp(state.p0, state.p1, u);
            voxel.orient.setSlerp(state.q0, state.q1, u);

            instance.worldMat  = _localToWorld * Core::Matrix4f(voxel.orient.toMatrix3(), voxel.position);
            instance.worldMat.transposeSelf();
        }

        animationFinished = (animationFinished && (state.t >= state.t2));

        //Voxel blink
        if(state.blinking)
        {
            state.blinkTimer = state.blinkTimer + (float) elapsed;
            
            if(state.blinkTimer >= state.blinkTotalTime)
            {
                state.blinking = false;
                voxel.color = voxel.colorSave;
            }
            else
            {
                float ratio = Core::L_abs(Core::L_sin(((state.blinkTimer * (float)state.blinkNumber))/state.blinkTotalTime * f_PI));

                voxel.color.setLerp(voxel.colorSave, Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), ratio);
            }
        }

        instance.color     = voxel.color;
        instance.glow      = voxel.glow;
    }

    _animationFinished = animationFinished;
}

void VoxelSprite::setMatrices(const Core::Matrix4f & localToWorld, const Core::Matrix4f & worldToLocal)
{
    _localToWorld = localToWorld;
    _worldToLocal = worldToLocal;

    _localToWorldTrans = _localToWorld;
    _localToWorldTrans.transposeSelf();

    _worldBox.set(_localBox, _localToWorld);
    updateBox();
}

void VoxelSprite::updateBox()
{
    if(_animationFinished)
        _pGroup->setPrecomputedAABox(_worldBox);
    else
        _pGroup->setPrecomputedAABox(Core::AABoxf(Core::Vector3f(-1000.0f, -1000.0f, -1000.0f), Core::Vector3f(1000.0f, 1000.0f, 1000.0f)));
}

}//namespace ChronoRage