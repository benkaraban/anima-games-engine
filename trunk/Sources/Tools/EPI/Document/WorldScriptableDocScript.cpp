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
#include "WorldScriptableDocScript.h"

#include <QMessageBox>

#include <Universe/World.h>
#include <EPI/Document/Property.moc.h>
#include <EPI/Document/DocumentWorld.moc.h>
#include <EPI/Document/DocumentBase.moc.h>
#include <EPI/Document/LuaOutput.moc.h>
#include <EPI/DocumentRenderer.h>

#include <Packing/TexCompressionOnTheFly.h>
#include <Renderer/IRenderer.h>
#include <Core/VFS/VirtualFileSystem.h>
#include <EPI/Document/DocumentFactory.h>
#include <EPI/ImportInfo/ImportWorldInfo.h>

#include <EPI/Document/Tools.h>

namespace EPI
{
static Ptr<Property> _pBidon;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class WorldScriptableForDocScript::implementation 
{
public:

    implementation(const Ptr<DocumentRenderer>& pDocR) : 
        pDocWorld(null),
        isReady(false), isRunning(false), pLuaOut(Ptr<LuaOutput> (new LuaOutput())),
        pDocRenderer(pDocR)
      {}

    Ptr<Universe::World>            pWorldDefault;
    Ptr<Universe::World>            pWorldRunning;

    String                          worldXMLDataPath;
    Ptr<DocumentBase>               pDocWorld;
    Ptr<DocumentBase>               pDocWorldXML;

    Ptr<Universe::NodeCamera>      pNodeCamScript;

    bool                            isReady;
    bool                            isRunning;
    Ptr<LuaOutput>                  pLuaOut;
    Ptr<DocumentRenderer>           pDocRenderer;    
    Ptr<Universe::ITask>            pTaskScript;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
WorldScriptableForDocScript::WorldScriptableForDocScript(const Ptr<DocumentRenderer>& pDocRenderer) :
    _impl(new implementation(pDocRenderer))
{
    Ptr<Universe::IOnTheFlyTextureGenerator> pTexGen(new Packing::OnTheFlyTextureGenerator());
    _impl->pWorldDefault = Ptr<Universe::World>( new Universe::World(pDocRenderer->getRenderer(), pDocRenderer->getVFS(), null, pDocRenderer->getAudioDevice()));
    _impl->pWorldDefault->getRessourcesPool()->setOnTheFlyTextureGenerator(pTexGen);
    _impl->pWorldDefault->setFogColor(Core::Vector4f(0.2f, 0.2f, 0.25f, 0.f));
    _impl->pWorldDefault->getNodeListener()->setGain(0.f);
    _impl->pWorldDefault->setScriptOutput(getLuaOutput());

  //  connect(_impl->pLuaOut.get(), SIGNAL(newErrorMsg(const String & )), this, SLOT(stopScript()));
}
//-----------------------------------------------------------------------------
WorldScriptableForDocScript::~WorldScriptableForDocScript()
{
    delete _impl;
}
//-----------------------------------------------------------------------------
void WorldScriptableForDocScript::setWorldXMLDataPath  (const String& path, GuiDocument& guiDoc)
{
    if(_impl->worldXMLDataPath != path)
    {
        _impl->worldXMLDataPath = path;
        loadWolrdXmlData(guiDoc);
    }
}
//-----------------------------------------------------------------------------
void WorldScriptableForDocScript::loadWolrdXmlData(GuiDocument& guiDoc)
{
    if (_impl->worldXMLDataPath != L"")
    {
         Ptr<ImportWorldInfo> pImportWorldInfo (new ImportWorldInfo());
            pImportWorldInfo->worldFileName = _impl->worldXMLDataPath;
        _impl->pDocWorldXML = DocumentFactory::factoryLoadDocument(guiDoc, pImportWorldInfo, _impl->pDocRenderer);
        _impl->pDocWorldXML->getWorld()->setScriptOutput(getLuaOutput());
        _impl->isReady = true;

        /*
        _impl->pDocWorldXML = Ptr<DocumentWorld> (new DocumentWorld(_impl->pDocRenderer, _impl->worldXMLDataPath));
        _impl->isReady = true;*/
        /*
        _impl->pWorldXml = Ptr<Universe::World>( new Universe::World(_impl->pRenderer,  _impl->pVFS));
            Ptr<Universe::IOnTheFlyTextureGenerator> pTexGen(new Packing::OnTheFlyTextureGenerator());
            _impl->pWorldXml->getRessourcesPool()->setOnTheFlyTextureGenerator(pTexGen);

        try
        {
            Ptr<Core::InputStream> pInput = _impl->pVFS->readFile(_impl->worldXMLDataPath);
            Core::XMLDocument xmlDocument;
            xmlDocument.loadDocument(*pInput);
            _impl->pWorldXml->importWorldXML(xmlDocument);
            _impl->pWorldXml->setScriptOutput(getLuaOutput());
            _impl->isReady = true;
        }
        catch(Core::Exception & e)
        {
            String message;
            message << L"An exception has been caught while extracting data from the XML file :\n" << e.getMessage();
            QMessageBox::critical( NULL, "Nyx", Core::String8(message).c_str());
        }*/
    }
    else
    {
        _impl->pDocWorldXML = null;
        _impl->isReady = false;
    }
}
//-----------------------------------------------------------------------------
/*void WorldScriptableForDocScript::setWorldDocumentData (const Ptr<DocumentWorld> pDocWorld)
{
    _impl->pDocWorld = pDocWorld;
}*/
//-----------------------------------------------------------------------------
const Ptr<Universe::World>&  WorldScriptableForDocScript::getWorldForAnimation() const
{
    if (_impl->isRunning)
    {
        return _impl->pWorldRunning;
    }

    if (isReadyForAnimation() == true)
    {
        return _impl->pDocWorldXML->getWorld();
    }

    return _impl->pWorldDefault;
}
//-----------------------------------------------------------------------------
bool WorldScriptableForDocScript::isReadyForAnimation() const
{
    return _impl->isReady;
}
//-----------------------------------------------------------------------------
const Ptr<LuaOutput>& WorldScriptableForDocScript::getLuaOutput() const
{
    return _impl->pLuaOut;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void WorldScriptableForDocScript::runScript(const Core::String8 script, const String& name, const String& tagGamePlay)
{
    LM_ASSERT(_impl->pTaskScript==null);

    getLuaOutput()->startNewExecutionMemo(name);
    Core::XMLDocument doc;

    if (_impl->pWorldRunning == null)
    {
        _impl->pWorldRunning = Ptr<Universe::World>( new Universe::World(_impl->pDocRenderer->getRenderer(), _impl->pDocRenderer->getVFS(), null, _impl->pDocRenderer->getAudioDevice()));
        Ptr<Universe::IOnTheFlyTextureGenerator> pTexGen(new Packing::OnTheFlyTextureGenerator());
        _impl->pWorldRunning->getRessourcesPool()->setOnTheFlyTextureGenerator(pTexGen);
        _impl->pWorldRunning->setFogColor(Core::Vector4f(0.2f, 0.2f, 0.25f, 0.f));
        _impl->pWorldRunning->getNodeListener()->setGain(0.f);
    }

    if (isReadyForAnimation() == true)
    {
        _impl->pDocWorldXML->getWorld()->exportWorldXML(doc);
        _impl->pWorldRunning->importWorldXML(doc);
    }
    else
    {
        _impl->pWorldDefault->exportWorldXML(doc);
        _impl->pWorldRunning->importWorldXML(doc);
    }

    Ptr<Universe::NodeListener> pNodeList = _impl->pWorldRunning->getNodeListener();
    if (pNodeList!=null) {pNodeList->setGain(MiscConst::sSoundGlobalGain);}

    prepareWorldForRunning(*(_impl->pWorldRunning), tagGamePlay);

    Universe::ScriptContext context;
    context.fileName = name;
    _impl->pWorldRunning->setScriptOutput(getLuaOutput());
    getLuaOutput()->output(L"\n--- Start script ---");


    _impl->pNodeCamScript = _impl->pWorldRunning->createPerspectiveCamera(f_PI_DIV_4, 1.0f, 0.01f, 1000.0f);
    _impl->pNodeCamScript->addTag(TAG_CAM_SCRIPT);
    _impl->isRunning = true;

    emit scriptStarted();
    _impl->pTaskScript = _impl->pWorldRunning->runInline(script, context);
    

    if (_impl->isRunning == false)
    {
        _impl->pTaskScript->kill();
        _impl->pTaskScript = null;
    }
    
}
//-----------------------------------------------------------------------------
void WorldScriptableForDocScript::stopScript()
{
    LM_ASSERT(isRunning() == true);

    if (_impl->pTaskScript != null)
    {
        _impl->pTaskScript->kill();
    }

    _impl->isRunning = false;


    getLuaOutput()->output(L"--- Stop script ---\n");
    emit scriptStopped();
    _impl->pTaskScript = null;
    _impl->pNodeCamScript->kill();
    _impl->pNodeCamScript = null;
}
//-----------------------------------------------------------------------------
bool WorldScriptableForDocScript::isRunning() const
{
    return _impl->isRunning;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase> WorldScriptableForDocScript::getDocumentForAnimation() const
{
    return _impl->pDocWorldXML;
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera>   WorldScriptableForDocScript::getCameraScript() const
{
    return _impl->pNodeCamScript;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
};
