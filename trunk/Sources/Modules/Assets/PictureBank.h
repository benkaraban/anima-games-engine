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
#ifndef ASSETS_PICTURE_BANK_H_
#define ASSETS_PICTURE_BANK_H_

#include <Core/List.h>
#include <Core/Map.h>
#include <Core/IO/Streams.h>
#include <Assets/Texture.h>

namespace Assets
{
class LM_API_ASS PictureBank : public Core::Object, public Core::Serializable
{
public:
    struct Pic
    {
        int32   textureIndex;

        int32   width;
        int32   height;

        float   u1;
        float   v1;

        float   u2;
        float   v2;
    };

    PictureBank();
    PictureBank(const Core::List<Ptr<Texture> > & textures, const Core::Map<String, Pic> & pics);

    const Core::List<Ptr<Texture> > & getTextureList() const { return _textures; };

    const Pic & getPicture(const String & name) const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

protected:
    Core::List<Ptr<Texture> >    _textures;
    Core::Map<String, Pic>       _pics;
};
}

#endif
