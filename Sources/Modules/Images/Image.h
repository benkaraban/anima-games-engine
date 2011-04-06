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
#ifndef IMAGE_H_
#define IMAGE_H_

#include <Core/List.h>
#include <Core/IO/Streams.h>
#include <Core/Bitmap.h>
#include "Pixel.h"

#pragma warning( push )
#pragma warning( disable : 4290 )

namespace Images
{
/**
 * Description rapide du fonctionnement du module Image
 * le type image est definit par la classe Images::Image
 * cette classe permet de definir un conteneur 2D pour les types definit par l'enumerateur Core::EBitmapFormat.
 * Afin de permettre de travailler sur une partie du conteneur2D (le buffer image) la structure View est definit.
 * elle permet d'obtenir une vue sur une region rectangulaire de l'image.
 * Les algotithme applicable sur une Image (et egalement sur une vue) sont definit dans l'header Operator.h
 *
 * La classe iterator2D permet de faciliter le parcourir sur le conteneur 2D (View ou Image)
 * L'interet de passer par iterator2D permet d'obtenir un code plus lisible et generique sur le type du code du pixel (uint8, int32, float)
 * par contre l'iterator2D ne gère pas la genericite sur le nombre de channel de l'image (GRAY, RGB, ARGB, ...)
 * pour gerer ce cas sans ralentir l'execution on est oblige de passer par un ensemble de classe template
 * qui rendrai la compilation un poil trop lente.
 *
 * Exemple de parcours d'une image codee en niveau de gris sur des octets (fonctionne de la même manière sur une View)
 * 1 / La methode brute a eviter:
 * for (int32 iY=0; iY<imgage.getHeight(); iY++)
 *  for (int32 iX=0; iX<image.getWidth(); iX++)
 *  {
 *      image.getData()[iX+iY*image.getRowStep()] = //la valeur que l'on souhaite donner au pixel (iX, iY)
 *  }
 *
 * 2/ en utilisant les macro permettant de parcourir en ligne puis en colonne
 * it2D_grayU8 itPix;
 * makeIterator(image, itPix);
 * foreach_line_i(itPix, iY)
 *  foreach_column_i(itPix, iX)
 *  {
 *     itPix->i = //la valeur que l'on souhaite donner au pixel (iX, iY)
 *  }
 *
 * 3/ utilisation de la macro foreach_pixel qui fait appel au macro foreach_line et foreach_column
 * it2D_grayU8 itPix;
 * makeIterator(image, itPix);
 * foreach_pixel_i(itPix, iY, iX)
 * {
 *    itPix->i = //la valeur que l'on souhaite donner au pixel (iX, iY)
 * }
 *
 * 4/ s'il n'est pas necessaire de disposer des indice iX et iY les macro peuvent etre directement appellees de la maniere suivante
 * it2D_grayU8 itPix;
 * makeIterator(image, itPix);
 * foreach_pixel(itPix)
 * {
 *    itPix->i = //la valeur que l'on souhaite donner au pixel
 * }
 *
 * 5/ pour une image de type rgb:
 * it2D_rgbU8 itPix;
 * makeIterator(image, itPix);
 * foreach_pixel(itPix)
 * {
 *    itPix->r = //la valeur que l'on souhaite donner a la composante rouge du pixel (iX, iY)
 *    itPix->g = //la valeur que l'on souhaite donner a la composante verte du pixel (iX, iY)
 *    itPix->b = //la valeur que l'on souhaite donner a la composante bleu du pixel (iX, iY)
 * }
 */

/**
 * Les type d'images sont exprimés du point de vue "par pixel". Par exemple, un pixel
 * au format ARGB_U8 est composé d'un int32 dont l'octet le plus fort est l'alpha
 * et l'octet le plus faible le bleu. Néanmoins, sur des machines x86 les octets
 * sont "à l'envers" à cause de l'endianess et donc si on lit le flux octet par octet, on va lire en premier
 * le bleu, puis le vert, puis le rouge, puis l'alpha, ce qui correspondrait à un
 * format BGRA_U8 et pas ARGB_U8. Selon la manière de voir le problème (par pixel ou par
 * octet) on a donc deux visions différentes. Direct3D utilise la vision "par pixel"
 * et le format ARGB_U8 correspond donc à l'ARGB_U8 de Direct3D. Sous OpenGL en revanche
 * on considère l'image comme un flux d'octet et le *même format* est appelé BGRA_U8.
 *
 * Contenu de la mémoire pour du ARGB_U8, octet par octet :
 *
 * [B1 G1 R1 A1  B2 G2 R2 A2 ... ] Données brutes, octet par octet
 * {  pixel 1  }{  pixel 2  }
 * [A1 R1 G1 B1  R2 G2 B2 A2 ... ] Données interprétées avec des int32
 * {   int32   }{   int32   }
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_ENUM_3 (EColorSpace, GRAY, RGB, ARGB)
LM_ENUM_5 (EChannelDepth, U8, U16, S16, S32, F32)
LM_API_IMG void imgTypeToColorAndDepth(const Core::EBitmapFormat type, EColorSpace& space, EChannelDepth& depth);
LM_API_IMG void colorAndDepthToimgType(const EColorSpace space, const EChannelDepth depth, Core::EBitmapFormat& type);
//-----------------------------------------------------------------------------
LM_ENUM_5(EImageFileFormat, IMAGE_FILE_JPG, IMAGE_FILE_PNG, IMAGE_FILE_BMP, IMAGE_FILE_TGA, IMAGE_FILE_PPM);
//-----------------------------------------------------------------------------
/**
 * Les différent mode d'interpolation:
 *  INTER_NEAREST : interpolation avec les valeur des plus proche voisin
 *  INTER_BILINEAR: interpolation bilineaire
 *  INTER_BICUBIC : interpolation bicubique
 */
LM_ENUM_3 (EInterpolationType, INTER_NEAREST, INTER_BILINEAR, INTER_BICUBIC)
//-----------------------------------------------------------------------------
LM_ENUM_2 (EImageCoord_t,
            COORD_PIXEL,
            COORD_NORM)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Core::EBitmapFormat          cvt(Core::EBitmapFormat format);
Core::EBitmapFormat cvt(Core::EBitmapFormat format);

/**
 * La classe stockant une image
 * les lignes des images sont alignees sur 16 bits
 */
class LM_API_IMG Image: public Core::Object
{
public:
    Image();
    ~Image();
    Image(const Ptr<Core::Bitmap> & pBitmap);
    Image(const Core::EBitmapFormat type, const int32 width, const int32 height);
    Image(void* data, int32 width, int32 height, int32 rowStep, int32 bitsPerPixel, uint32 redMask, uint32 greenMask,
          uint32 blueMask, bool topDown = false);

    void erase();

    /**
     * copie l'image src
     */
    void cloneTo(Image& dst) const;
    Ptr<Image> clone() const;

    /**
     * Le type de fichier est déterminé à partir de l'extension.
     */
    void loadFile(const String & fileName) throw(Core::IOException);

    /**
     * Le type de fichier est déterminé à partir de l'extension.
     * Pour le moment la fonction load retourne une image en 32bits
     * load2 permet d'eviter cela et retourne une image dans le format de l'uimage input
     * a terme load2 disparaitra et remplacera load
     */
    void load(Core::SeekableInputStream & input) throw(Core::IOException);
    void load2(Core::SeekableInputStream & input) throw(Core::IOException);

    /**
     * Le type de fichier est déterminé à partir de l'extension.
     * Note : pour le moment les formats d'image (tga, jpg, ..) ne sont pas tous compatible avec l'ensemble des type d'image (GARY_U8, RGB_U8, ...)
     * par exemple la sauvegarde d'une image GRAY_U8 au format tga ne fonctionne pas (freeImage ne semble pas gerer ce cas)
     * la partie suivante décrit pour chaque type d'image les formats a utiliser par defaut
     * GRAY_U8 : ppm
     * RGB_U8 : tga
     * ARGB_U8 :tga
     */
    void saveFile(const String & fileName) const;
    void save(Core::OutputStream & output, EImageFileFormat format = IMAGE_FILE_TGA) const;

    void resize(const int32 width, const int32 height, const EInterpolationType inter = INTER_BICUBIC);

    /**
     * Retourne une *copie* Bitmap
     */
    Ptr<Core::Bitmap> toBitmap();

    const EColorSpace& getColorSpace() const {return _colorSpace;}
    const EChannelDepth& getChannelDepth() const {return _chnDepth;}
    Core::EBitmapFormat getImgType() const;
    const int32& getWidth() const {return _width;}
    const int32& getHeight() const {return _height;}
    const int32& getRowStep() const {return _rowStep;}
    int32 getBytePerPixel() const;
    /**
     * Retourne les données brutes de l'image. Les lignes sont spécifiées du haut vers le
     * bas. Le premier pixel lu est donc le pixel en haut à gauche de l'image.
     */
    const byte* getData() const {return _data;}
    byte* getData() {return _data;}

    /**
     * retourne la valeur du pixel avec une précision  subPixel
     * interpolation bilineaire entre les quatre voisins
     * fonction tres lente ne devrai etre utilisee que pour les phases de prototypage et de debug
     */
    const Pixel getSubPixel(const EImageCoord_t coord, const float x, const float y) const;
    
    /**
     * modifie la valeur d'unb pixel
     * fonction tres lente ne devrai etre utilisee que pour les phases de prototypage et de debug
     */
    void setPixel(const EImageCoord_t coord, const float x, const float y, const Pixel pix);

    /**
     * Effectue une comparaison effective des images (ie. compare les pixels).
     */
    bool operator ==(const Image & image) const;
    bool operator !=(const Image & image) const;

protected:
    Image(const Image& img) {}
    Image& operator =(const Image & image) {return *this;}

public:
    byte* _data;
    int32 _width;
    int32 _height;
    int32 _rowStep;
    EColorSpace _colorSpace;
    EChannelDepth _chnDepth;
};

inline LM_API_IMG Ptr<Image> toImage(const Ptr<Core::Bitmap> & pBitmap)
{
    Ptr<Image> pImage(new Image(pBitmap));
    return pImage;
}

inline LM_API_IMG Core::String toString(const Image& img)
{
    Core::String str;
    str << L"width : " << img.getWidth() << L" height : " << img.getHeight();
    return str;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * View décrit une région rectangulaire d'une image
 * Cela permet de manipuler une partie du buffer image comme s'il s'agissait d'une image
 * Les operation sont ainsi defini sur des View et non des Images
 */
struct LM_API_IMG View
{
    const EColorSpace& getColorSpace() const {return pImg->getColorSpace();}
    const EChannelDepth& getChannelDepth() const {return pImg->getChannelDepth();}
    const int32& getWidth() const {return width;}
    const int32& getHeight() const {return height;}
    const int32& getRowStep() const {return pImg->getRowStep();}
    int32 getBytePerPixel() const {return pImg->getBytePerPixel();}
    Core::EBitmapFormat getImgType() const {return pImg->getImgType();};

    View(const Ptr<Image>& pImage);
    View(const Ptr<Image>& pImage, const int32 x, const int32 y, const int32 width, const int32 height);
    View(const Ptr<Image>& pImage, const EImageCoord_t typeCoord, const float x, const float y, const float width, float height);
    
    
    //position en pixel du point Haut gauche de la vue dans l'image (le pixel(x, y) est le premier pixel de la vue)
    int32 x;
    int32 y;

    //dimention de la vue en pixel
    int32 width;
    int32 height;

    const Pixel getSubPixel(const EImageCoord_t coord, const float x, const float y) const;
    
    //déplace la vue dans l'image de offsetX pixel en x et de offsetY pixel en y
    void move(const int32 offsetX, const int32 offsetY);
    void moveTo(const int32 posX, const int32 posY);

//private:
    Ptr<Image> pImg;
};
inline LM_API_IMG Core::String toString(const View& v)
{
    Core::String str;
    str << L"x : " << v.x << L" y : " << v.y << L" width : " << v.width << L" height : " << v.height;
    return str;
}
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
/**
 * La classe Iterator2D permet de se deplacer dans une View ou une Image
 * cette classe est pour le moment definit en template, et ne doit pas etre utilisee directement.
 * pour l'utiliser il suffit de faire appel au classe suivante, en fonction du type de l'image
 * it2D_grayU8 : pour une image GRAY_U8
 * it2D_rgbU8 : pour une image RGB_U8
 * it2D_argbU8 : pour une image ARGB_U8
 */
template<class Pixel_t>
class LM_API_IMG Iterator2D
{
public:
    void nextX() {_itPixel++;}
    void prevX() {_itPixel--;}
    void nextY_rowBegin() {_rowBegin = (Pixel_t*)((byte*)(_rowBegin) + _byteRowStep); _rowEnd = (Pixel_t*)((byte*)(_rowEnd) + _byteRowStep); _itPixel=_rowBegin;}

    Pixel_t* rowBegin() {return _rowBegin;}
    Pixel_t* rowEnd() {return _rowEnd;}
    Pixel_t* firstRow() {return _firstRow;}
    Pixel_t* lastRow() {return _lastRow;}
    bool isEnded() {return _rowBegin>=_lastRow;}
    bool isRowEnded() {return _itPixel>=_rowEnd;}
    Pixel_t& operator*(){return *_itPixel;}
    Pixel_t* operator->(){return _itPixel;}

public:
    Pixel_t* _itPixel;
    Pixel_t* _firstRow;
    Pixel_t* _lastRow;
    Pixel_t* _rowBegin;
    Pixel_t* _rowEnd;
    int32 _byteRowStep;

};
//-----------------------------------------------------------------------------
#define foreach_line(itPix) for (; itPix.isEnded() == false; itPix.nextY_rowBegin())
#define foreach_line_i(itPix, iY) for (int32 iY=0; itPix.isEnded() == false; itPix.nextY_rowBegin(), iY++)
#define foreach_column(itPix) for(; itPix.isRowEnded() == false; itPix.nextX())
#define foreach_column_i(itPix, iX) for(int32 iX=0; itPix.isRowEnded() == false; itPix.nextX(), iX++)
#define foreach_pixel(itPix) foreach_line(itPix) foreach_column(itPix)
#define foreach_pixel_i(itPix, iY, iX) foreach_line_i(itPix, iY) foreach_column_i(itPix, iX)
//-----------------------------------------------------------------------------
template class Iterator2D<grayU8>;
template class Iterator2D<rgbU8>;
template class Iterator2D<argbU8>;
template class Iterator2D<grayU16>;
template class Iterator2D<rgbU16>;
template class Iterator2D<argbU16>;
template class Iterator2D<grayS16>;
template class Iterator2D<rgbS16>;
template class Iterator2D<argbS16>;
template class Iterator2D<grayS32>;
template class Iterator2D<rgbS32>;
template class Iterator2D<argbS32>;
template class Iterator2D<grayF32>;
template class Iterator2D<rgbF32>;
template class Iterator2D<argbF32>;
typedef LM_API_IMG Iterator2D<grayU8> it2D_grayU8;
typedef LM_API_IMG Iterator2D<rgbU8> it2D_rgbU8;
typedef LM_API_IMG Iterator2D<argbU8> it2D_argbU8;
typedef LM_API_IMG Iterator2D<grayU16> it2D_grayU16;
typedef LM_API_IMG Iterator2D<rgbU16> it2D_rgbU16;
typedef LM_API_IMG Iterator2D<argbU16> it2D_argbU16;
typedef LM_API_IMG Iterator2D<grayS16> it2D_grayS16;
typedef LM_API_IMG Iterator2D<rgbS16> it2D_rgbS16;
typedef LM_API_IMG Iterator2D<argbS16> it2D_argbS16;
typedef LM_API_IMG Iterator2D<grayS32> it2D_grayS32;
typedef LM_API_IMG Iterator2D<rgbS32> it2D_rgbS32;
typedef LM_API_IMG Iterator2D<argbS32> it2D_argbS32;
typedef LM_API_IMG Iterator2D<grayF32> it2D_grayF32;
typedef LM_API_IMG Iterator2D<rgbF32> it2D_rgbF32;
typedef LM_API_IMG Iterator2D<argbF32> it2D_argbF32;

template<class TPixel>
inline void copy(Iterator2D<TPixel> & iDst, Iterator2D<TPixel> & iSrc)
{
    while(!iDst.isEnded())
    {
        while(!iDst.isRowEnded())
        {
            *iDst = *iSrc;
            iDst.nextX();
            iSrc.nextX();
        }
        
        iSrc.nextY_rowBegin();
        iDst.nextY_rowBegin();
    }
}
//-----------------------------------------------------------------------------
// Pour creer un iterator2D
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_grayU8& itPix);
LM_API_IMG void makeIterator(View& view, it2D_rgbU8& itPix);
LM_API_IMG void makeIterator(View& view, it2D_argbU8& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayU8& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbU8& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbU8& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_grayU8& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_rgbU8& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_argbU8& itPix);
LM_API_IMG void makeIterator(View& view, it2D_grayU16& itPix);
LM_API_IMG void makeIterator(View& view, it2D_rgbU16& itPix);
LM_API_IMG void makeIterator(View& view, it2D_argbU16& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayU16& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbU16& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbU16& itPix);
LM_API_IMG void makeIterator(const Image& img, it2D_grayU16& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_rgbU16& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_argbU16& itPix);
LM_API_IMG void makeIterator(View& view, it2D_grayS16& itPix);
LM_API_IMG void makeIterator(View& view, it2D_rgbS16& itPix);
LM_API_IMG void makeIterator(View& view, it2D_argbS16& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayS16& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbS16& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbS16& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_grayS16& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_rgbS16& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_argbS16& itPix);
LM_API_IMG void makeIterator(View& view, it2D_grayS32& itPix);
LM_API_IMG void makeIterator(View& view, it2D_rgbS32& itPix);
LM_API_IMG void makeIterator(View& view, it2D_argbS32& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayS32& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbS32& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbS32& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_grayS32& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_rgbS32& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_argbS32& itPix);
LM_API_IMG void makeIterator(View& view, it2D_grayF32& itPix);
LM_API_IMG void makeIterator(View& view, it2D_rgbF32& itPix);
LM_API_IMG void makeIterator(View& view, it2D_argbF32& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayF32& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbF32& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbF32& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_grayF32& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_rgbF32& itPix);
LM_API_IMG void makeIterator(Image& img, it2D_argbF32& itPix);
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// la classe it2D regroupe l'ensemble des iterator possible
//-----------------------------------------------------------------------------
union it2D
{
    it2D_grayU8 as_grayU8;
    it2D_rgbU8 as_rgbU8;
    it2D_argbU8 as_argbU8;
    it2D_grayU16 as_grayU16;
    it2D_rgbU16 as_rgbU16;
    it2D_argbU16 as_argbU16;
    it2D_grayS16 as_grayS16;
    it2D_rgbS16 as_rgbS16;
    it2D_argbS16 as_argbS16;
    it2D_grayS32 as_grayS32;
    it2D_rgbS32 as_rgbS32;
    it2D_argbS32 as_argbS32;
    it2D_grayF32 as_grayF32;
    it2D_rgbF32 as_rgbF32;
    it2D_argbF32 as_argbF32;
};
LM_API_IMG void makeIterator(View& view, it2D& itPix);
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D& itPix);
LM_API_IMG void makeIterator(Image& img, it2D& itPix);
//-----------------------------------------------------------------------------
} //namespace Images

#pragma warning( pop )

#endif /* IMAGE_H_ */
