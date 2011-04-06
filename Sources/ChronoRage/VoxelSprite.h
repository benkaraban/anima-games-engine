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
#ifndef CHRONORAGE_VOXELSPRITE_H_
#define CHRONORAGE_VOXELSPRITE_H_

#include <Core/Math/Interpolate.h>
#include <CubeInstancesGroup.h>
#include <Universe/World.h>

namespace ChronoRage
{
LM_ENUM_2(ESpriteState,
          SPRITE_ANIM,
          SPRITE_EXPLOSION);

struct Voxel
{
    Core::Vector3f      position;
    Core::Quaternionf   orient;
    Core::Vector4f      color;
    Core::Vector4f      colorSave;
    Core::Vector4f      glow;
};

class VoxelSpriteTemplate : public Core::Object
{
public:
    VoxelSpriteTemplate(const Ptr<Core::Bitmap> & pBitmap, float cubeSize, float padding, float glowFactor = 0.1f);

    float getCubeSize() const { return _cubeSize; }
    const Core::List<Voxel> & getVoxels() const { return _voxels; }
    Core::List<Voxel> & getVoxels() { return _voxels; }

    const Core::AABoxf & getBox() const { return _box; }

protected:
    Core::List<Voxel>           _voxels;
    Core::AABoxf                _box;
    float                       _cubeSize;
};

struct VoxelCloud
{
    Core::Vector3f  position;
    float           dispRadius;
    float           cosAlpha;

    float           alpha;
    float           beta;
};

struct VoxelExplosion
{
    Core::Vector3f      dir;
    Core::Vector3f      gravityHole;
    float               minSpeed;
    float               maxSpeed;
    float               clampSpeed;
    float               dispAngle;
    float               selfRotSpeed;
};

class VoxelSprite : public Core::Object
{
public:
    VoxelSprite(const Ptr<Universe::World> & pWorld, const Ptr<VoxelSpriteTemplate> & pTemplate,
        const Core::Vector4f & color = Core::Vector4f::ONE, bool optimHack = true);

    void setMatrices(const Core::Matrix4f & localToWorld, const Core::Matrix4f & worldToLocal);

    void defaultSprite();

    void cloudToSprite(const VoxelCloud & cloud, float minDuration, float maxDuration);
    void currentToSprite(float minDuration, float maxDuration);
    void spriteToCloud(const VoxelCloud & cloud, float minDuration, float maxDuration);
    void spriteToCloud2(const VoxelCloud & cloud1, const VoxelCloud & cloud2, float minT1, float maxT1, float minT2, float maxT2);

    void teleport(const Core::Matrix4f & cvtMat, float warmupDuration, float teleportDuration);

    void explosion(const VoxelExplosion & explo);
    void partialExplosion(const VoxelExplosion & explo, float a);

    void blink(float blinkTotalTime, int32 blinkNumber);

    void setGlowFlag(bool glow);

    bool isAnimationFinished() { return _animationFinished; }

    bool isAlive() const { return _pGroup->isAlive(); }

    void kill() { _pGroup->kill(); }

    void update(double elapsed);
    int32 pixelCount() const { return _pixelCount; }

protected:
    struct VoxelState
    {
        float               t;

        Core::Vector3f      p0;
        Core::Vector3f      p1;
        Core::Vector3f      p2;

        Core::Quaternionf   q0;
        Core::Quaternionf   q1;
        Core::Quaternionf   q2;

        float               t1;
        float               t2;

        Core::EInterpolateFunc  f01;
        Core::EInterpolateFunc  f12;

        Core::Vector4f      color0;
        Core::Vector4f      color1;
        Core::Vector4f      color2;

        Core::Vector4f      glow0;
        Core::Vector4f      glow1;
        Core::Vector4f      glow2;

        float               dteta;
        float               dzeta;

        bool                blinking;
        float               blinkTimer;
        float               blinkTotalTime;
        int32               blinkNumber;

        Core::Vector3f      speed;
    };

    void updateAnim(double elapsed, Core::List<CubeInstance> & instances);
    void updateExplo(double elapsed, Core::List<CubeInstance> & instances);
    bool updatePartialExplo(double elapsed, Core::List<CubeInstance> & instances);
    void updateBox();

    Ptr<Universe::World>            _pWorld;
    Core::AABoxf                    _localBox;
    Core::AABoxf                    _worldBox;
    Ptr<CubeInstancesGroup>         _pGroup;
    Core::List<Voxel>               _template;
    Core::List<VoxelState>          _states;
    Core::List<Voxel>               _voxels;
    Core::Matrix4f                  _localToWorld;
    Core::Matrix4f                  _localToWorldTrans;

    Core::Matrix4f                  _worldToLocal;

    ESpriteState                    _state;
    VoxelExplosion                  _explo;
    bool                            _animationFinished;
    int32                           _startPixelCount;
    int32                           _iExplosion;
    int32                           _pixelCount;
};


}//namespace ChronoRage


#endif/*CHRONORAGE_Shot_H_*/