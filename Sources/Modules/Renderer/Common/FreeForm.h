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
#ifndef RENDERER_FREEFORM_H_
#define RENDERER_FREEFORM_H_

#include <Gfx/IDevice.h>
#include <Renderer/IFreeForm.h>
#include <Renderer/StreamBuffer.h>

namespace Renderer
{
class LM_API_RDR FreeForm : public Core::Object, public IFreeForm, public IGPUResource
{
public:
    // Long life FreeForm with its own buffer
    FreeForm(int32 maxVertices, int32 maxIndices, EFreeFormMode mode, const Ptr<ITextureMap> & pTexture, 
             const Ptr<Gfx::IDevice> & pDevice, const Ptr<GPUResourceLib> & pResLib);

    // Short life FreeForm with a shared StreamBuffer
    FreeForm(int32 maxVertices, int32 maxIndices, EFreeFormMode mode, const Ptr<ITextureMap> & pTexture, 
             const Ptr<StreamBuffer> & pBuffer);

    virtual ~FreeForm();

    virtual void setWorldMatrix(const Core::Matrix4f & matrix);
    virtual void getWorldMatrix(Core::Matrix4f & matrix) const { matrix = _worldMatrix; };

    virtual void setMax(int32 maxVert, int32 maxIndices);
    virtual void getMax(int32 & maxVert, int32 & maxIndices);

    virtual void setMode(EFreeFormMode mode) { _mode = mode; };
    virtual EFreeFormMode getMode() const { return _mode; };

    virtual void setGlowFlag(bool glow) { _glowFlag = glow; };
    virtual bool getGlowFlag() const { return _glowFlag; };

    virtual void setWorldSpaceCoords(bool flag) { _isWorldSpaceCoords = flag; };
    virtual bool isWorldSpaceCoords() const { return _isWorldSpaceCoords; };

    virtual FreeFormData & beginUpdate() { return _data; };
    virtual void endUpdate();

    virtual void setPrecomputedAABox(const Core::AABoxf & box);
    virtual const Core::AABoxf & getPrecomputedAABox() const { return _localBox; }

    virtual void unsetPrecomputedAABox() { _usePrecomputedAABox = false; }
    virtual bool usePrecomputedAABox() const { return _usePrecomputedAABox; }

    virtual void setTexture(const Ptr<ITextureMap> & pTexture) { _pTexture = pTexture; };
    virtual Ptr<ITextureMap> getTexture() const { return _pTexture; };

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

    virtual void onDeviceLost();
    virtual void onDeviceReset();

    void sendData() const;

protected:
    Ptr<Gfx::IDevice>       _pDevice;
    Ptr<StreamBuffer>       _pBuffer;
    Ptr<GPUResourceLib>     _pResLib;

    bool                    _isAlive;
    bool                    _isVisible;
    bool                    _glowFlag;

    int32                   _maxVertices;
    int32                   _maxIndices;

    EFreeFormMode           _mode;
    Core::Matrix4f          _worldMatrix;
    Core::Vector3f          _position;
    Ptr<ITextureMap>        _pTexture;

    Core::AABoxf            _localBox;
    Core::AABoxf            _box;
    bool                    _usePrecomputedAABox;
    bool                    _isWorldSpaceCoords;

    FreeFormData            _data;
    Ptr<Gfx::IVertexBuffer> _pVBuffer;
    Ptr<Gfx::IIndexBuffer>  _pIBuffer;

    int32                   _vCount;
    int32                   _iCount;
    mutable bool            _isDirty;
};
}

#endif /*ABSTRACT_IDECAL_H_*/
