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
#ifndef CHRONORAGE_KAMIKAZE_H_
#define CHRONORAGE_KAMIKAZE_H_

#include <Foes/FoeCommon.h>

namespace ChronoRage
{

const Core::String KAMIKAZE_FILE    = L"/ChronoRageData/Foes/voxels-kamikaze.tga";
const Core::String KAMIKAZE_FILE2   = L"/ChronoRageData/Foes/voxels-kamikaze2.tga";
const Core::String KAMIKAZE_FILE3   = L"/ChronoRageData/Foes/voxels-kamikaze3.tga";

class LM_API_CHR Kamikaze : public FoeCommon
{
public:
    Kamikaze(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode);
    virtual ~Kamikaze();

    virtual void initialize(const Core::Vector3f & position);

protected:
    virtual Ptr<VoxelSpriteTemplate> getTemplate() const;
    static Ptr<VoxelSpriteTemplate> spTemplate;
    static Ptr<VoxelSpriteTemplate> spTemplate2;
    static Ptr<VoxelSpriteTemplate> spTemplate3;

    virtual void updateActive(double elapsed);
    
protected:
    Core::Vector3f  _approachPointSave;
    float           _distance;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_KAMIKAZE_H_*/