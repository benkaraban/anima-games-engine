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
#include "WorldScriptableDocBase.h"


#include <Packing/TexCompressionOnTheFly.h>

#include <EPI/Constants.h>
#include <EPI/Document/LuaOutput.moc.h>
#include <EPI/DocumentRenderer.h>
#include <EPI/Document/Tools.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class WorldScriptableForDocBase::implementation
{
public:
    implementation() :
        isReady(false),
        isRunning(false),
        pWorldDoc(null),
        pNodeCamScript(null),
        pDocRenderer(null),
        pLuaOut(Ptr<LuaOutput> (new LuaOutput())) {}

     ~implementation() {}

public:
    Ptr<LuaOutput>                  pLuaOut;
    bool                            isReady;
    bool                            isRunning;
    Ptr<Universe::World>            pWorldDoc;
    Ptr<Universe::World>            pWorldRunning;
    Ptr<DocumentRenderer>           pDocRenderer;
    Ptr<Universe::NodeCamera>       pNodeCamScript;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
WorldScriptableForDocBase::WorldScriptableForDocBase(const Ptr<DocumentRenderer>& pDocRenderer, const Ptr<Universe::World>& pWorldDoc) :
    _impl (new implementation())
{
    _impl->pWorldDoc    = pWorldDoc;
    _impl->pDocRenderer = pDocRenderer;

    if (_impl->pWorldDoc != null)
    {
        _impl->isReady = true;
    }
}
//-----------------------------------------------------------------------------
WorldScriptableForDocBase::~WorldScriptableForDocBase()
{
    delete _impl;
}
//-----------------------------------------------------------------------------
bool WorldScriptableForDocBase::isReadyForAnimation() const
{
    return _impl->isReady;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& WorldScriptableForDocBase::getWorldForAnimation() const
{
    if (_impl->isRunning)
    {
        return _impl->pWorldRunning;
    }

    return _impl->pWorldDoc;
}
//-----------------------------------------------------------------------------
const Ptr<LuaOutput>& WorldScriptableForDocBase::getLuaOutput() const
{
    return _impl->pLuaOut;
}
//-----------------------------------------------------------------------------
void WorldScriptableForDocBase::startWorldScripts()
{
    LM_ASSERT(isRunning() == false);

    getLuaOutput()->startNewExecutionMemo(L"worldScripts");
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
        _impl->pWorldDoc->exportWorldXML(doc);
        _impl->pWorldRunning->importWorldXML(doc);
    }

    prepareWorldForRunning(*(_impl->pWorldRunning));


    _impl->pWorldRunning->setScriptOutput(getLuaOutput());
    getLuaOutput()->output(L"\n--- Start script ---");


    _impl->pNodeCamScript = _impl->pWorldRunning->createPerspectiveCamera(f_PI_DIV_4, 1.0f, 0.01f, 1000.0f);
    _impl->pNodeCamScript->addTag(TAG_CAM_SCRIPT);
    _impl->isRunning = true;

    emit scriptStarted();
    _impl->pWorldRunning->restart();
    _impl->pWorldRunning->startWorldScripts();
}
//-----------------------------------------------------------------------------
bool WorldScriptableForDocBase::isRunning() const
{
    return _impl->isRunning;
}
//-----------------------------------------------------------------------------
void WorldScriptableForDocBase::stopScript ()
{
    LM_ASSERT(isRunning() == true);

    _impl->pWorldRunning->stopWorldScripts();

    _impl->isRunning = false;


    getLuaOutput()->output(L"--- Stop script ---\n");
    emit scriptStopped();

    _impl->pNodeCamScript->kill();
    _impl->pNodeCamScript = null;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI
