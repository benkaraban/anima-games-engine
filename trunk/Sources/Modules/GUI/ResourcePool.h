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
#ifndef GUI_GUIRESPOOL_H_
#define GUI_GUIRESPOOL_H_

#include <Core/VFS/VirtualFileSystem.h>

#include <Renderer/IRenderer.h>
#include <Renderer/IHUD.h>

#include <GUI/BitmapFont.h>

namespace GUI
{
class LM_API_GUI IOnTheFlyPictureLoader
{
public:
    virtual ~IOnTheFlyPictureLoader() {};
    virtual Renderer::PictureInfos getPicture(const String & name) = 0;
};

class LM_API_GUI ResourcePool : public Core::Object
{
public:
    ResourcePool(const Ptr<Core::VirtualFileSystem> & pVFS,
                 const Ptr<Renderer::IRenderer> & pRenderer,
                 const Ptr<Renderer::ITextureGrabber> & pTexGrabber);

    Ptr<Core::VirtualFileSystem> getVFS() { return _pVFS; };

    virtual Ptr<Renderer::IFont>        getFont(const String & name, int32 size);
    virtual Ptr<BitmapFont>             getBitmapFont(const String & name);
    virtual Renderer::PictureInfos      getPicture(const String & name);

    virtual void mountPictureBank(const String & mountPoint, const String & picBankFile);

    virtual void setOnTheFlyPictureLoader(const Ptr<IOnTheFlyPictureLoader> & pOnTheFlyLoader);

protected:
    String getCleanMountPoint(const String & mountPoint);

    Ptr<Core::VirtualFileSystem>    _pVFS;
    Ptr<Renderer::IRenderer>        _pRenderer;
    Ptr<Renderer::ITextureGrabber>  _pTexGrabber;
    Ptr<IOnTheFlyPictureLoader>     _pOnTheFlyLoader;

    Core::Map<String, Ptr<Renderer::IFont> >        _fonts;
    Core::Map<String, Ptr<BitmapFont> >             _bitmapFonts;
    Core::Map<String, Ptr<Renderer::IPictureBank> > _picBanks;
    Core::Map<String, Renderer::PictureInfos >      _pics;
    
};
}

#endif

