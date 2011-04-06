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
#include <Assets/PictureBank.h>
#include <Workflow/ZonePacking.h>

namespace Assets
{
//-----------------------------------------------------------------------------
PictureBank::PictureBank()
{
}
//-----------------------------------------------------------------------------
PictureBank::PictureBank(const Core::List<Ptr<Texture> > & textures, const Core::Map<String, Pic> & pics) :
    _textures(textures),
    _pics(pics)
{
}
//-----------------------------------------------------------------------------
const PictureBank::Pic & PictureBank::getPicture(const String & name) const
{
    Core::Map<String, Pic>::const_iterator iPic = _pics.find(name);

    if(iPic == _pics.end())
        throw Core::Exception(L"Picture '" + name + L"' not found in bank");

    return iPic->second;
}
//-----------------------------------------------------------------------------
void PictureBank::read(Core::InputStream & input)
{
    int32 texCount = input.readInt32();
    _textures.resize(texCount);

    for(int32 ii=0; ii < texCount; ii++)
    {
        _textures[ii] = Ptr<Texture>(new Texture());
        _textures[ii]->read(input);
    }

    _pics.clear();
    int32 picCount = input.readInt32();

    for(int32 ii=0; ii < picCount; ii++)
    {
        Pic pic;
        String name;

        input.read(name);
        input.read(pic.textureIndex);
        input.read(pic.width);
        input.read(pic.height);
        input.read(pic.u1);
        input.read(pic.v1);
        input.read(pic.u2);
        input.read(pic.v2);

        _pics[name] = pic;
    }
}
//-----------------------------------------------------------------------------
void PictureBank::write(Core::OutputStream & output) const
{
    output.write(_textures.size());

    for(int32 ii=0; ii < _textures.size(); ii++)
        _textures[ii]->write(output);

    output.write(_pics.size());

    for(Core::Map<String, Pic>::const_iterator iPic = _pics.begin(); iPic != _pics.end(); ++iPic)
    {
        const Pic & pic = iPic->second;
        output.write(iPic->first); // name
        output.write(pic.textureIndex);
        output.write(pic.width);
        output.write(pic.height);
        output.write(pic.u1);
        output.write(pic.v1);
        output.write(pic.u2);
        output.write(pic.v2);
    }
}
//-----------------------------------------------------------------------------
}
