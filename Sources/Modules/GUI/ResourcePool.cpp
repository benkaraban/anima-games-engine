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
#include <Core/XML/XMLDocument.h>
#include <Core/Strings/Tools.h>
#include <Core/Logger.h>

#include <GUI/ResourcePool.h>

namespace GUI
{
//-----------------------------------------------------------------------------
ResourcePool::ResourcePool(const Ptr<Core::VirtualFileSystem> & pVFS,
                           const Ptr<Renderer::IRenderer> & pRenderer,
                           const Ptr<Renderer::ITextureGrabber> & pTexGrabber) :
    _pVFS(pVFS),
    _pRenderer(pRenderer),
    _pTexGrabber(pTexGrabber)
{
}
//-----------------------------------------------------------------------------
Ptr<Renderer::IFont> ResourcePool::getFont(const String & name, int32 size)
{
    Ptr<Renderer::IFont> pFont;
    String fullName(name + Core::toString(size));
    Core::Map<String, Ptr<Renderer::IFont> >::iterator iFont = _fonts.find(fullName);

    if(iFont != _fonts.end())
    {
        pFont = iFont->second;
    }
    else
    {
        pFont = _pRenderer->createFont(name, size);
        _fonts[fullName] = pFont;
    }

    return pFont;
}
//-----------------------------------------------------------------------------
Renderer::PictureInfos ResourcePool::getPicture(const String & name)
{
    Renderer::PictureInfos result;
    Core::Map<String, Renderer::PictureInfos>::iterator iPic = _pics.find(name);

    if(iPic != _pics.end())
    {
        result = iPic->second;
    }
    else
    {
        String dir = Core::System::getPath(name);
        String mp(getCleanMountPoint(dir));

        Core::Map<String, Ptr<Renderer::IPictureBank> >::iterator iBank = _picBanks.find(mp);

        if(iBank != _picBanks.end())
        {
            result = iBank->second->getPictureInfos(Core::System::getFileName(name));
            _pics[name] = result;
        }
        else if(_pOnTheFlyLoader != null)
        {
            return _pOnTheFlyLoader->getPicture(name);
        }
        else
        {
            throw Core::Exception(L"No picture bank found");
        }        
    }

    return result;
}
//-----------------------------------------------------------------------------
void ResourcePool::mountPictureBank(const String & mountPoint, const String & picBankFile)
{
    try
    {
        Ptr<Core::InputStream> pInput(_pVFS->readFile(picBankFile));

        if(pInput == null)
            throw Core::Exception(L"picture bank file not found");

        Ptr<Assets::PictureBank> pSrcPicBank(new Assets::PictureBank());
        pSrcPicBank->read(*pInput);

        Ptr<Renderer::IPictureBank> pPicBank = _pRenderer->uploadPictureBank(pSrcPicBank);

        String mp(getCleanMountPoint(mountPoint));
        _picBanks[mp] = pPicBank;
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error mounting picture bank '" << picBankFile << L"' to '" << mountPoint << L"' : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
    }
}
//-----------------------------------------------------------------------------
Ptr<BitmapFont> ResourcePool::getBitmapFont(const String & name)
{
    Ptr<BitmapFont> pFont;

    try
    {
        Core::Map<String, Ptr<BitmapFont> >::iterator iFont = _bitmapFonts.find(name);

        if(iFont != _bitmapFonts.end())
        {
            pFont = iFont->second;
        }
        else
        {
            Ptr<Core::InputStream> pInput(_pVFS->readFile(name));

            if(pInput == null)
                throw Core::Exception(L"bitmap font file not found");

            Core::XMLDocument doc;
            doc.loadDocument(*pInput);

            Ptr<Core::XMLNode> pXMLNode(doc.firstChildNode(L"font"));

            if(pXMLNode == null)
                throw Core::Exception(L"invalid bitmap font file");

            BitmapFontInfos infos;
            infos.importXML(pXMLNode);

            Ptr<Renderer::ITextureMap> pTexture = _pTexGrabber->getTexture(infos.textureName);
            pFont = Ptr<BitmapFont>(new BitmapFont(pTexture, infos));
        }
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error loading bitmap font : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
    }

    return pFont;
}
//-----------------------------------------------------------------------------
String ResourcePool::getCleanMountPoint(const String & mountPoint)
{
    String noFinalSlash;
    Core::trimRight(mountPoint, noFinalSlash, L"/");

    return noFinalSlash;
}
//-----------------------------------------------------------------------------
void ResourcePool::setOnTheFlyPictureLoader(const Ptr<IOnTheFlyPictureLoader> & pOnTheFlyLoader)
{
    _pOnTheFlyLoader = pOnTheFlyLoader;
}
//-----------------------------------------------------------------------------
}

