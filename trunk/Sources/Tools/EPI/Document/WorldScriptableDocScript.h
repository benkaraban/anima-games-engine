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
#ifndef WORLD_SCRIPTABLE_FOR_DOC_SCRIPT_H_
#define WORLD_SCRIPTABLE_FOR_DOC_SCRIPT_H_



#include <EPI/Document/IWorldScriptable.moc.h>


namespace Universe
{
    class World;
}

namespace EPI
{
class Property;
class LuaOutput;
class DocumentBase;
class DocumentRenderer;
class GuiDocument;
//-----------------------------------------------------------------------------
class LM_API_EPI WorldScriptableForDocScript : public IWorldScriptable
{
public:
    WorldScriptableForDocScript(const Ptr<DocumentRenderer>& pDocRenderer);
    virtual ~WorldScriptableForDocScript();

    void setWorldXMLDataPath  (const String& path, GuiDocument& guiDoc);
    //void setWorldDocumentData (const Ptr<DocumentWorld> pDocWorld);

    bool isReadyForAnimation() const;

    const Ptr<Universe::World>&     getWorldForAnimation() const;
    const Ptr<LuaOutput>&           getLuaOutput() const;

    
    void                            runScript  (const Core::String8   script, const String& name, const String& tagGamePlay = L"");

    void    startWorldScripts() {}

    Ptr<DocumentBase>  getDocumentForAnimation() const;

    void stopScript ();
    bool isRunning() const;

    Ptr<Universe::NodeCamera>   getCameraScript() const;
protected:
    void loadWolrdXmlData(GuiDocument& guiDoc);

private:
    class implementation;
	implementation* const _impl;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif WORLD_SCRIPTABLE_FOR_DOC_SCRIPT_H_
