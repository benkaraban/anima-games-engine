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
#ifndef TERRAIN_TEXTURE_GEN_H_
#define TERRAIN_TEXTURE_GEN_H_

#include <Core/IProgressObs.h>
#include <Universe/World.h>

namespace Packing
{
//-----------------------------------------------------------------------------
struct LM_API_PAK TerrainTextureGenOptions
{
    TerrainTextureGenOptions() :
        compressTextures(false)
    {};

    Ptr<Universe::NodeTerrain>  pTerrain;

    Ptr<Renderer::IRenderer>    pRenderer;

    /**
     * RessourcePool utilis�e pour charger les textures (peut �tre celle du World).
     */
    Ptr<Universe::RessourcePool> pPool;

    /**
     * VFS dans lequel seront sauvegard�es les textures g�n�r�es.
     */
    Ptr<Core::VirtualFileSystem> pVFS;

    /**
     * Chemin du r�pertoire dans lequel seront stock�es les textures g�n�r�es.
     */
    String                       path;

    /**
     * Pr�fixe ins�r� avant le nom de chacun des fichiers g�n�r�s qui ont donc la forme.
     * /path/prefix*
     */
    String                       prefix;

    /** 
     * R�solution des textures pour un LOD donn�.
     * resolution.size() doit �tre >= 1
     */
    Core::List<int32>            resolution;

    /**
     * Si compressTextures = true, les textures diffuse sont compress�es
     * en DXTC1 et les normales en DXTC5n, sinon elles sont toutes les deux en
     * ARGB8.
     */
    bool                         compressTextures;

    Ptr<Core::IProgressObs>      pObserver;
};

LM_API_PAK void generateTerrainTexture(const TerrainTextureGenOptions & options);

}

#endif