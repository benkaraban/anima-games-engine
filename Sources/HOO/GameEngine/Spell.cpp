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
#include <GameEngine/Spell.h>
#include <GameEngine/Enums.h>
#include <Core/IO/Streams.h>
#include <Core/IO/FileStream.h>
#include <Core/System/Files.h>
#include <Core/XML/XMLDocument.h>

namespace GameEngine
{
const String Spell::_sPREFIX(L"SPELL_");


//----------------------------------------------------------------------------
Spell::Spell(const Spell & s) :
    GameEntity(s),
    _pEffectsOnCast(s._pEffectsOnCast),
    _pEffectsOnHit(s._pEffectsOnHit),
    _sphere(s._sphere),
    _type(s._type)
{
    _sphereControl[SPHERE_ENERGY] = s._sphereControl[SPHERE_ENERGY];
    _sphereControl[SPHERE_SPIRIT] = s._sphereControl[SPHERE_SPIRIT];
    _sphereControl[SPHERE_LIFE] = s._sphereControl[SPHERE_LIFE];
}
//----------------------------------------------------------------------------
Spell::Spell(int32 id,
        ESphere sphere,
        ESpellType type,
        const SpellEffect & effectOnHit1) :
    _pEffectsOnCast(Ptr<Core::List<SpellEffect> > (new Core::List<SpellEffect>())),
    _pEffectsOnHit(Ptr<Core::List<SpellEffect> > (new Core::List<SpellEffect>())),
    _sphere(sphere),
    _type(type)
{
    _id = id;
    _pEffectsOnHit->push_back(effectOnHit1);
}
//----------------------------------------------------------------------------
Spell::Spell(int32 id,
        ESphere sphere,
        ESpellType type,
        const SpellEffect & effectOnHit1,
        const SpellEffect & effectOnHit2) :
    _pEffectsOnCast(Ptr<Core::List<SpellEffect> > (new Core::List<SpellEffect>())),
    _pEffectsOnHit(Ptr<Core::List<SpellEffect> > (new Core::List<SpellEffect>())),
    _sphere(sphere),
    _type(type)
{
    _id = id;
    _pEffectsOnHit->push_back(effectOnHit1);
    _pEffectsOnHit->push_back(effectOnHit2);
}
//----------------------------------------------------------------------------
Spell::Spell(int32 id,
        ESphere sphere,
        ESpellType type,
        const SpellEffect & effectOnHit1,
        const SpellEffect & effectOnHit2,
        const SpellEffect & effectOnHit3) :
    _pEffectsOnCast(Ptr<Core::List<SpellEffect> > (new Core::List<SpellEffect>())),
    _pEffectsOnHit(Ptr<Core::List<SpellEffect> > (new Core::List<SpellEffect>())),
    _sphere(sphere),
    _type(type)
{
    _id = id;
    _pEffectsOnHit->push_back(effectOnHit1);
    _pEffectsOnHit->push_back(effectOnHit2);
    _pEffectsOnHit->push_back(effectOnHit3);
}
//----------------------------------------------------------------------------
Spell::Spell(int32 id, 
    ESphere sphere, 
    ESpellType type,
    int sphereControl[3],
    const Ptr<Core::List<SpellEffect> > & pEffectsOnCast,
    const Ptr<Core::List<SpellEffect> > & pEffectsOnHit
    ) :
    _pEffectsOnCast(pEffectsOnCast),
    _pEffectsOnHit(pEffectsOnHit),
    _sphere(sphere),
    _type(type)
{
    _id = id;
    _sphereControl[SPHERE_ENERGY] = sphereControl[SPHERE_ENERGY];
    _sphereControl[SPHERE_LIFE] = sphereControl[SPHERE_LIFE];
    _sphereControl[SPHERE_SPIRIT] = sphereControl[SPHERE_SPIRIT];
}
//----------------------------------------------------------------------------
 Spell::Spell(const Ptr<Core::VirtualFileSystem> & vfs, const String & path) :
    _pEffectsOnCast(Ptr<Core::List<SpellEffect> > (new Core::List<SpellEffect>())),
    _pEffectsOnHit(Ptr<Core::List<SpellEffect> > (new Core::List<SpellEffect>()))
{
    this->load(vfs, path);
}
//----------------------------------------------------------------------------
const Ptr<Core::List<SpellEffect> > & Spell::getEffects() const
{
    return _pEffectsOnHit;
}
//----------------------------------------------------------------------------
const Ptr<Core::List<SpellEffect> > & Spell::getEffectsOnCast() const
{
    return _pEffectsOnCast;
}
const Ptr<Core::List<SpellEffect> > & Spell::getEffectsOnHit() const
{
    return _pEffectsOnHit;
}
//----------------------------------------------------------------------------
void Spell::save(const Ptr<Core::VirtualFileSystem> & vfs, const String & path) const
{
    Ptr<Core::OutputStream> os = vfs->writeFile(path);

    Ptr<Core::XMLNode> spellNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"spell"));
    spellNode->setAttribute(L"id", Core::toString(this->_id));

    Ptr<Core::XMLNode> sphereNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"sphere"));
    sphereNode->setValue(Core::toString(this->_sphere));

    Ptr<Core::XMLNode> typeNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"type"));
    typeNode->setValue(Core::toString(this->_type));

    Ptr<Core::XMLNode> costEnergyNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"cost-energy"));
    costEnergyNode->setValue(Core::toString(this->_sphereControl[SPHERE_ENERGY]));

    Ptr<Core::XMLNode> costLifeNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"cost-life"));
    costEnergyNode->setValue(Core::toString(this->_sphereControl[SPHERE_LIFE]));

    Ptr<Core::XMLNode> costSpiritNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"cost-spirit"));
    costEnergyNode->setValue(Core::toString(this->_sphereControl[SPHERE_SPIRIT]));

    /*Ptr<Core::XMLNode> cooldownNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"cooldown"));
    cooldownNode->setValue(Core::toString(this->_cooldown));*/

    Ptr<Core::XMLNode> effectsNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"effects"));
    
    for(int32 i = 0; i < this->_pEffectsOnHit->size(); i++)
    {
        Ptr<Core::XMLNode> effectNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"effect"));
        effectNode->setAttribute(L"id", Core::toString((*_pEffectsOnHit)[i].getEffect()));
        effectNode->setValue(L"magnitude", Core::toString((*_pEffectsOnHit)[i].getMagnitude()));
        effectsNode->addNode(effectNode);
    }

    for(int32 i = 0; i < this->_pEffectsOnCast->size(); i++)
    {
        Ptr<Core::XMLNode> effectNode = Ptr<Core::XMLNode>(new Core::XMLNode(L"effect"));
        effectNode->setAttribute(L"id", Core::toString((*_pEffectsOnCast)[i].getEffect()));
        effectNode->setValue(L"magnitude", Core::toString((*_pEffectsOnCast)[i].getMagnitude()));
        effectsNode->addNode(effectNode);
    }

    spellNode->setValue(L"script", _script);

    spellNode->addNode(sphereNode);
    spellNode->addNode(typeNode);
    /*spellNode->addNode(costNode);
    spellNode->addNode(cooldownNode);*/
    spellNode->addNode(effectsNode);

    Core::XMLDocument doc = Core::XMLDocument();
    doc.addNode(spellNode);

    doc.saveDocument(* os);
}
//----------------------------------------------------------------------------
void Spell::load(const Ptr<Core::VirtualFileSystem> & vfs, const String & path)
{
    Ptr<Core::InputStream> is = vfs->readFile(path);

    Core::XMLDocument doc = Core::XMLDocument();
    doc.loadDocument(*is);

    String stringRead;
    int32 intRead;

    /* Récupération de l'identifiant */
    doc.getAttribute(L"spell", L"id", stringRead);
    this->_id = Core::toInteger(stringRead);

    /* Récupération du script */
    doc.getValue(L"spell.script", stringRead);
    this->_script = stringRead;

    /* Récupération de la sphere */
    doc.getValue(L"spell.sphere", stringRead);
    stringRead.toLower();
    if(stringRead == L"energy")
        this->_sphere = SPHERE_ENERGY;
    else if(stringRead == L"spirit")
        this->_sphere = SPHERE_SPIRIT;
    else if(stringRead == L"life")
        this->_sphere = SPHERE_LIFE;

    /* Récupération du type */
    doc.getValue(L"spell.type", stringRead);
    stringRead.toLower();
    if(stringRead == L"spell")
        this->_type = SPELLTYPE_SPELL;
    else if(stringRead == L"counter")
        this->_type = SPELLTYPE_COUNTER;
    else if(stringRead == L"trick")
        this->_type = SPELLTYPE_TRICK;
    else if(stringRead == L"retribution")
        this->_type = SPELLTYPE_RETRIBUTION;

    /* Récupération des modificateurs de controles des sphères */
    doc.getValue(L"spell.cost-energy", intRead);
    this->_sphereControl[SPHERE_ENERGY] = intRead;
    doc.getValue(L"spell.cost-spirit", intRead);
    this->_sphereControl[SPHERE_SPIRIT] = intRead;
    doc.getValue(L"spell.cost-life", intRead);
    this->_sphereControl[SPHERE_LIFE] = intRead;

    /* Récupération des effets */
    Core::List<Ptr<Core::XMLNode> > effectsNodes;
    doc.getNodes(L"spell.effects", L"effect", effectsNodes);

    _pEffectsOnHit->clear();
    _pEffectsOnCast->clear();
    int32 effectMagnitude;
    ESpellEffectType effectType;
    for(int32 i = 0; i < effectsNodes.size(); i++)
    {
        bool b;
        b = effectsNodes[i]->getValue(L"type", stringRead);
        stringRead.toUpper();
        /* on remplace les espaces par des underscores pour correspondre aux enumérés */
        for(Core::String::iterator ii = stringRead.begin(); ii != stringRead.end(); ii++)
            if(*ii == ' ')
                *ii = '_';

        effectsNodes[i]->getValue(L"magnitude", effectMagnitude);

        /* Répartition des effets entre effets de lancement et effets de toucher */
        effectType = toSpellEffectType(stringRead);
        if( effectType == REQUIRE_ENERGY ||
            effectType == REQUIRE_LIFE   ||
            effectType == REQUIRE_SPIRIT ||
            effectType == ENERGY_ASSAULT ||
            effectType == LIFE_ASSAULT   ||
            effectType == SPIRIT_ASSAULT)
            _pEffectsOnCast->push_back(SpellEffect(effectType, effectMagnitude));
        else
            _pEffectsOnHit->push_back(SpellEffect(effectType, effectMagnitude));
    }

    LM_ASSERT((this->hasEffect(REQUIRE_ENERGY) && getMagnitudeOfEffect(REQUIRE_ENERGY) > 0) 
           || (!this->hasEffect(REQUIRE_ENERGY) && getMagnitudeOfEffect(REQUIRE_ENERGY) == 0));

    LM_ASSERT((this->hasEffect(REQUIRE_LIFE) && getMagnitudeOfEffect(REQUIRE_LIFE) > 0) 
           || (!this->hasEffect(REQUIRE_LIFE) && getMagnitudeOfEffect(REQUIRE_LIFE) == 0));

    LM_ASSERT((this->hasEffect(REQUIRE_SPIRIT) && getMagnitudeOfEffect(REQUIRE_SPIRIT) > 0) 
           || (!this->hasEffect(REQUIRE_SPIRIT) && getMagnitudeOfEffect(REQUIRE_SPIRIT) == 0));

    LM_ASSERT((this->hasEffect(ENERGY_ASSAULT) && getMagnitudeOfEffect(ENERGY_ASSAULT) > 0) 
           || (!this->hasEffect(ENERGY_ASSAULT) && getMagnitudeOfEffect(ENERGY_ASSAULT) == 0));

    LM_ASSERT((this->hasEffect(LIFE_ASSAULT) && getMagnitudeOfEffect(LIFE_ASSAULT) > 0) 
           || (!this->hasEffect(LIFE_ASSAULT) && getMagnitudeOfEffect(LIFE_ASSAULT) == 0));

    LM_ASSERT((this->hasEffect(SPIRIT_ASSAULT) && getMagnitudeOfEffect(SPIRIT_ASSAULT) > 0) 
           || (!this->hasEffect(SPIRIT_ASSAULT) && getMagnitudeOfEffect(SPIRIT_ASSAULT) == 0));

    doc.getValue(L"spell.script", _script);
}
//----------------------------------------------------------------------------
bool Spell::hasEffect(ESpellEffectType type) const
{
    bool result = false;

    for(int32 i = 0; i < _pEffectsOnHit->size(); i++)
        if((*_pEffectsOnHit)[i].getEffect() == type)
            result = true;

    for(int32 i = 0; i < _pEffectsOnCast->size(); i++)
        if((*_pEffectsOnCast)[i].getEffect() == type)
            result = true;

    return result;
}
//----------------------------------------------------------------------------
const String & Spell::getPrefix() const
{
    return Spell::_sPREFIX;
}
//----------------------------------------------------------------------------
const String & Spell::getSpellsPrefix()
{
    return Spell::_sPREFIX;
}
//----------------------------------------------------------------------------
const SpellEffect & Spell::getEffect(int32 i) const
{
    return (* this->_pEffectsOnHit)[i];
}
//----------------------------------------------------------------------------
int32 Spell::getMagnitudeOfEffect(ESpellEffectType effect) const
{
    int32 result = 0;
    for(int32 ii = 0; ii < _pEffectsOnHit->size(); ii++)
        if((*_pEffectsOnHit)[ii].getEffect() == effect)
            result = (*_pEffectsOnHit)[ii].getMagnitude();
    for(int32 ii = 0; ii < _pEffectsOnCast->size(); ii++)
        if((*_pEffectsOnCast)[ii].getEffect() == effect)
            result = (*_pEffectsOnCast)[ii].getMagnitude();
    return result;
}
}
