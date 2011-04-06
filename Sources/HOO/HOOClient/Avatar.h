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
#ifndef HOOCLIENT_AVATAR_H_
#define HOOCLIENT_AVATAR_H_

#include <Universe/NodeSkeleton.h>

const int32     CHARACTER_SILENE                    = 1;
const int32     CHARACTER_HERO                      = 2;
const int32     CHARACTER_PRIESTESS                 = 3;

const int32     CHARACTER_COUNT                     = 3;

// ID equipment = 2 C S E
// 2 = Toujours 2 (1 pour identifier les sorts)
// C = identifiant du personnage à équiper
// S = identifiant du set (0 => base, 1 => vénérable, 2 => aguerri, 3 => impétieux)


// Equipment characters

const int32     EC_SILENE                           = CHARACTER_SILENE * 100;
const int32     EC_HERO                             = CHARACTER_HERO * 100;
const int32     EC_PRIESTESS                        = CHARACTER_PRIESTESS * 100;


// Equipment Sets

const int32     ES_BASE                             = 2000;
const int32     ES_VENERABLE                        = 2010;
const int32     ES_BOLD                             = 2020;
const int32     ES_IMPETIOUS                        = 2030;


// Equipment elements

const int32     EE_SYMBOL                           = 1;
const int32     EE_CLOTHES                          = 2;
const int32     EE_HEADDRESS                        = 3;
const int32     EE_ORNAMENT                         = 4;
const int32     EE_AURA                             = 5;


// Presets by characters

const int32     EQUIP_BASE_SYMBOL                   = ES_BASE + EE_SYMBOL;
const int32     EQUIP_BASE_CLOTHES                  = ES_BASE + EE_CLOTHES;
const int32     EQUIP_BASE_HEADDRESS                = ES_BASE + EE_HEADDRESS;
const int32     EQUIP_BASE_ORNAMENT                 = ES_BASE + EE_ORNAMENT;
const int32     EQUIP_BASE_AURA                     = ES_BASE + EE_AURA;


// Presets by characters

const int32     EQUIP_SILENE_BASE_SYMBOL            = EC_SILENE + EQUIP_BASE_SYMBOL;
const int32     EQUIP_SILENE_BASE_CLOTHES           = EC_SILENE + EQUIP_BASE_CLOTHES;
const int32     EQUIP_SILENE_BASE_HEADDRESS         = EC_SILENE + EQUIP_BASE_HEADDRESS;
const int32     EQUIP_SILENE_BASE_ORNAMENT          = EC_SILENE + EQUIP_BASE_ORNAMENT;
const int32     EQUIP_SILENE_BASE_AURA              = EC_SILENE + EQUIP_BASE_AURA;

const int32     EQUIP_HERO_BASE_SYMBOL              = EC_HERO + EQUIP_BASE_SYMBOL;
const int32     EQUIP_HERO_BASE_CLOTHES             = EC_HERO + EQUIP_BASE_CLOTHES;
const int32     EQUIP_HERO_BASE_HEADDRESS           = EC_HERO + EQUIP_BASE_HEADDRESS;
const int32     EQUIP_HERO_BASE_ORNAMENT            = EC_HERO + EQUIP_BASE_ORNAMENT;
const int32     EQUIP_HERO_BASE_AURA                = EC_HERO + EQUIP_BASE_AURA;

const int32     EQUIP_PRIESTESS_BASE_SYMBOL         = EC_PRIESTESS + EQUIP_BASE_SYMBOL;
const int32     EQUIP_PRIESTESS_BASE_CLOTHES        = EC_PRIESTESS + EQUIP_BASE_CLOTHES;
const int32     EQUIP_PRIESTESS_BASE_HEADDRESS      = EC_PRIESTESS + EQUIP_BASE_HEADDRESS;
const int32     EQUIP_PRIESTESS_BASE_ORNAMENT       = EC_PRIESTESS + EQUIP_BASE_ORNAMENT;
const int32     EQUIP_PRIESTESS_BASE_AURA           = EC_PRIESTESS + EQUIP_BASE_AURA;


struct AvatarDesc
{
    int32               characterId;
    Core::List<int32>   equipment;
};

String getCharacterModel(int32 charId);
String getSetString(int32 setId);
String getElementString(int32 elemId);

inline int32 getCharacterFromEquipmentId(int32 id) { return ((id % 1000) / 100); }
inline int32 getElementFromEquipmentId(int32 id) { return id % 10; }
inline int32 getSetFromEquipmentId(int32 id) { return id - (getCharacterFromEquipmentId(id) * 100) - getElementFromEquipmentId(id); }

Ptr<Universe::NodeSkeleton> createAvatar(Universe::World & world, const AvatarDesc & desc);
Ptr<Universe::NodeSkeleton> createBaseAvatar(Universe::World & world, int32 charId);


#endif
