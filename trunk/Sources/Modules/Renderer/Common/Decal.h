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
#ifndef RENDERER_DECAL_H_
#define RENDERER_DECAL_H_

#include <Renderer/IDecal.h>

namespace Renderer
{
class LM_API_RDR Decal : public Core::Object, public IDecal
{
public:
    Decal(const Ptr<ITextureMap> & pTexture, EDecalMode mode);

    virtual void setWorldMatrix(const Core::Matrix4f & matrix);
    virtual void getWorldMatrix(Core::Matrix4f & matrix) const { matrix = _matrix; };
    virtual void setSize(float width, float height);
    virtual void setSize(float size) { setSize(size, size); };

    virtual float getWidth() const { return _width; };
    virtual float getHeight() const { return _height; };

    virtual void setMode(EDecalMode mode) { _mode = mode; };
    virtual EDecalMode getMode() const { return _mode; };

    virtual void setDecalNormal(EDecalNormal n) { _normal = n; };
    virtual EDecalNormal getDecalNormal() const { return _normal; };

    virtual void setColor(const Core::Vector4f & color) { _color = color; };
    virtual const Core::Vector4f & getColor() const { return _color; };

    virtual void setGlow(const Core::Vector4f & color) { _glow = color; };
    virtual const Core::Vector4f & getGlow() const { return _glow; };

    virtual void setTexture(const Ptr<ITextureMap> & pTexture) { _pTexture = pTexture; };
    virtual Ptr<ITextureMap> getTexture() const { return _pTexture; };

    virtual void setZBias(float z) { _zBias = z; }
    virtual float getZBias() const { return _zBias; }

    virtual void setFogFactor(float f) { _fogFactor = f; }
    virtual float getFogFactor() const { return _fogFactor; }

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setSpriteLayout(int32 uCount, int32 vCount);
    virtual int32 getUSpriteLayout() const { return _spriteU; };
    virtual int32 getVSpriteLayout() const { return _spriteV; };

    virtual bool isAnimated() const;
    virtual bool hasSpriteLayout() const { return _spriteU > 1 || _spriteV > 1;  };

    virtual void setTimeKey(float k) { _timeKey = k; };
    virtual float getTimeKey() const { return _timeKey; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

    // PACKAGE

    virtual void getVertices(DecalVertex pVertices [], bool glowColor) const;
    virtual float getSqDist(const Core::Vector3f & position) const;

    const Core::Vector3f & getPosition() const { return _position; }

protected:
    virtual void updateBoundingBox();

    Core::Matrix4f  _matrix;
    Core::Vector3f  _position;

    float           _width;
    float           _height;
    float           _timeKey;
    float           _zBias;
    float           _fogFactor;

    Ptr<ITextureMap> _pTexture;
    EDecalMode       _mode;
    EDecalNormal     _normal;
    Core::Vector4f   _color;
    Core::Vector4f   _glow;
    Core::AABoxf     _box;
    bool             _isAlive;
    bool             _isVisible;
    int32            _spriteU;
    int32            _spriteV;
};
}

#endif /* DECAL_H_ */
