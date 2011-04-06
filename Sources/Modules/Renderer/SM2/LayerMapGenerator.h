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
#ifndef RENDERER_LAYERMAPGENERATOR_H_
#define RENDERER_LAYERMAPGENERATOR_H_

#include <Gfx/IDevice.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/ScreenQuad.h>
#include <Renderer/Common/PackedVertices.h>
#include <Renderer/IRenderer.h>

namespace Renderer
{
class LayerMapGenerator : public Core::Object
{
public:
    LayerMapGenerator(const Ptr<Gfx::IDevice> & pDevice, 
                      const Ptr<ShaderLib> & pShaderLib, 
                      const Ptr<PackedVerticesFormat> & pFormat,
                      const Ptr<TextureMap> & pDefaultWhiteTexture,
                      const Ptr<TextureMap> & pDefaultBumpTexture);
    bool initialise();

    virtual Ptr<Core::Bitmap> generateFromModel(const LayerMapInfos & lm, const Ptr<IMeshInstance> & pMesh);

protected:
    virtual void internalGenerateFromModel(
        const LayerMapInfos & lm, 
        const Ptr<IMeshInstance> & pMesh,
        const Ptr<Gfx::IRenderTargetView> & pView,
        bool isChain,
        bool isFirstLayerDetailMap);

    Ptr<TextureMap> getDefaultTexture(bool isNormal);

    Ptr<Gfx::IDevice>            _pDevice;
    Ptr<ShaderLib>               _pShaderLib;
    Ptr<PackedVerticesFormat>    _pFormat;
    Ptr<TextureMap>              _pDefaultWhiteTexture;
    Ptr<TextureMap>              _pDefaultBumpTexture;

    int32                        _resolution;
    bool                         _screenshotOn;

    Gfx::GlobalRenderState       _stateGen;
    Ptr<Gfx::IVertexFormat>      _pFormatGen;
    Ptr<Gfx::IVertexShader>      _pVShaderGen;
    Ptr<Gfx::IPixelShader>       _pPShaderGen;

    int32                        _idWorldGen;
    int32                        _idTextureSizeGen;
    int32                        _idIsTextureBorderOnGen;
    int32                        _idIsLayer1DetailLayer;

    static const int32 LAYER_COUNT = 8;

    int32                        _idMinPos;
    int32                        _idRangePos;
    int32                        _idIsNormalMap;
    int32                        _idIsChain;
    int32                        _idMatRotationLayer;
    int32                        _idIsDXT5Norm[LAYER_COUNT];
    int32                        _idGenerateU[LAYER_COUNT];
    int32                        _idGenerateV[LAYER_COUNT];
    int32                        _idColor[LAYER_COUNT];
    int32                        _idSamplerSource[LAYER_COUNT];
    int32                        _idSamplerMask[LAYER_COUNT];
    int32                        _idMaskContrast[LAYER_COUNT];
    int32                        _idNormalStrength[LAYER_COUNT];
    int32                        _idMapType[LAYER_COUNT];
    int32                        _idSamplerSize[LAYER_COUNT];   //la taille de la texture envoyer pour generer le calque
    
};

}

#endif
