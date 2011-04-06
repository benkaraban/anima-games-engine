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
#include <HOOClient/HOOConfig.h>

static const String CONFIG_FILE(L"/User/hoo.cfg");

HOOConfig::HOOConfig()
{
    reset();
}

void HOOConfig::reset()
{
    login.clear();
    password.clear();
    savePassword = false;
}

void HOOConfig::load(const Ptr<Core::VirtualFileSystem> & pVFS)
{
    if(pVFS->fileExists(CONFIG_FILE))
    {
        Ptr<Core::InputStream> pInput = pVFS->readFile(CONFIG_FILE);
        Ptr<Core::XMLDocument> pXmlGameSettings(new Core::XMLDocument());

        if(pInput != null && pXmlGameSettings->loadDocument(*pInput))        
            importXML(pXmlGameSettings);
    }
    else
    {
        reset();
    }
}

void HOOConfig::save(const Ptr<Core::VirtualFileSystem> & pVFS)
{
    Ptr<Core::OutputStream> pOutput = pVFS->writeFile(CONFIG_FILE);
    Ptr<Core::XMLDocument> pXMLDoc(new Core::XMLDocument());

    if(pOutput != null)
    {
        exportXML(pXMLDoc);
        pXMLDoc->saveDocument(*pOutput);
    }
}

void HOOConfig::importXML(const Ptr<Core::XMLNode> & pXMLNode)
{
    reset();

    pXMLNode->getValue(L"hoo.login", login);
    savePassword = pXMLNode->getValue(L"hoo.sp", password);
}

void HOOConfig::exportXML(const Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"hoo.login", login);

    if(savePassword)
        pXMLNode->setValue(L"hoo.sp", password);
}

