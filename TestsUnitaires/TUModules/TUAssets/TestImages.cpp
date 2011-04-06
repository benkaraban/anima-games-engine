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
#include <UTests.h>
#include <Core/Math/Random.h>
#include <Core/IO/FileStream.h>
#include <Images/Image.h>
#include <Core/Math/Color.h>
#include <Workflow/ZonePacking.h>

const String IMAGE_IN_24_PNG(L"Data/TUData/Textures/24bpp.png");
const String IMAGE_IN_32_TGA(L"Data/TUData/Textures/32bpp.tga");
const String IMAGE_IN_24_JPG(L"Data/TUData/Textures/rabbit.jpg");

const String IMAGE_OUT_24_BMP(L"Data/TUData/Tmp/24bpp.bmp");
const String IMAGE_OUT_32_TGA(L"Data/TUData/Tmp/32bpp.tga");
const String IMAGE_OUT_24_PNG(L"Data/TUData/Tmp/rabbit.png");

using namespace Images;
using namespace Core;

void testImages()
{
    UTEST_PACKAGE("Images");

    Ptr<Images::Image > imgIn24PNG (new Images::Image);
    Ptr<Images::Image > imgIn32TGA (new Images::Image);
    Ptr<Images::Image > imgIn24JPG (new Images::Image);

    Ptr<Images::Image > imgOut24BMP (new Images::Image);
    Ptr<Images::Image > imgOut32TGA (new Images::Image);
    Ptr<Images::Image > imgOut24PNG (new Images::Image);

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Color conversion");
    ColorARGB8 color = Core::toARGB8(0xFF, 0x99, 0xBB, 0xAA);
    UTEST_EQU(color, 0xAAFF99BB);
    UTEST_EQU(red(color), 0xFF);
    UTEST_EQU(green(color), 0x99);
    UTEST_EQU(blue(color), 0xBB);
    UTEST_EQU(alpha(color), 0xAA);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Default image");
    UTEST_EQU(imgIn24PNG->getWidth(), 1);
    UTEST_EQU(imgIn24PNG->getHeight(), 1);
    UTEST_EQU(imgIn24PNG->getImgType(), Core::ARGB_U8);
    UTEST_EQU(imgIn24PNG->getBytePerPixel(), 4);
    ColorARGB8 * pixels = (ColorARGB8*)imgIn24PNG->getData();
    UTEST_EQU(red(pixels[0]), 0);
    UTEST_EQU(green(pixels[0]), 0);
    UTEST_EQU(blue(pixels[0]), 0);
    UTEST_EQU(alpha(pixels[0]), 0);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"24bits PNG File load");
    imgIn24PNG->loadFile(IMAGE_IN_24_PNG);
    UTEST_EQU(imgIn24PNG->getWidth(), 40);
    UTEST_EQU(imgIn24PNG->getHeight(), 30);
    UTEST_EQU(imgIn24PNG->getImgType(), Core::ARGB_U8);
    UTEST_EQU(imgIn24PNG->getBytePerPixel(), 4);
    ColorARGB8 * pixels = (ColorARGB8*)imgIn24PNG->getData();
    UTEST_EQU(red(pixels[0]), 200);
    UTEST_EQU(green(pixels[0]), 100);
    UTEST_EQU(blue(pixels[0]), 50);
    UTEST_EQU(alpha(pixels[0]), 255);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"32bits TGA File load");
    imgIn32TGA->loadFile(IMAGE_IN_32_TGA);
    UTEST_EQU(imgIn32TGA->getWidth(), 40);
    UTEST_EQU(imgIn32TGA->getHeight(), 30);
    UTEST_EQU(imgIn32TGA->getImgType(), Core::ARGB_U8);
    UTEST_EQU(imgIn32TGA->getBytePerPixel(), 4);
    ColorARGB8 * pixels = (ColorARGB8*)imgIn32TGA->getData();
    UTEST_EQU(red(pixels[0]), 200);
    UTEST_EQU(green(pixels[0]), 100);
    UTEST_EQU(blue(pixels[0]), 50);
    UTEST_EQU(alpha(pixels[0]), 65);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"24bits JPG Stream load");
    Core::FileInputStream file;
    file.open(IMAGE_IN_24_JPG);
    UTEST_EQU(file.isOpened(), true);
    imgIn24JPG->load(file);
    UTEST_EQU(imgIn24JPG->getWidth(), 100);
    UTEST_EQU(imgIn24JPG->getHeight(), 80);
    UTEST_EQU(imgIn24JPG->getImgType(), Core::ARGB_U8);
    UTEST_EQU(imgIn24JPG->getBytePerPixel(), 4);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    // Save
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"24bits BMP File save");
    imgIn24PNG->saveFile(IMAGE_OUT_24_BMP);
    UTEST_EQU(imgIn24PNG->getWidth(), 40);
    UTEST_EQU(imgIn24PNG->getHeight(), 30);
    UTEST_EQU(imgIn24PNG->getImgType(), Core::ARGB_U8);
    UTEST_EQU(imgIn24PNG->getBytePerPixel(), 4);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"32bits TGA Stream save");
    Core::FileOutputStream file;
    file.open(IMAGE_OUT_32_TGA, Core::FileOutputStream::FILE_TRUNCATE);
    UTEST_EQU(file.isOpened(), true);
    imgIn32TGA->save(file, Images::IMAGE_FILE_TGA);
    UTEST_EQU(imgIn32TGA->getWidth(), 40);
    UTEST_EQU(imgIn32TGA->getHeight(), 30);
    UTEST_EQU(imgIn32TGA->getImgType(), Core::ARGB_U8);
    UTEST_EQU(imgIn32TGA->getBytePerPixel(), 4);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"24bits PNG File save");
    imgIn24JPG->saveFile(L"Data/TUData/Tmp/24bpp2.bmp");
    imgIn24JPG->saveFile(IMAGE_OUT_24_PNG);
    UTEST_EQU(imgIn24JPG->getWidth(), 100);
    UTEST_EQU(imgIn24JPG->getHeight(), 80);
    UTEST_EQU(imgIn24JPG->getImgType(), Core::ARGB_U8);
    UTEST_EQU(imgIn24JPG->getBytePerPixel(), 4);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    // Reload
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"24bits BMP File reload");
    imgOut24BMP->loadFile(IMAGE_OUT_24_BMP);
    UTEST_EQU(imgOut24BMP->getWidth(), 40);
    UTEST_EQU(imgOut24BMP->getHeight(), 30);
    UTEST_EQU(imgOut24BMP->getImgType(), Core::ARGB_U8);
    UTEST_EQU(imgOut24BMP->getBytePerPixel(), 4);
    ColorARGB8 * pixels = (ColorARGB8*)imgOut24BMP->getData();
    UTEST_EQU(red(pixels[0]), 200);
    UTEST_EQU(green(pixels[0]), 100);
    UTEST_EQU(blue(pixels[0]), 50);
    UTEST_EQU(alpha(pixels[0]), 255);
    UTEST_EQU(*imgOut24BMP, *imgIn24PNG);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"32bits TGA Stream reload");
    Core::FileInputStream file;
    file.open(IMAGE_OUT_32_TGA);
    UTEST_EQU(file.isOpened(), true);
    imgOut32TGA->load(file);
    UTEST_EQU(*imgIn32TGA, *imgOut32TGA);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"24bits PNG File reload");
    imgOut24PNG->loadFile(IMAGE_OUT_24_PNG);
    UTEST_EQU(*imgIn24JPG, *imgOut24PNG);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Missing file loading");
    UTEST_CATCH_EXCEPTION(Core::IOException, imgOut24PNG->loadFile(L"does_not_exist"));
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Image copy");

    Ptr<Images::Image> imgCopy1 (new Images::Image);
    imgOut24BMP->cloneTo(*imgCopy1);
    UTEST_EQU(*imgCopy1, *imgOut24BMP);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Image pack");

    Core::List<Workflow::Zone> zones;
    Core::List<bool> isPacked;
    Core::List<Workflow::ZonePack> pack;

    for(int32 ii=0; ii < 100; ii++)
    {
        Workflow::Zone zone;
        int32 size = Core::irand() % 32 + 1;
        zone.width = size + Core::irand() % 16;
        zone.height = size + Core::irand() % 16;

        zones.push_back(zone);
        isPacked.push_back(false);
    }

    Workflow::packZones(pack, zones, 256, 256);

    for(int32 ii=0; ii < pack.size(); ii++)
    {
        UTEST_EQU(pack[ii].width, 256);
        UTEST_EQU(pack[ii].height, 256);

        Ptr<Images::Image> pImage(new Images::Image(Core::ARGB_U8, pack[ii].width, pack[ii].height));

        Images::it2D_argbU8 iPix;
        Images::makeIterator(pImage, iPix);

        foreach_pixel(iPix)
        {
            iPix->r = 0x00;
            iPix->g = 0x00;
            iPix->b = 0x00;
            iPix->a = 0xFF;
        }

        for(int32 iZone=0; iZone < pack[ii].zones.size(); iZone++)
        {
            const Workflow::ZonePack::PackedZone & pzone = pack[ii].zones[iZone];

            int32 width = pzone.x2 - pzone.x1 + 1;
            int32 height = pzone.y2 - pzone.y1 + 1;

            UTEST_EQU(isPacked[pzone.id], false);
            UTEST_EQU(zones[pzone.id].width, width);
            UTEST_EQU(zones[pzone.id].height, height);

            isPacked[pzone.id] = true;

            Images::View view1(pImage, pzone.x1, pzone.y1, width, height);
            Images::View view2(pImage, pzone.x1 + 1, pzone.y1 + 1, std::max(1, width - 2), std::max(1, height - 2));

            Images::makeIterator(view1, iPix);

            foreach_pixel(iPix)
            {
                iPix->r = 0xFF;
                iPix->g = 0xFF;
                iPix->b = 0xFF;
                iPix->a = 0xFF;
            }

            Images::makeIterator(view2, iPix);

            foreach_pixel(iPix)
            {
                iPix->r = 0xFF;
                iPix->g = 0x00;
                iPix->b = 0x00;
                iPix->a = 0xFF;
            }
        }

        pImage->saveFile(L"Local/Pack" + Core::toString(ii, 4) + L".png");
    }

    for(int32 ii=0; ii < isPacked.size(); ii++)
        UTEST_EQU(isPacked[ii], true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------

    /*TODO
    UTEST_START_TEST(L"Image resize");

    imgIn24PNG.loadFile(IMAGE_IN_24_PNG);
    UTEST_EQU(imgIn24PNG.getWidth(), 40);
    UTEST_EQU(imgIn24PNG.getHeight(), 30);

    Images::Image rescaled1(imgIn24PNG, 50, 20);
    UTEST_EQU(rescaled1.getWidth(), 50);
    UTEST_EQU(rescaled1.getHeight(), 20);

    Images::Image expected;
    expected.loadFile(L"Data/TUData/Images/rescaled.png");
    UTEST_EQU(rescaled1, expected);

    Images::Image rescaled2;
    rescaled2.resize(imgIn24PNG, 50, 20);
    UTEST_EQU(rescaled1, rescaled2);

    Images::Image rescaled3(imgIn24PNG);
    UTEST_EQU(rescaled3, imgIn24PNG);
    rescaled3.resize(50, 20);
    UTEST_EQU(rescaled3, rescaled2);

    UTEST_END_TEST;*/
}
