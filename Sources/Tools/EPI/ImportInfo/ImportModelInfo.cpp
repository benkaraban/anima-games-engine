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
#include "ImportModelInfo.h"

namespace EPI
{

void ImportModelInfo::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setAttribute(L"asset", L"type", EPI::toString(MODEL_ASSET));

    pXMLNode->setValue(L"asset.source-name", sourceName);
    pXMLNode->setValue(L"asset.model-name", modelName);
    pXMLNode->setValue(L"asset.options.max-bones-per-vertex", modelPackingOptions.maxBonesPerVertex);
    pXMLNode->setValue(L"asset.options.max-bones-per-blending-group", modelPackingOptions.maxBonesPerBlendingGroup);
    pXMLNode->setValue(L"asset.options.generate-normals", modelPackingOptions.generateNormals);
    pXMLNode->setValue(L"asset.options.generate-tangent-basis", modelPackingOptions.generateTangentBasis);
    pXMLNode->setValue(L"asset.options.optimize-for-cache", modelPackingOptions.optimizeForCache);
}

void ImportModelInfo::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    pModel = Ptr<Assets::Model>(new Assets::Model());

    success &= pXMLNode->getValue(L"asset.source-name", sourceName);
    success &= pXMLNode->getValue(L"asset.model-name", modelName);
    success &= pXMLNode->getValue(L"asset.options.max-bones-per-vertex", modelPackingOptions.maxBonesPerVertex);
    success &= pXMLNode->getValue(L"asset.options.max-bones-per-blending-group", modelPackingOptions.maxBonesPerBlendingGroup);
    success &= pXMLNode->getValue(L"asset.options.generate-normals", modelPackingOptions.generateNormals);
    success &= pXMLNode->getValue(L"asset.options.generate-tangent-basis", modelPackingOptions.generateTangentBasis);
    success &= pXMLNode->getValue(L"asset.options.optimize-for-cache", modelPackingOptions.optimizeForCache);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML ImportModelInfo values.");
}

}//namespace EPI