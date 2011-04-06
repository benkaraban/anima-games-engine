/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef TERRAIN_MODEL_GEN_H_
#define TERRAIN_MODEL_GEN_H_

#include <Assets/TerrainModel.h>
#include <Images/Image.h>

namespace Workflow
{
//-----------------------------------------------------------------------------
struct LM_API_WKF TMGenInput
{
    Ptr<Images::Image> pHeightMap;       // Image en GRAY_F32
    Ptr<Images::Image> pNormalMap;       // Image en ARGB8_F32, si pNormalMap est à null, une normale map est générée
                                         // à partir de la height map et en sortie pNormalMap pointe sur cette map générée.

    Core::Vector2f     terrainSize;      // Dimensions du terrain
    float              maxHeight;        // Altitude du point le plus haut du terrain par rapport au point le plus bas

    int32              LODCount;         // Le nombre de niveau de detail present sur le terrain
    int32              blockWidth;       // Le nombre de vertex par block est obtenu par widthBlockSize * widthBlockSize

    float              blendScaleFactor; // Facteur de transition entre les niveaux de détail
};

//-----------------------------------------------------------------------------
LM_API_WKF Ptr<Assets::TerrainModel> generateTerrainModel(TMGenInput & gen);

}

#endif