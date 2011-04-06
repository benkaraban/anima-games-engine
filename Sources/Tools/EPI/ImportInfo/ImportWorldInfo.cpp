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
#include "ImportWorldInfo.h"

namespace EPI
{

void ImportWorldInfo::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setAttribute(L"asset", L"type", EPI::toString(WORLD_ASSET));
    pXMLNode->setValue(L"asset.world-file-name", worldFileName);

    if(lightmaps.sourceWorld != String::EMPTY)
    {
        Core::Ptr<Core::XMLNode> pNodeLM(new Core::XMLNode(L"lightmaps"));
        lightmaps.exportXML(pNodeLM);

        pXMLNode->addNode(pNodeLM);
    }

    if(occlusionmaps.sourceWorld != String::EMPTY)
    {
        Core::Ptr<Core::XMLNode> pNodeOM(new Core::XMLNode(L"occlusionmaps"));
        occlusionmaps.exportXML(pNodeOM);

        pXMLNode->addNode(pNodeOM);
    }
}
void ImportWorldInfo::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;
    success &= pXMLNode->getValue(L"asset.world-file-name", worldFileName);

    Ptr<Core::XMLNode> pNodeLM = pXMLNode->firstChildNode(L"lightmaps");
    Ptr<Core::XMLNode> pNodeOM = pXMLNode->firstChildNode(L"occlusionmaps");

    if(pNodeLM != null)
        lightmaps.importXML(pNodeLM);
    else
        lightmaps = LightmapsGenerationInfo();

    if(pNodeOM != null)
        occlusionmaps.importXML(pNodeOM);
    else
        occlusionmaps = OcclusionmapsGenerationInfo();


    if(!success)
        throw Core::Exception(L"An error occured while importing the XML ImportTextureInfo values.");
}

}//namespace EPI