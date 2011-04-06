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
#include <Renderer/Common/Tools.h>
#include <Core/Set.h>
#include <Core/Logger.h>

namespace Renderer
{
Ptr<Core::Bitmap> borderDiffuse(const Ptr<Core::Bitmap> & pSrcBMP)
{
    using Core::PixelARGB8;

    int32 width = pSrcBMP->getWidth();
    int32 height = pSrcBMP->getHeight();
    byte * pData = new byte [width * height * 4];

    PixelARGB8 * pDst = (PixelARGB8 *)pData;
    Core::PixelARGB8 * pSrc = (PixelARGB8 *)pSrcBMP->getData();

    for(int32 y1=0; y1 < height; y1++)
    {
        int32 y0 = std::max(0, y1 - 1);
        int32 y2 = std::min(height - 1, y1 + 1);

        for(int32 x1=0; x1 < width; x1++)
        {
            int32 x0 = std::max(0, x1 - 1);
            int32 x2 = std::min(width - 1, x1 + 1);

            PixelARGB8 p00 = pSrc[y0 * width + x0];
            PixelARGB8 p10 = pSrc[y1 * width + x0];
            PixelARGB8 p20 = pSrc[y2 * width + x0];

            PixelARGB8 p01 = pSrc[y0 * width + x1];
            PixelARGB8 p11 = pSrc[y1 * width + x1];
            PixelARGB8 p21 = pSrc[y2 * width + x1];
            
            PixelARGB8 p02 = pSrc[y0 * width + x2];
            PixelARGB8 p12 = pSrc[y1 * width + x2];
            PixelARGB8 p22 = pSrc[y2 * width + x2];
            
            static const int32 THRESHOLD = 100;

            if(p11.a > THRESHOLD)
                pDst[y1 * width + x1] = p11;
            else if(p01.a > THRESHOLD)
                pDst[y1 * width + x1] = p01;
            else if(p21.a > THRESHOLD)
                pDst[y1 * width + x1] = p21;
            else if(p10.a > THRESHOLD)
                pDst[y1 * width + x1] = p10;
            else if(p12.a > THRESHOLD)
                pDst[y1 * width + x1] = p12;
            else if(p00.a > 0)
                pDst[y1 * width + x1] = p00;
            else if(p02.a > 0)
                pDst[y1 * width + x1] = p02;
            else if(p20.a > 0)
                pDst[y1 * width + x1] = p20;
            else if(p22.a > 0)
                pDst[y1 * width + x1] = p22;
            else
                pDst[y1 * width + x1] = p11;
        }
    }

    return Ptr<Core::Bitmap>(new Core::Bitmap(pData, width, height, Core::ARGB_U8));
}

void getCubeMapMatrix(Gfx::ECubeMapFace face, Core::Matrix4f & mat, const Core::Vector3f & pos)
{
    switch(face)
    {
    case Gfx::CUBE_FACE_POSITIVE_X:
        mat.set( 0.0,  0.0,  1.0, pos.x,
                 0.0,  1.0,  0.0, pos.y,
                -1.0,  0.0,  0.0, pos.z,
                 0.0,  0.0,  0.0, 1.0);
    break;
    case Gfx::CUBE_FACE_NEGATIVE_X:
        mat.set( 0.0,  0.0, -1.0, pos.x,
                 0.0,  1.0,  0.0, pos.y,
                 1.0,  0.0,  0.0, pos.z,
                 0.0,  0.0,  0.0, 1.0);
    break;
    case Gfx::CUBE_FACE_POSITIVE_Y:
        mat.set( 1.0,  0.0,  0.0, pos.x,
                 0.0,  0.0,  1.0, pos.y,
                 0.0, -1.0,  0.0, pos.z,
                 0.0,  0.0,  0.0, 1.0);
    break;
    case Gfx::CUBE_FACE_NEGATIVE_Y:
        mat.set( 1.0,  0.0,  0.0, pos.x,
                 0.0,  0.0, -1.0, pos.y,
                 0.0,  1.0,  0.0, pos.z,
                 0.0,  0.0,  0.0, 1.0);
    break;
    case Gfx::CUBE_FACE_POSITIVE_Z:
        mat.set( 1.0,  0.0,  0.0, pos.x,
                 0.0,  1.0,  0.0, pos.y,
                 0.0,  0.0,  1.0, pos.z,
                 0.0,  0.0,  0.0, 1.0);
    break;
    case Gfx::CUBE_FACE_NEGATIVE_Z:
        mat.set(-1.0,  0.0,  0.0, pos.x,
                 0.0,  1.0,  0.0, pos.y,
                 0.0,  0.0, -1.0, pos.z,
                 0.0,  0.0,  0.0, 1.0);
    break;
    }
}

void LM_API_RDR setSampler(Gfx::RSSamplerDesc & sampler, EFilterLevel level)
{
    switch(level)
    {
    case FILTER_BILINEAR:
        sampler.filter = Gfx::FILTER_MIN_MAG_LINEAR_MIP_POINT;
        sampler.maxAnisotropy = 0;
        break;
    case FILTER_TRILINEAR:
        sampler.filter = Gfx::FILTER_MIN_MAG_MIP_LINEAR;
        sampler.maxAnisotropy = 0;
        break;
    case FILTER_ANISO_2X:
        sampler.filter = Gfx::FILTER_ANISOTROPIC;
        sampler.maxAnisotropy = 2;
        break;
    case FILTER_ANISO_4X:
        sampler.filter = Gfx::FILTER_ANISOTROPIC;
        sampler.maxAnisotropy = 4;
        break;
    case FILTER_ANISO_8X:
        sampler.filter = Gfx::FILTER_ANISOTROPIC;
        sampler.maxAnisotropy = 8;
        break;
    }
}

Gfx::EMultiSamplingType cvt(EMSAALevel msaa)
{
    switch(msaa)
    {
    case MSAA_NONE: return Gfx::MSAA_NONE;
    case MSAA_2X: return Gfx::MSAA_2X;
    case MSAA_4X: return Gfx::MSAA_4X;
    case MSAA_8X: return Gfx::MSAA_8X;
    }

    return Gfx::MSAA_NONE;
}

EMSAALevel cvt(Gfx::EMultiSamplingType msaa)
{
    switch(msaa)
    {
    case Gfx::MSAA_NONE: return MSAA_NONE;
    case Gfx::MSAA_2X: return MSAA_2X;
    case Gfx::MSAA_4X: return MSAA_4X;
    case Gfx::MSAA_8X: return MSAA_8X;
    }

    return MSAA_NONE;
}

String toStringRatio(float r)
{
    if(Core::isEquivalent(r, 16.0f / 9.0f))
        return L"16:9";
    if(Core::isEquivalent(r, 16.0f / 10.0f))
        return L"16:10";
    if(Core::isEquivalent(r, 5.0f / 3.0f))
        return L"5:3";
    if(Core::isEquivalent(r, 5.0f / 4.0f))
        return L"5:4";
    if(Core::isEquivalent(r, 4.0f / 3.0f))
        return L"4:3";
    return Core::toString(r);
}

void filterResolutions(Core::List<Resolution> & resolutions, int32 minWidth, int32 minHeight)
{
    Core::List<Resolution> result;

    for(int32 ii=0; ii < resolutions.size(); ii++)
    {
        if(resolutions[ii].width >= minWidth && resolutions[ii].height >= minHeight)
            result.push_back(resolutions[ii]);
    }

    resolutions.swap(result);
}

void filterResolutions(const Core::List<Gfx::DisplayMode> & modes,
                       const Gfx::DisplayMode &             initialMode,
                       Core::List<Resolution> &             allRes,
                       Core::List<Resolution> &             goodRes,
                       bool                                 logOn)
{
	Core::Set<Resolution> resSet;
    Resolution bestRes(0, 0);

    for(int32 ii=0; ii < modes.size(); ii++)
    {
        const Gfx::DisplayMode & mode = modes[ii];

        if(mode.format == Gfx::DPF_X8R8G8B8)
        {
            Resolution res(mode.width, mode.height);

            if((res.width * res.height) >= (bestRes.width * bestRes.height))
                bestRes = res;

		    resSet.insert(res);
        }
    }

    float ratio = float(initialMode.width) / float(initialMode.height);

    if(logOn)
    {
        INF << L"Best resolution      : " << Core::toString(bestRes.width) << L"x" << Core::toString(bestRes.height) << L"\n";
        INF << L"Initial resolution   : " << Core::toString(initialMode.width) << L"x" << Core::toString(initialMode.height) << L"\n";
        INF << L"Preferred ratio      : " << toStringRatio(ratio) << L"\n";
    }

    goodRes.clear();
    allRes.clear();

    for(Core::Set<Resolution>::const_iterator iRes=resSet.begin(); iRes != resSet.end(); ++iRes)
    {
        const Resolution & res = *iRes;
        if(Core::isEquivalent(float(res.width) / float(res.height), ratio))
            goodRes.push_back(res);

        allRes.push_back(res);
    }

    if(logOn)
    {
        INF << L"Prefered resolutions {\n";

        for(int32 ii=0; ii < goodRes.size(); ii++)
            INF << L"  " << Core::toString(goodRes[ii].width) << L"x" << Core::toString(goodRes[ii].height) << L"\n";

        INF << L"}\n";
    }
}

Gfx::DisplayMode bestFit(const Ptr<Gfx::IDriver> & pDriver, int32 id, int32 width, int32 height, Gfx::EDisplayPixelFormat pf)
{
    Gfx::DisplayMode result(width, height, 0, pf);
    const Core::List<Gfx::DisplayMode> & formats = pDriver->getDisplayFormats(id);
    int32 minDiff = Core::INT32_MAX;

    for(int32 ii=0; ii < formats.size(); ii++)
    {
        const Gfx::DisplayMode & dm = formats[ii];

        if(dm.format == pf)
        {
            int32 dx = abs(dm.width - width);
            int32 dy = abs(dm.height - height);

            int32 diff = dx * height + dy * width;
            if(diff < minDiff)
            {
                result.width = dm.width;
                result.height = dm.height;
                minDiff = diff;
            }
        }
    }

    return result;
}

}
