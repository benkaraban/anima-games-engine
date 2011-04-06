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
#ifndef RENDERER_BILLBOARDSTUB_H_
#define RENDERER_BILLBOARDSTUB_H_

#include <Renderer/IBillboard.h>

namespace Renderer
{
class LM_API_RDS BillboardGroupStub : public Core::Object, public IBillboardGroup
{
public:
    BillboardGroupStub(const Ptr<ITextureMap> & pTexture, EBillboardMode mode) :
        _isAlive(true), _isVisible(true), _pTexture(pTexture), _matrix(Core::IDENTITY), _mode(mode)
    {};

    virtual Core::List<Billboard> & beginUpdate() { return _bills; };
    virtual void endUpdate() {};

    virtual void setMaxSize(int32 maxSize) {};
    virtual int32 getMaxSize() const { return 0; };

    virtual void setPartScale(float s) {};
    virtual float getPartScale() const { return 0.0f; };

    virtual Ptr<ITextureMap> getTexture() const { return _pTexture; };
    virtual void setTexture(const Ptr<ITextureMap> & pTexture) { _pTexture = pTexture; };

    virtual const Core::Matrix4f & getWorldMatrix() const { return _matrix; };
    virtual void setWorldMatrix(const Core::Matrix4f & matrix) { _matrix = matrix; };

    virtual void setMode(EBillboardMode mode) { _mode = mode; };
    virtual EBillboardMode getMode() const { return _mode; };

    virtual void setAlign(EBillboardAlign align) {};
    virtual EBillboardAlign getAlign() const { return (EBillboardAlign)0; };

    virtual void setAlignVector(const Core::Vector3f & vec) {};
    virtual const Core::Vector3f & getAlignVector() const { return Core::Vector3f::ONE; };

    virtual void setSpriteLayout(int32 uCount, int32 vCount) {};
    virtual int32 getUSpriteLayout() const { return 0; };
    virtual int32 getVSpriteLayout() const { return 0; };

    virtual void setColorFactor(float f) { _colorFactor = f; };
    virtual float getColorFactor() const { return _colorFactor; };

    virtual void setGlowFactor(float f) { _glowFactor = f; };
    virtual float getGlowFactor() const { return _glowFactor; };

    virtual bool isAnimated() const { return false; };
    virtual bool hasSpriteLayout() const { return false; };

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void setRefractionScale(float r) {};
    virtual float getRefractionScale() const { return 0.0f; };

    virtual void setZBias(float z) { _zBias = z; }
    virtual float getZBias() const { return _zBias; }

    virtual void setFogFactor(float f) { _fogFactor = f; }
    virtual float getFogFactor() const { return _fogFactor; }

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

    virtual void setSpriteSmooth(bool s) {};
    virtual bool isSpriteSmooth() const { return false; };

protected:
    Core::AABoxf           _box;
    bool                   _isAlive;
    bool                   _isVisible;
    Ptr<ITextureMap>       _pTexture;
    Core::List<Billboard>  _bills;
    Core::Matrix4f         _matrix;
    EBillboardMode         _mode;
    float                  _colorFactor;
    float                  _glowFactor;
    float                  _zBias;
    float                  _fogFactor;
};
}

#endif /*ABSTRACT_IBILLBOARD_H_*/
