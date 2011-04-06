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
#include "ImportTerrainInfo.h"

namespace EPI
{

void ImportTerrainInfo::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setAttribute(L"asset", L"type", EPI::toString(TERRAIN_ASSET));

    pXMLNode->setValue(L"asset.height-map-name", heightMapName);
    pXMLNode->setValue(L"asset.normal-map-name", normalMapName);
    pXMLNode->setValue(L"asset.terrain-name", terrainModelName);
    pXMLNode->setValue(L"asset.options.level-of-detail", pTerrainOptions->LODCount);
    pXMLNode->setValue(L"asset.options.block-width", pTerrainOptions->blockWidth);
    pXMLNode->setValue(L"asset.options.blend-scale-factor", pTerrainOptions->blendScaleFactor);
    pXMLNode->setValue(L"asset.options.max-height", pTerrainOptions->maxHeight);
    pXMLNode->setValue(L"asset.options.terrain-size.width", pTerrainOptions->terrainSize.x);
    pXMLNode->setValue(L"asset.options.terrain-size.height", pTerrainOptions->terrainSize.y);
}

void ImportTerrainInfo::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    pTerrainModel = Ptr<Assets::TerrainModel>(new Assets::TerrainModel());
    pTerrainOptions = Ptr<Workflow::TMGenInput>(new Workflow::TMGenInput());

    success &= pXMLNode->getValue(L"asset.height-map-name", heightMapName);
    success &= pXMLNode->getValue(L"asset.normal-map-name", normalMapName);
    success &= pXMLNode->getValue(L"asset.terrain-name", terrainModelName);
    success &= pXMLNode->getValue(L"asset.options.level-of-detail", pTerrainOptions->LODCount);
    success &= pXMLNode->getValue(L"asset.options.block-width", pTerrainOptions->blockWidth);
    success &= pXMLNode->getValue(L"asset.options.blend-scale-factor", pTerrainOptions->blendScaleFactor);
    success &= pXMLNode->getValue(L"asset.options.max-height", pTerrainOptions->maxHeight);
    success &= pXMLNode->getValue(L"asset.options.terrain-size.width", pTerrainOptions->terrainSize.x);
    success &= pXMLNode->setValue(L"asset.options.terrain-size.height", pTerrainOptions->terrainSize.y);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML ImportModelInfo values.");
}

}//namespace EPI