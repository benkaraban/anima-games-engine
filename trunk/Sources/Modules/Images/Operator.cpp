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
#include <Core/Logger.h>
#include <Core/Math/MathTools.h>

#include "Operator.h"

#include <OpenCV/cv.h>

//cvReleaseImageHeader
//-----------------------------------------------------------------------------
struct WrapperIplImage : public IplImage
{
    WrapperIplImage() {};
    ~WrapperIplImage() {if(roi!=NULL){cvResetImageROI(this);}};
};
void ImageToWrapperIplImage(const Images::Image& img, WrapperIplImage& ipl)
{
    ipl.nSize = sizeof(IplImage);
    ipl.ID = 0;
    ipl.roi = NULL; // Image ROI. If NULL, the whole image is selected.
    ipl.origin = 0; // 0 - top-left origin, 1 - bottom-left origin (Windows bitmaps style).

    switch(img._chnDepth)
    {
        case Images::U8: ipl.depth = IPL_DEPTH_8U; break;
        case Images::U16: ipl.depth = IPL_DEPTH_16U; break;
        case Images::S16: ipl.depth = IPL_DEPTH_16S; break;
        case Images::S32: ipl.depth = IPL_DEPTH_32S; break;
        case Images::F32: ipl.depth = IPL_DEPTH_32F; break;
    }

    switch(img._colorSpace)
    {
        case Images::GRAY:
            ipl.nChannels = 1; // Most of OpenCV functions support 1,2,3 or 4 channels
            ipl.dataOrder = 0; // 0 - interleaved color channels, 1 - separate color channels. cvCreateImage can only create interleaved images
        break;
        case Images::RGB:
            ipl.nChannels = 3; // Most of OpenCV functions support 1,2,3 or 4 channels
            ipl.dataOrder = 0; // 0 - interleaved color channels, 1 - separate color channels. cvCreateImage can only create interleaved images
        break;
        case Images::ARGB:
            ipl.nChannels = 4; // Most of OpenCV functions support 1,2,3 or 4 channels
            ipl.dataOrder = 0; // 0 - interleaved color channels, 1 - separate color channels. cvCreateImage can only create interleaved images
        break;
    }

    ipl.width = img._width;
    ipl.height = img._height;
    ipl.widthStep = img._rowStep;
    ipl.imageSize = ipl.height * ipl.widthStep;
    ipl.imageData = (char*)(img._data);
    ipl.imageDataOrigin = (char*)(img._data);  // Pointer to very origin of image data

    ipl.maskROI = NULL;      // Must be NULL.
    ipl.imageId = NULL;                // Must be NULL.
    ipl.tileInfo = NULL; // Must be NULL.
    /*
    ipl.BorderConst[4];    // Ditto
    ipl.channelSeq[4];     // ditto

    ipl.BorderMode[4];     // Ignored by OpenCV.
    ipl.colorModel[4];     // Ignored by OpenCV
    ipl.alphaChannel;      // Ignored by OpenCV
    ipl.align;             // Alignment of image rows (4 or 8). OpenCV ignores it and uses widthStep instead.
  */
}

void ViewToWrapperIplImage(const Images::View& view, WrapperIplImage& ipl)
{
    ImageToWrapperIplImage(*(view.pImg), ipl);
    cvSetImageROI( &ipl, cvRect( view.x, view.y, view.width, view.height ));
}
//-----------------------------------------------------------------------------
namespace Images
{
//-----------------------------------------------------------------------------
// resize
//-----------------------------------------------------------------------------
LM_API_IMG void resize(const Image& src, Image& dst, const EInterpolationType inter)
{
    if (src.getColorSpace() != dst.getColorSpace() ||
        src.getChannelDepth() != dst.getChannelDepth())
    {
        ERR << L"error Images::resize"; // TODO gerer l'exeption
    }

    switch(src.getChannelDepth())
    {
        case U8:
        case U16:
        case S16:
        case S32:
        case F32:
            switch(src.getColorSpace())
            {
                case GRAY:
                case RGB:
                case ARGB:
                {
                    WrapperIplImage iplSrc, iplDst;
                    int interpolation = 0;

                    ImageToWrapperIplImage(src, iplSrc);
                    ImageToWrapperIplImage(dst, iplDst);

                    switch(inter)
                    {
                        case INTER_NEAREST: interpolation = CV_INTER_NN; break;
                        case INTER_BILINEAR: interpolation = CV_INTER_LINEAR; break;
                        case INTER_BICUBIC: interpolation = CV_INTER_CUBIC; break;
                    }

                    cvResize(&iplSrc, &iplDst, interpolation);
                }
                break;
            }
        break;
    }
}
//-----------------------------------------------------------------------------
LM_API_IMG void resizeByStep(const Ptr<Image>& pSrc, Ptr<Image>& pDst, const EInterpolationType)
{
    if((pSrc->getWidth() > 2 * pDst->getWidth()) || (pSrc->getHeight() > 2 * pDst->getHeight()))
    {
        int32 width = std::max(1, pSrc->getWidth() / 2);
        int32 height = std::max(1, pSrc->getHeight() / 2);
        Ptr<Image> pInterImage(new Image(pDst->getImgType(), width, height));
        resize(pSrc, pInterImage);
        resizeByStep(pInterImage, pDst);
    }
    else
    {
        resize(pSrc, pDst);
    }
}
//-----------------------------------------------------------------------------
LM_API_IMG void resize(const Ptr<Image>& pSrc, Ptr<Image>& pDst, const EInterpolationType inter)
{
    View viewSrc(pSrc);
    View viewDst(pDst);
    resize(viewSrc, viewDst, inter);
}
//-----------------------------------------------------------------------------
LM_API_IMG void resize(const View& src, View& dst, const EInterpolationType inter)
{
    if (src.getColorSpace() != dst.getColorSpace() ||
        src.getChannelDepth() != dst.getChannelDepth())
    {
        ERR << L"error Images::resize"; // TODO gerer l'exeption
        ERR << L"Color Space : " << src.getColorSpace() << L" " << dst.getColorSpace();
        ERR << L"Channel Depth : " << src.getChannelDepth() << L" " << dst.getChannelDepth();
    }

    switch(src.getChannelDepth())
    {
        case U8:
        case U16:
        case S16:
        case S32:
        case F32:
            switch(src.getColorSpace())
            {
                case GRAY:
                case RGB:
                case ARGB:
                {
                    WrapperIplImage iplSrc, iplDst;
                    int interpolation = 0;

                    ViewToWrapperIplImage(src, iplSrc);
                    ViewToWrapperIplImage(dst, iplDst);

                    switch(inter)
                    {
                        case INTER_NEAREST: interpolation = CV_INTER_NN; break;
                        case INTER_BILINEAR: interpolation = CV_INTER_LINEAR; break;
                        case INTER_BICUBIC: interpolation = CV_INTER_CUBIC; break;
                    }

                    cvResize(&iplSrc, &iplDst, interpolation);
                }
                break;
            }
        break;
    }
}
//-----------------------------------------------------------------------------
// fill
//-----------------------------------------------------------------------------
LM_API_IMG void setToZero(Ptr<Image>& pImg)
{
    View view(pImg);
    setToZero(view);
}
LM_API_IMG void setToZero(View& view)
{
    Pixel color;
    color.as_argbF32.b = color.as_argbF32.g = color.as_argbF32.r = color.as_argbF32.a = 0;
    fill(view, color);
}
template <typename Pixel_t>
void fill_T(Iterator2D<Pixel_t>& itPix, const Pixel_t& color)
{
    foreach_pixel(itPix)
    {
        *itPix = color;
    }
}

//-----------------------------------------------------------------------------
LM_API_IMG void fill(Ptr<Image>& pImg, const Pixel& color)
{
    View view(pImg);
    fill(view, color);
}
//-----------------------------------------------------------------------------
LM_API_IMG void fill(View& view, const Pixel& color)
{
    it2D itPix;
    makeIterator(view, itPix);

    Core::EBitmapFormat type = view.getImgType();

    switch(type)
    {
        case Core::GRAY_U8: fill_T(itPix.as_grayU8, color.as_grayU8); break;
        case Core::RGB_U8: fill_T(itPix.as_rgbU8, color.as_rgbU8); break;
        case Core::ARGB_U8: fill_T(itPix.as_argbU8, color.as_argbU8); break;
        case Core::GRAY_U16: fill_T(itPix.as_grayU16, color.as_grayU16); break;
        case Core::RGB_U16: fill_T(itPix.as_rgbU16, color.as_rgbU16); break;
        case Core::ARGB_U16: fill_T(itPix.as_argbU16, color.as_argbU16); break;
        case Core::GRAY_S16: fill_T(itPix.as_grayS16, color.as_grayS16); break;
        case Core::RGB_S16: fill_T(itPix.as_rgbS16, color.as_rgbS16); break;
        case Core::ARGB_S16: fill_T(itPix.as_argbS16, color.as_argbS16); break;
        case Core::GRAY_S32: fill_T(itPix.as_grayS32, color.as_grayS32); break;
        case Core::RGB_S32: fill_T(itPix.as_rgbS32, color.as_rgbS32); break;
        case Core::ARGB_S32: fill_T(itPix.as_argbS32, color.as_argbS32); break;
        case Core::GRAY_F32: fill_T(itPix.as_grayF32, color.as_grayF32); break;
        case Core::RGB_F32: fill_T(itPix.as_rgbF32, color.as_rgbF32); break;
        case Core::ARGB_F32: fill_T(itPix.as_argbF32, color.as_argbF32); break;
    }
}
//-----------------------------------------------------------------------------
// findMinMax
//-----------------------------------------------------------------------------
template <typename Pixel_t>
void minMax_T(Iterator2D<Pixel_t>& itPix, Pixel_t& colorMin, Pixel_t& colorMax)
{
    colorMin = Pixel_t::MAX_VALUE;
    colorMax = Pixel_t::MIN_VALUE;

    foreach_pixel(itPix)
    {
        if (colorMax <*itPix ) colorMax = *itPix;
        if (*itPix < colorMin) colorMin = *itPix;
    }
}
//-----------------------------------------------------------------------------
LM_API_IMG void minMax(const Ptr<Image>& pImg, Pixel& colorMin, Pixel& colorMax)
{
    View view(pImg);
    minMax(view, colorMin, colorMax);
}
//-----------------------------------------------------------------------------
LM_API_IMG void minMax(View& view, Pixel& colorMin, Pixel& colorMax)
{
    it2D itPix;
    makeIterator(view, itPix);

    Core::EBitmapFormat type = view.getImgType();

    switch(type)
    {
        case Core::GRAY_U8: minMax_T(itPix.as_grayU8, colorMin.as_grayU8, colorMax.as_grayU8); break;
        case Core::RGB_U8: minMax_T(itPix.as_rgbU8, colorMin.as_rgbU8, colorMax.as_rgbU8); break;
        case Core::ARGB_U8: minMax_T(itPix.as_argbU8, colorMin.as_argbU8, colorMax.as_argbU8); break;
        case Core::GRAY_U16: minMax_T(itPix.as_grayU16, colorMin.as_grayU16, colorMax.as_grayU16); break;
        case Core::RGB_U16: minMax_T(itPix.as_rgbU16, colorMin.as_rgbU16, colorMax.as_rgbU16); break;
        case Core::ARGB_U16: minMax_T(itPix.as_argbU16, colorMin.as_argbU16, colorMax.as_argbU16); break;
        case Core::GRAY_S16: minMax_T(itPix.as_grayS16, colorMin.as_grayS16, colorMax.as_grayS16); break;
        case Core::RGB_S16: minMax_T(itPix.as_rgbS16, colorMin.as_rgbS16, colorMax.as_rgbS16); break;
        case Core::ARGB_S16: minMax_T(itPix.as_argbS16, colorMin.as_argbS16, colorMax.as_argbS16); break;
        case Core::GRAY_S32: minMax_T(itPix.as_grayS32, colorMin.as_grayS32, colorMax.as_grayS32); break;
        case Core::RGB_S32: minMax_T(itPix.as_rgbS32, colorMin.as_rgbS32, colorMax.as_rgbS32); break;
        case Core::ARGB_S32: minMax_T(itPix.as_argbS32, colorMin.as_argbS32, colorMax.as_argbS32); break;
        case Core::GRAY_F32: minMax_T(itPix.as_grayF32, colorMin.as_grayF32, colorMax.as_grayF32); break;
        case Core::RGB_F32: minMax_T(itPix.as_rgbF32, colorMin.as_rgbF32, colorMax.as_rgbF32); break;
        case Core::ARGB_F32: minMax_T(itPix.as_argbF32, colorMin.as_argbF32, colorMax.as_argbF32); break;
    }
}
//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------
LM_API_IMG void copy(const Ptr<Image>& pSrc, Ptr<Image>& pDst)
{
    View viewSrc(pSrc);
    View viewDst(pDst);
    copy(viewSrc, viewDst);
}
//-----------------------------------------------------------------------------
LM_API_IMG void copy(const View& src, View& dst)
{
    if (src.getColorSpace() != dst.getColorSpace() ||
        src.getChannelDepth() != dst.getChannelDepth() ||
        src.getWidth() != dst.getWidth() ||
        src.getHeight() != dst.getHeight())
    {
        String message;
        message << L"Images::copy - View format source is different from View format destination :"
                << L"\ncolor : "<<src.getColorSpace() <<L" - "<<dst.getColorSpace()
                << L"\ndepth : "<<src.getChannelDepth() <<L" - "<<dst.getChannelDepth()
                << L"\nwidth : "<<src.getWidth() <<L" - "<<dst.getWidth()
                << L"\nheight: "<<src.getHeight() <<L" - "<<dst.getHeight()
                << L"\ncopy aborted";
        throw Core::Exception(message);
    }

    switch(src.getChannelDepth())
    {
        case U8:
        case U16:
        case S16:
        case S32:
        case F32:
            switch(src.getColorSpace())
            {
                case GRAY:
                case RGB:
                case ARGB:
                {
                    WrapperIplImage iplSrc, iplDst;

                    ViewToWrapperIplImage(src, iplSrc);
                    ViewToWrapperIplImage(dst, iplDst);

                    cvCopy(&iplSrc, &iplDst);
                }
                break;
            }
        break;
    }
}
//-----------------------------------------------------------------------------
// convert
//-----------------------------------------------------------------------------
LM_API_IMG void convertChannelDepth(const Ptr<Image>& pSrc, Ptr<Image>& pDst)
{
    if (pSrc->getColorSpace() != pDst->getColorSpace() ||
        pSrc->getWidth() != pDst->getWidth() ||
        pSrc->getHeight() != pDst->getHeight())
    {
        ERR << L"error Images::convertChannelDepth"; // TODO gerer l'exeption
    }

    switch(pSrc->getChannelDepth())
    {
        case U8:
        case U16:
        case S16:
        case S32:
        case F32:
            switch(pSrc->getColorSpace())
            {
                case GRAY:
                {
                    WrapperIplImage iplSrc, iplDst;

                    ImageToWrapperIplImage(*pSrc, iplSrc);
                    ImageToWrapperIplImage(*pDst, iplDst);

                    double minVal, maxVal;
                    cvMinMaxLoc(&iplSrc, &minVal, &maxVal, NULL, NULL, 0);

                    if (maxVal != 0)
                        cvConvertScale(&iplSrc, &iplDst, 255.0/maxVal, 0);
                    else
                    {
                        ERR << L"images::convertChannelDepth max = 0";
                        cvConvertScale(&iplSrc, &iplDst, 255.0, 0);
                    }
                }
                break;
                case RGB:
                case ARGB:
                {
                    WrapperIplImage iplSrc, iplDst;

                    ImageToWrapperIplImage(*pSrc, iplSrc);
                    ImageToWrapperIplImage(*pDst, iplDst);

                    cvConvert(&iplSrc, &iplDst);
                }
                break;
            }
        break;
    }
}
//-----------------------------------------------------------------------------
//pour pSrc et pDst ayant un channel depth identique
LM_API_IMG void convertColorSpace(const Ptr<Image>& pSrc, Ptr<Image>& pDst)
{
    if (pSrc->getChannelDepth() != pDst->getChannelDepth() ||
        pSrc->getWidth() != pDst->getWidth() ||
        pSrc->getHeight() != pDst->getHeight())
    {
        ERR << L"error Images::convertColorSpace"; // TODO gerer l'exeption
    }


    switch(pSrc->getColorSpace())
    {
        case GRAY:
        {
            ERR << L"error Images::convertColorSpace Gray";
        }
        break;

        case RGB:
        {
            WrapperIplImage iplSrc;
            ImageToWrapperIplImage(*pSrc, iplSrc);

            switch(pDst->getColorSpace())
            {
                case GRAY:
                {
                    WrapperIplImage iplDst;
                    ImageToWrapperIplImage(*pDst, iplDst);
                    cvCvtColor(&iplSrc, &iplDst, CV_BGRA2GRAY);
                }
                break;

                case RGB:
                {
                    ERR << L"error Images::convertColorSpace RGB -> RGB";
                }
                break;

                case ARGB:
                {
                    ERR << L"error Images::convertColorSpace RGB -> ARGB";
                }
                break;
            }
        }
        break;

        case ARGB:
        {
            WrapperIplImage iplSrc;
            ImageToWrapperIplImage(*pSrc, iplSrc);

            switch(pDst->getColorSpace())
            {
                case GRAY:
                {
                    WrapperIplImage iplDst;
                    ImageToWrapperIplImage(*pDst, iplDst);
                    cvCvtColor(&iplSrc, &iplDst, CV_BGRA2GRAY);
                }
                break;

                case RGB:
                {
                    ERR << L"error Images::convertColorSpace ARGB -> RGB";
                }
                break;

                case ARGB:
                {
                    ERR << L"error Images::convertColorSpace ARGB -> ARGB";
                }
                break;
            }
        }
        break;
    }

}
//-----------------------------------------------------------------------------
LM_API_IMG void convert(const Ptr<Image>& pSrc, Ptr<Image>& pDst)
{
    if (pSrc->getWidth() != pDst->getWidth() ||
        pSrc->getHeight() != pDst->getHeight())
    {
        ERR << L"error Images::convert"; // TODO gerer l'exeption
    }

    if ( pSrc->getColorSpace() == pDst->getColorSpace() )
    {
        convertChannelDepth(pSrc, pDst);
    }
    else if( pSrc->getChannelDepth() == pDst->getChannelDepth() )
    {
        convertColorSpace(pSrc, pDst);
    }
    else
    {
        Core::EBitmapFormat type;
        colorAndDepthToimgType(pSrc->getColorSpace(), pDst->getChannelDepth(), type);
        Ptr<Image> pInterImg(new Image(type, pDst->getWidth(), pDst->getHeight()));
        convert(pSrc, pInterImg);
        convert(pInterImg, pDst);
    }
}
//-----------------------------------------------------------------------------
LM_API_IMG void blur(const Ptr<Image>& pSrc, Ptr<Image>& pDst, const int scale, const EBlurType blurType)
{
    View viewSrc(pSrc);
    View viewDst(pDst);
    blur(viewSrc, viewDst, scale, blurType);
}

LM_API_IMG void blur(const View& src, View& dst, const int scale, const EBlurType blurType)
{
    if (src.getImgType() != dst.getImgType() ||
        src.getWidth() != dst.getWidth() ||
        src.getHeight() != dst.getHeight())
    {
        ERR << L"error Images::blur"; // TODO gerer l'exeption
    }

    if (src.getImgType()!= Core::GRAY_U8 && src.getImgType()!= Core::GRAY_F32 && src.getImgType()!= Core::RGB_U8)
    {
        ERR << L"error Images::blur non implemente"; // TODO gerer l'exeption
    }
    else
    {

    }

    WrapperIplImage iplSrc, iplDst;
    int32 kernelSize = Core::L_pow(2, scale-1);
    kernelSize = kernelSize*2+1;

    ViewToWrapperIplImage(src, iplSrc);
    ViewToWrapperIplImage(dst, iplDst);

    cvSmooth(&iplSrc, &iplDst, CV_GAUSSIAN, kernelSize);
}
//-----------------------------------------------------------------------------
} // namespace Images
