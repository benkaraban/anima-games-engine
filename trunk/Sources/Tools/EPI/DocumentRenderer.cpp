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
#include "DocumentRenderer.h"

#include <QWidget>

#include <Core/XML/XMLDocument.h>
#include <Core/VFS/VFSXMLTranslator.h>

#include <Gfx/GfxDx9/DriverDx9.h>
#include <Renderer/SM2/RendererSM2.h>

#include <EPI/ApplicationBase.moc.h>
#include <Core/VFS/VirtualFileSystem.h>
#include <Renderer/IRenderer.h>

#include <Audio/AudioGlobals.h>
#include <Audio/AudioOpenAL/DriverOpenAL.h>

namespace EPI
{
//-----------------------------------------------------------------------------
class DocumentRenderer::implementation
{
public:
    implementation(const ERendererType type) : _type(type), _id(countRenderer++) {}
    ~implementation() {    _pFontDefault = null; _pRenderer = null;}

    Ptr<Renderer::IFont>                _pFontDefault;
    ERendererType                       _type;
    Ptr<Renderer::IRenderer>            _pRenderer;
    Ptr<Core::VirtualFileSystem>        _pVFS;
    int32                               _id;

    Ptr<Audio::IDriver>                 _audioDriver;
    Ptr<Audio::IDevice>                 _audioDevice;


    //counter de renderer permettant d'affecter l'identifiant
    static int32 countRenderer;
};
//-----------------------------------------------------------------------------
int32 DocumentRenderer::implementation::countRenderer = 0;
//-----------------------------------------------------------------------------
DocumentRenderer::DocumentRenderer(const ERendererType type) :
    _impl(new implementation(type))
{
    QWidget* bidon = new QWidget;
    bidon->resize(800,600);


    Core::logStdOut_Off();

    switch(_impl->_type)
    {
        case ERT_RENDERER_SM2:
            
            // VFS
            String vfs;
            vfs = L"./Data/VFS-Nyx.xml";//TODO 

            Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());
            if(pXmlDocument->loadDocument(vfs))
            {
                _impl->_pVFS = Core::VFSFromXMLNode(pXmlDocument->firstChildNode(L"vfs"));
                INF << L"VirtualFileSystem layout :\n";
                INF << _impl->_pVFS->toString() << "\n";
            }
            else
                throw Core::Exception(L"Unable to mount VFS.");

            Renderer::RendererSettings rs(Renderer::GLOBAL_LOW);
            rs.fullscreen = false;
            rs.width = bidon->width();
            rs.height = bidon->height();

             _impl->_pRenderer = Ptr<Renderer::IRenderer>(  new Renderer::RendererSM2(Gfx::IDriverPtr(new Gfx::DriverDx9()),
                                                    rs,
                                                    bidon->winId(),
                                                    _impl->_pVFS,
                                                    L"/Shaders",
                                                    Renderer::MULTITHREADED_FLAG));

        break;
    }

    bool result = _impl->_pRenderer->initialise();
/*
    Renderer::RendererSettings settings = _pRenderer->getRenderSettings();
    settings.polyMode = Renderer::POLY_SOLID_WIREFRAME;
        _pRenderer->setRenderSettings(settings);*/

    _impl->_pFontDefault = _impl->_pRenderer->createFont(L"Arial", 10);


    Audio::initVorbisFileDll();
        _impl->_audioDriver = Ptr<Audio::IDriver>(new AudioOpenAL::DriverOpenAL());
        _impl->_audioDevice = _impl->_audioDriver->createDevice(0);


    Core::logStdOut_On();
    if(!result)
    {
        throw Core::Exception(L"Unable to initialise Renderer.");
    }
}
//-----------------------------------------------------------------------------
const Ptr<Renderer::IFont>& DocumentRenderer::getDefaultFont()
{
    return _impl->_pFontDefault;
}
//-----------------------------------------------------------------------------
DocumentRenderer::~DocumentRenderer()
{
    delete _impl;
}
//-----------------------------------------------------------------------------
const Ptr<Renderer::IRenderer>& DocumentRenderer::getRenderer()
{
    return _impl->_pRenderer;
}
//-----------------------------------------------------------------------------
const Ptr<Core::VirtualFileSystem>& DocumentRenderer::getVFS() const
{
    return _impl->_pVFS;
}
//-----------------------------------------------------------------------------
const Ptr<Audio::IDevice>& DocumentRenderer::getAudioDevice() const
{
    return _impl->_audioDevice;
}
//-----------------------------------------------------------------------------
Ptr<Core::VirtualFileSystem>& DocumentRenderer::getVFS()
{
    return _impl->_pVFS;
}
//-----------------------------------------------------------------------------
int32 DocumentRenderer::getID()
{
    return _impl->_id;
}
//-----------------------------------------------------------------------------
} //namespace EPI
