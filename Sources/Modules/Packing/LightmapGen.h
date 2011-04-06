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
#ifndef PACKING_LIGHTMAP_GEN_H_
#define PACKING_LIGHTMAP_GEN_H_

#include <Universe/World.h>

namespace Packing
{
//-----------------------------------------------------------------------------
struct LM_API_PAK LightmapsGenOptions
{
    Ptr<Universe::World>     pWorld;

    /**
     * Chemin du r�pertoire dans lequel seront stock�es les textures g�n�r�es.
     */
    String                       path;

    /**
     * Pr�fixe ins�r� avant le nom de chacun des fichiers g�n�r�s qui ont donc la forme
     * /path/prefix*
     */
    String                       prefix;

    /**
     * Si compressTextures = true, les textures diffuse sont compress�es en DXTC1.
     * sinon elles sont en ARGB8.
     */
    bool                         compressTextures;

    bool                         debugCopy;

    String                       tag;

    Ptr<Core::IProgressObs>      pObserver;
    int32                        defaultResolution;
};

LM_API_PAK void generateLightmaps(const LightmapsGenOptions & options);

}

#endif
