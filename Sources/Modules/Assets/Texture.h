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
#ifndef ASSETS_TEXTURE_H_
#define ASSETS_TEXTURE_H_

#include <Core/IO/Streams.h>
#include <Core/Standard.h>
#include <Core/List.h>

namespace Assets
{
/**
 * Format de texture. Il s'agit de formats "prêts à l'emplois" pour le GPU.
 * - TEX_FORMAT_ARGB8 : image 32 bits classique.
 * - TEX_FORMAT_ARGB16 : image 64 bits, chaque composant est un entier sur 16 bits.
 * - TEX_FORMAT_ARGB16F : image 64 bits, chaque composant est un half-float sur 16 bits.
 * - TEX_FORMAT_ARGB32F : image 128 bits, chaque composant est un float sur 32 bits.
 * - TEX_FORMAT_DXTC1 : image compressée en DXTC1. Le format compresse une image 24 bits avec
 * un ratio de 8:1, ce qui en fait le format à utiliser en priorité si une image ne contient
 * pas de couche alpha.
 * - TEX_FORMAT_DXTC3 : image compressée en DXTC3. Le format compresse une image 32 bits avec
 * un ratio de 4:1.
 * - TEX_FORMAT_DXTC5 : image compressée en DXTC5. Le format compresse une image 32 bits avec
 * un ratio de 4:1. La différence entre le DXTC3 et le DXTC5 est que le format DXTC3 est censé
 * moins altérer les images avec des variations brusques d'alpha (par exemple quand la couche
 * alpha n'est qu'un masque booléen pour un sprite, avec juste un peu d'antialias sur les bords).
 * A l'inverse, le format DXTC5 est censé mieux conserver les variations progressives d'alpha
 * comme celles d'un halo ou d'une particule. La différence n'est pas toujours aussi simple
 * et le mieux est encore de tester les deux compressions et de conserver celle qui rend le
 * mieux.
 *
 * A noter que tous les formats DXTC sont des formats de compression avec perte qui sont
 * utilisables par le GPU : les textures sont stockées en mémoire vidéo dans ce format.
 * Non seulement cela permet d'économiser de la mémoire mais cela permet de baisser
 * la bande passante utilisée pour les transferts ce qui améliore considérablement les
 * performances (de l'ordre de 30-40%). Le gain peut être utilisé pour améliorer la
 * qualité : on peut utiliser une texture de 2Nx2N au lieu d'une texture de NxN. Et même
 * compressée une texture de 2Nx2N est beaucoup (mais vraiment beaucoup) plus nette.
 *
 * Pour cette raison, il faut utiliser autant que possible les formats DXTC. Si une image
 * n'a pas de couche alpha, il faut la compresser en DXTC1. Si elle en a un il faut essayer
 * les formats DXTC3 et DXTC5. A noter qu'il ne faut jamais utiliser les formats DXTC3 et
 * DXTC5 pour des textures sans alpha puisque cela revient à doubler l'utilisation mémoire
 * sans raison (la compression des couleurs RGB est la même pour les trois formats).
 *
 * En réalité le format TEX_FORMAT_ARGB8 ne devrait être utilisé que dans le cas où une
 * texture ne passe pas bien dans un format compressé et qu'il est très important
 * qu'elle soit d'une excellente qualité, par exemple pour stocker du texte dans
 * l'interface.
 *
 * Les formats avec plus de 8 bits par composant ne servent normalement pas à stocker des
 * images "normales" mais des informations de lumière, pour gérer des effets de HDR. Ils
 * peuvent par exemple stocker des lightmaps ou des textures d'éclairage environemental.
 */
LM_ENUM_4(ETextureFormat,
         TEX_FORMAT_ARGB8,
//         TEX_FORMAT_ARGB16,
//         TEX_FORMAT_ARGB16F,
//         TEX_FORMAT_ARGB32F,
         TEX_FORMAT_DXTC1,
         TEX_FORMAT_DXTC3,
         TEX_FORMAT_DXTC5);

/**
 * Permet d'identifier les différentes faces d'un cube d'une texture cubique.
 */
LM_ENUM_6(ECubeFace,
         CUBE_POSITIVE_X,
         CUBE_NEGATIVE_X,
         CUBE_POSITIVE_Y,
         CUBE_NEGATIVE_Y,
         CUBE_POSITIVE_Z,
         CUBE_NEGATIVE_Z);

const int32 ECubeFace_SIZE = int32(CUBE_NEGATIVE_Z) + 1;

/**
 * Représente un image d'une texture. Une texture peut être elle même composée
 * de plusieurs TextureImage, chacune correspondant à un niveau de mipmap.
 * Toutes les TextureImage d'une même texture sont stockées dans le même format.
 */
class LM_API_ASS TextureImage : public Core::Object, public Core::Serializable
{
public:
    TextureImage();
    TextureImage(const TextureImage & tex);
    TextureImage(int32 width, int32 height, ETextureFormat format,
                 const void * pData, int32 dataSize);

    const void * getData() const { return _data.data(); };
    int32 getDataSize() const;
    int32 getWidth() const { return _width; };
    int32 getHeight() const { return _height; };
    ETextureFormat getFormat() const { return _format; };
    int32 getBitsPerPixels() const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    TextureImage & operator = (const TextureImage & tex);

    bool operator == (const TextureImage & texture) const;
    bool operator != (const TextureImage & texture) const;

protected:
    int32            _width;
    int32            _height;
    ETextureFormat   _format;
    Core::List<byte> _data;
};

class CubeTexture;

/**
 * Représente une texture à deux dimensions. La texture peut avoir plusieurs
 * niveaux de mipmap.
 */
class LM_API_ASS Texture : public Core::Object, public Core::Serializable
{
public:
    Texture();
    Texture(const Texture & tex);
    Texture(const Core::List<TextureImage> & images);
    Texture(const Core::List<Core::List<TextureImage> > & images);	//_images[mipmap][depth]

    ETextureFormat getFormat() const { return _format; };
    int32 getMipmapCount() const { return _images.size(); };
    int32 getWidth() const { return _width; };
    int32 getHeight() const { return _height; };
	int32 getDepth() const { return _depth; };
    const Core::List<TextureImage> & getImage(int32 level) const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    virtual void readDDS(Core::InputStream & input);
    virtual void writeDDS(Core::OutputStream & output) const;

    Texture & operator = (const Texture & tex);

    bool operator == (const Texture & texture) const;
    bool operator != (const Texture & texture) const;

protected:
    static void readDDS(Core::InputStream & input, Core::List<Texture> & textures);
    static void writeDDS(Core::OutputStream & output, const Core::List<Texture> & textures);

    friend class CubeTexture;

    int32            _width;
    int32            _height;
	int32            _depth;	//nbSlice
    int32            _flags;
    ETextureFormat   _format;
    Core::List<Core::List<TextureImage> >  _images;	//_images[mipmap][depth]
};

/**
 * Représente une texture cubique, qui définit une image pour chaque
 * face d'un cube. La texture peut disposer de plusieurs niveaux de mipmap.
 */
class LM_API_ASS CubeTexture : public Core::Object, public Core::Serializable
{
public:
    CubeTexture();
    CubeTexture(const CubeTexture & tex);
    CubeTexture(const Core::List<Texture> & faces);

    ETextureFormat getFormat() const { return _format; };
    int32 getMipmapCount() const { return _mipmapCount; };
    int32 getWidth() const { return _width; };
    int32 getHeight() const { return _height; };
    const Core::List<TextureImage> & getImage(ECubeFace face, int32 level) const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    virtual void readDDS(Core::InputStream & input);
    virtual void writeDDS(Core::OutputStream & output) const;

    CubeTexture & operator = (const CubeTexture & tex);

    bool operator == (const CubeTexture & texture) const;
    bool operator != (const CubeTexture & texture) const;

protected:
    int32                _width;
    int32                _height;
    ETextureFormat       _format;
    int32                _mipmapCount;
    int32                _flags;
    Core::List<Texture>  _faces;
};

}

#endif /*ASSETS_TEXTURE_H_*/
