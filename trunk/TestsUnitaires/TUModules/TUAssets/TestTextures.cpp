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
#include <Core/Math/MathTools.h>
#include <Core/Math/Color.h>
#include <Core/IO/BufferStream.h>
#include <Core/IO/FileStream.h>
#include <Images/Image.h>
#include <Workflow/TexCompression.h>

const String TEXTURE_SOURCE_COLOR   (L"Data/TUData/Textures/source-colormap.tga");
const String TEXTURE_SOURCE_HEIGHT  (L"Data/TUData/Textures/source-heightmap.png");
const String TEXTURE_SOURCE_LIGHT   (L"Data/TUData/Textures/24bpp.png");
const String TEXTURE_SOURCE_NORM    (L"Data/TUData/Textures/source-normmap.jpg");
const String TEXTURE_SOURCE_CUBE0   (L"Data/TUData/Textures/cube00.png");
const String TEXTURE_SOURCE_CUBE1   (L"Data/TUData/Textures/cube01.png");
const String TEXTURE_SOURCE_CUBE2   (L"Data/TUData/Textures/cube02.png");
const String TEXTURE_SOURCE_CUBE3   (L"Data/TUData/Textures/cube03.png");
const String TEXTURE_SOURCE_CUBE4   (L"Data/TUData/Textures/cube04.png");
const String TEXTURE_SOURCE_CUBE5   (L"Data/TUData/Textures/cube05.png");
const String TEXTURE_SOURCE_VOLUME0 (L"Data/TUData/Textures/Anim00/00.jpg");
const String TEXTURE_SOURCE_VOLUME1 (L"Data/TUData/Textures/Anim00/01.jpg");
const String TEXTURE_SOURCE_VOLUME2 (L"Data/TUData/Textures/Anim00/02.jpg");
const String TEXTURE_SOURCE_VOLUME3 (L"Data/TUData/Textures/Anim00/03.jpg");

class ProgressTU : public Core::IProgressObs
{
public:
    ProgressTU() : _curStep(0)
    {};

    virtual void progress(float p)
    {
        int32 step = int32(10.0f * p);
        while(step > _curStep)
        {
            //std::cout << "=" << std::flush;
            _curStep ++;
        }
    };

protected:
    int32   _curStep;
};

void testTexture(const Ptr<Images::Image>& pImage, Workflow::TextureOptions options)
{
    Ptr<Assets::Texture> pTexture = Workflow::generateTexture(options, pImage);
    UTEST_EQU(pTexture->getWidth(), Core::nextPowerOfTwo(pImage->getWidth()));
    UTEST_EQU(pTexture->getHeight(), Core::nextPowerOfTwo(pImage->getHeight()));
    UTEST_EQU(pTexture->getFormat(), options.format);

    Assets::Texture cloneTexture(*pTexture);
    UTEST_EQU(cloneTexture.getWidth(), Core::nextPowerOfTwo(pImage->getWidth()));
    UTEST_EQU(cloneTexture.getHeight(), Core::nextPowerOfTwo(pImage->getHeight()));
    UTEST_EQU(cloneTexture.getFormat(), options.format);

    Assets::Texture copyTexture;
    copyTexture = *pTexture;
    UTEST_EQU(copyTexture, *pTexture);

    Core::BufferStream buffer;
    pTexture->write(buffer);
    Assets::Texture streamedTexture;
    streamedTexture.read(buffer);
    UTEST_EQU(streamedTexture, *pTexture);

    pTexture->writeDDS(buffer);
    Assets::Texture ddsTexture;
    ddsTexture.readDDS(buffer);
}

void testTextures()
{
    UTEST_PACKAGE("Textures");

    Core::List<Ptr<Images::Image> > imagesCube;
    Core::List<Ptr<Images::Image> > imagesVolume;
    Ptr<Images::Image> imageColor (new Images::Image);
    Ptr<Images::Image> imageHeight (new Images::Image);
    Ptr<Images::Image> imageLight (new Images::Image);
    Ptr<Images::Image> imageNorm (new Images::Image);

    UTEST_START_TEST(L"Texture source load");

    imageColor->loadFile(TEXTURE_SOURCE_COLOR);
    imageHeight->loadFile(TEXTURE_SOURCE_HEIGHT);
    imageLight->loadFile(TEXTURE_SOURCE_LIGHT);
    imageNorm->loadFile(TEXTURE_SOURCE_NORM);

    imagesCube.resize(6);
    imagesCube[0] = Ptr<Images::Image>(new Images::Image);
    imagesCube[1] = Ptr<Images::Image>(new Images::Image);
    imagesCube[2] = Ptr<Images::Image>(new Images::Image);
    imagesCube[3] = Ptr<Images::Image>(new Images::Image);
    imagesCube[4] = Ptr<Images::Image>(new Images::Image);
    imagesCube[5] = Ptr<Images::Image>(new Images::Image);
    
    imagesCube[0]->loadFile(TEXTURE_SOURCE_CUBE0);
    imagesCube[1]->loadFile(TEXTURE_SOURCE_CUBE1);
    imagesCube[2]->loadFile(TEXTURE_SOURCE_CUBE2);
    imagesCube[3]->loadFile(TEXTURE_SOURCE_CUBE3);
    imagesCube[4]->loadFile(TEXTURE_SOURCE_CUBE4);
    imagesCube[5]->loadFile(TEXTURE_SOURCE_CUBE5);

    imagesVolume.resize(4);
    imagesVolume[0] = Ptr<Images::Image>(new Images::Image());
    imagesVolume[1] = Ptr<Images::Image>(new Images::Image());
    imagesVolume[2] = Ptr<Images::Image>(new Images::Image());
    imagesVolume[3] = Ptr<Images::Image>(new Images::Image());

    imagesVolume[0]->loadFile(TEXTURE_SOURCE_VOLUME0);;
    imagesVolume[1]->loadFile(TEXTURE_SOURCE_VOLUME1);;
    imagesVolume[2]->loadFile(TEXTURE_SOURCE_VOLUME2);;
    imagesVolume[3]->loadFile(TEXTURE_SOURCE_VOLUME3);;

    UTEST_END_TEST;

    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Texture ARGB8 color no mipmap");

    Workflow::TextureOptions options;
    options.generateMipmap = false;
    options.format         = Assets::TEX_FORMAT_ARGB8;
    options.texType        = Workflow::TEXTURE_COLORMAP;

    testTexture(imageColor, options);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Texture DXTC1 color triangle mipmap");

    Workflow::TextureOptions options;
    options.format       = Assets::TEX_FORMAT_DXTC1;
    options.mipmapFilter = Workflow::MIPMAP_FILTER_TRIANGLE;
    options.texType      = Workflow::TEXTURE_COLORMAP;

    testTexture(imageColor, options);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Texture DXTC3 color kaiser mipmap");

    Workflow::TextureOptions options;
    options.format       = Assets::TEX_FORMAT_DXTC3;
    options.mipmapFilter = Workflow::MIPMAP_FILTER_KAISER;
    options.texType      = Workflow::TEXTURE_COLORMAP;

    testTexture(imageColor, options);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Texture DXTC5 color box mipmap");

    ProgressTU prog;

    Workflow::TextureOptions options;
    options.format       = Assets::TEX_FORMAT_DXTC5;
    options.mipmapFilter = Workflow::MIPMAP_FILTER_BOX;
    options.texType      = Workflow::TEXTURE_COLORMAP;
    options.pProgressObs = &prog;

    testTexture(imageColor, options);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Texture ARGB8 height nomip");

    Workflow::TextureOptions options;
    options.generateMipmap = false;
    options.format         = Assets::TEX_FORMAT_ARGB8;
    options.texType        = Workflow::TEXTURE_HEIGHTMAP;

    testTexture(imageHeight, options);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Texture DXTC5 height box mipmap");

    Workflow::TextureOptions options;
    options.format       = Assets::TEX_FORMAT_DXTC5;
    options.mipmapFilter = Workflow::MIPMAP_FILTER_BOX;
    options.texType      = Workflow::TEXTURE_HEIGHTMAP;

    testTexture(imageHeight, options);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Texture ARGB8 norm nomip");

    Workflow::TextureOptions options;
    options.generateMipmap = false;
    options.format         = Assets::TEX_FORMAT_ARGB8;
    options.texType        = Workflow::TEXTURE_NORMALMAP;

    testTexture(imageNorm, options);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Texture DXTC5 norm box mipmap");

    Workflow::TextureOptions options;
    options.format       = Assets::TEX_FORMAT_DXTC5;
    options.mipmapFilter = Workflow::MIPMAP_FILTER_BOX;
    options.texType      = Workflow::TEXTURE_NORMALMAP;

    testTexture(imageNorm, options);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Texture DXTC1 light triangle mipmap");

    Workflow::TextureOptions options;
    options.format       = Assets::TEX_FORMAT_DXTC1;
    options.mipmapFilter = Workflow::MIPMAP_FILTER_TRIANGLE;
    options.texType      = Workflow::TEXTURE_NORMALMAP;

    testTexture(imageLight, options);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"VolumeTexture ARGB8 color box mipmap");

    Workflow::TextureOptions options;
    options.format       = Assets::TEX_FORMAT_ARGB8;
    options.mipmapFilter = Workflow::MIPMAP_FILTER_BOX;
    options.texType      = Workflow::TEXTURE_COLORMAP;

    Ptr<Assets::Texture> pTexture = Workflow::generateTexture3D(options, imagesVolume);
    UTEST_EQU(pTexture->getDepth(), 4);

    Core::BufferStream buffer;
    pTexture->writeDDS(buffer);

    Assets::Texture texture;
    texture.readDDS(buffer);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"CubeTexture DXTC1 color box mipmap");

    Workflow::TextureOptions options;
    options.format       = Assets::TEX_FORMAT_DXTC1;
    options.mipmapFilter = Workflow::MIPMAP_FILTER_BOX;
    options.texType      = Workflow::TEXTURE_COLORMAP;

    Ptr<Assets::CubeTexture> pTexture = Workflow::generateTextureCube(options, imagesCube);

    Core::FileOutputStream outputStream;
    outputStream.open(L"Data/TUData/Tmp/cube-dxtc1-box.dds");
    pTexture->writeDDS(outputStream);
    outputStream.close();

    Core::BufferStream buffer;
    pTexture->writeDDS(buffer);

    Assets::CubeTexture texture;
    texture.readDDS(buffer);

    UTEST_END_TEST;
}
