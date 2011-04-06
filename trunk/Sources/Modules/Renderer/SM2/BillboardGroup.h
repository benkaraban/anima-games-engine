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
#ifndef RENDERER_BILLBOARDGROUP_H_
#define RENDERER_BILLBOARDGROUP_H_

#include <Renderer/IBillboard.h>
#include <Gfx/IDevice.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{

struct LM_API_SM2 BillboardVertex
{
    Core::Vector3f position; // 12 - 12
    byte   texcoords[4];     //  4 - 16
    float  size[2];          //  8 - 24

    union
    {
        float   angle;            //  4 - 28
        byte    dir[4];
        uint32  dirInt;
    };

    union
    {
        byte   color[4];         //  4 - 32
        uint32 colorInt;
    };
};

LM_STATIC_ASSERT(sizeof(BillboardVertex) == 32);

class LM_API_SM2 BillboardGroup : public IBillboardGroup, public IGPUResource
{
public:
    BillboardGroup(int32 maxSize, EBillboardMode mode, const Ptr<ITextureMap> & pTexture, 
                   const Ptr<Gfx::IDevice> & pDevice, const Ptr<GPUResourceLib> & pResLib);

    ~BillboardGroup();

    /**
     * Cette méthode, avec la méthode endUpdate, permet de modifier
     * le contenu du groupe. Lorsque l'on désire modifier le groupe, que ce soit
     * pour ajouter de nouveaux éléments, en retirer ou les mettre à jour, on
     * doit appeler la méthode beginUpdate, modifier la liste dont la référence
     * est retournée puis appeler endUpdate.
     *
     * A la fin de la mise à jour, le groupe sera optimisé pour être rendu efficacement,
     * en utilisant le nouveau contenu de la liste.
     *
     * La référence retournée par beginUpdate n'est valide que jusqu'au prochain
     * appel à endUpdate, elle ne doit en aucun cas être utilisée après cet appel.
     *
     * Chaque update ayant un coût fixe, il est plus efficace d'effectuer toutes les
     * modifications de la liste avec une seule paire de begin/end.
     */
    virtual Core::List<Billboard> & beginUpdate() { return _bills; };
    virtual void endUpdate();

    virtual void setMaxSize(int32 maxSize);
    virtual int32 getMaxSize() const;

    virtual void setPartScale(float s);
    virtual float getPartScale() const { return _partScale; };

    virtual Ptr<ITextureMap> getTexture() const { return _pTexture; };
    virtual void setTexture(const Ptr<ITextureMap> & pTexture) { _pTexture = pTexture; };

    virtual const Core::Matrix4f & getWorldMatrix() const { return _worldMatrix; };
    virtual void setWorldMatrix(const Core::Matrix4f & matrix);

    virtual void setMode(EBillboardMode mode) { _mode = mode; };
    virtual EBillboardMode getMode() const { return _mode; };

    virtual void setAlign(EBillboardAlign align) { _align = align; };
    virtual EBillboardAlign getAlign() const { return _align; };

    virtual void setAlignVector(const Core::Vector3f & vec) { _alignVec = vec; };
    virtual const Core::Vector3f & getAlignVector() const { return _alignVec; };

    virtual void setSpriteLayout(int32 uCount, int32 vCount);
    virtual int32 getUSpriteLayout() const { return _spriteU; };
    virtual int32 getVSpriteLayout() const { return _spriteV; };

    virtual void setSpriteSmooth(bool s) { _spriteSmooth = s; };
    virtual bool isSpriteSmooth() const { return _spriteSmooth; };

    virtual void setColorFactor(float f) { _colorFactor = f; };
    virtual float getColorFactor() const { return _colorFactor; };

    virtual void setGlowFactor(float f) { _glowFactor = f; };
    virtual float getGlowFactor() const { return _glowFactor; };

    virtual void setRefractionScale(float r) { _refrScale = r; };
    virtual float getRefractionScale() const { return _refrScale; };

    virtual void setZBias(float z) { _zBias = z; }
    virtual float getZBias() const { return _zBias; }

    virtual void setFogFactor(float f) { _fogFactor = f; }
    virtual float getFogFactor() const { return _fogFactor; }

    virtual bool isAnimated() const;
    virtual bool hasSpriteLayout() const { return _spriteU > 1 || _spriteV > 1;  };

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

    virtual void onDeviceLost();
    virtual void onDeviceReset();

    // PACKAGE

    virtual void sortByDist(const Core::Vector3f & viewPos, const Core::Vector3f & viewDir);
    virtual void sendData(const Ptr<Gfx::IDevice> & pDevice);
    virtual float getSqDist(const Core::Vector3f & position) const;

protected:
    Ptr<Gfx::IDevice>    _pDevice;
    Ptr<GPUResourceLib>  _pResLib;

    virtual void update(const Ptr<Gfx::IVertexBuffer> & pVBuffer, int32 & size);
    virtual void send(const Ptr<Gfx::IVertexBuffer> & pVBuffer, const Ptr<Gfx::IDevice> & pDevice, int32 size);

    virtual void updateLocalBoundingBox();

    int32            _maxSize;
    EBillboardMode   _mode;
    EBillboardAlign  _align;
    Core::Vector3f   _alignVec;
    Core::Matrix4f   _worldMatrix;
    Core::Matrix4f   _worldMatrixInv;
    Core::Vector3f   _position;
    Ptr<ITextureMap> _pTexture;

    Core::AABoxf     _localBox;
    Core::AABoxf     _box;


    float           _zBias;
    float           _fogFactor;
    int32           _frame;

    Ptr<Gfx::IVertexBuffer> _pVBuffer;

    Core::List<Billboard>       _bills;
    Core::List<BillboardVertex> _vertices;
    int32                       _size;
    float                       _partScale;
    float                       _colorFactor;
    float                       _glowFactor;
    bool                        _isAlive;
    bool                        _isVisible;
    int32                       _spriteU;
    int32                       _spriteV;
    bool                        _spriteSmooth;
    float                       _refrScale;
};
}

#endif /* BILLBOARDGROUP_H_ */
