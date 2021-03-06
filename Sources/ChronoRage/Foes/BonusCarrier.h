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
#ifndef CHRONORAGE_BONUS_CARRIER_H_
#define CHRONORAGE_BONUS_CARRIER_H_

#include <Foes/FoeCommon.h>
#include <Foes/Buckler.h>

namespace ChronoRage
{

const Core::String BONUS_CARRIER_FILE = L"/ChronoRageData/Foes/voxels-bonus_carrier.tga";

class Buckler;

class LM_API_CHR BonusCarrier : public FoeCommon
{
public:
    BonusCarrier(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode, bool withShield = false);
    virtual ~BonusCarrier();

    virtual void initialize(const Core::Vector3f & position);
    virtual bool damageOnPlayerDeath() const { return false; }

protected:
    virtual Ptr<VoxelSpriteTemplate> getTemplate() const;
    static Ptr<VoxelSpriteTemplate> spTemplate;

    virtual void update(double elapsed);

    virtual void updateActive(double elapsed);
    virtual void updateDestruction(double elapsed);

protected:
    Ptr<Buckler>    _pBuckler;
    bool            _createBonus;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_BONUS_CARRIER_H_*/