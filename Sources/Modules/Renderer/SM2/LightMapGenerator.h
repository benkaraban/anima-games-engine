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
#ifndef RENDERER_LIGHTMAPGENERATOR_H_
#define RENDERER_LIGHTMAPGENERATOR_H_

#include <Gfx/IDevice.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/ScreenQuad.h>
#include <Renderer/Common/PackedVertices.h>
#include <Renderer/IRenderer.h>

namespace Renderer
{
struct LightMapGenerationInfos
{
    struct PixelInfos
    {
        Core::Vector3f position;
        Core::Vector3f normal;
        bool           used;

        PixelInfos();
    };

    bool   debugPositionBitmap;
    bool   debugNormalBitmap;

    Ptr<Core::Bitmap> pPositionBitmap;
    Ptr<Core::Bitmap> pNormalBitmap;

    bool    internalTextureBorder;

    Ptr<TextureMap>  pNormalMap;

    Core::List<Core::List<PixelInfos> > pixels;
};

class LightMapCoordsGenerator : public Core::Object
{
public:
    LightMapCoordsGenerator(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<PackedVerticesFormat> & pFormat, const Ptr<ScreenQuad> & pScreenQuad);
    bool initialise();

    void generateFromModel(
        LightMapGenerationInfos & lm, 
        const Core::Matrix4f & world,
        const IPackedStaticVertices & vertices, 
        int32 width,
        int32 height,
        bool superSample
        );

    void resize(Ptr<Gfx::ITexture2D> & pTex, Ptr<Gfx::IRenderTargetView> & pView, int32 srcWidth, int32 srcHeight, int32 dstWidth, int32 dstHeight);

protected:
    Ptr<Gfx::IDevice>            _pDevice;
    Ptr<ShaderLib>               _pShaderLib;
    Ptr<PackedVerticesFormat>    _pFormat;
    Ptr<ScreenQuad>              _pScreenQuad;

    int32                        _resolution;
    bool                         _screenshotOn;

    Gfx::GlobalRenderState       _stateGen;
    Ptr<Gfx::IVertexFormat>      _pFormatGen;
    Ptr<Gfx::IVertexShader>      _pVShaderGen;
    Ptr<Gfx::IPixelShader>       _pPShaderGen;
    int32                        _idWorldGen;
    int32                        _idTextureSizeGen;
    int32                        _idIsTextureBorderOnGen;
    int32                        _idSamplerNormalGen;
    int32                        _idIsNormalMapDXT5Gen;
    int32                        _idIsNormalMapOnGen;

    Gfx::GlobalRenderState       _stateMip;
    Ptr<Gfx::IPixelShader>       _pPShaderMip;
    int32                        _idSourceSizeMip;
    int32                        _idSamplerImageMip;
    int32                        _idIsAlphaWeightedMip;


};

}

#endif
