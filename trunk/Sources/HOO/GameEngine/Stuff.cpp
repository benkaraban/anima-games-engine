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
#include <GameEngine/Stuff.h>
#include <Core/XML/XMLDocument.h>

namespace GameEngine
{
const String Stuff::_sPREFIX(L"STUFF_");

Stuff::Stuff(const Ptr<Core::VirtualFileSystem> & vfs, const String & path) :
    _pEffects(Ptr<Core::List<StuffEffect> > (new Core::List<StuffEffect>()))
{
    this->load(vfs, path);
}

Stuff::Stuff(int32 id, EStuffSlot slot, StuffEffect effect1) :
    _slot(slot),
    _pEffects(Ptr<Core::List<StuffEffect> > (new Core::List<StuffEffect>()))
{
    _id = id;
    _pEffects->push_back(effect1);
}

Stuff::Stuff(int32 id, EStuffSlot slot, StuffEffect effect1, StuffEffect effect2) :
    _slot(slot),
    _pEffects(Ptr<Core::List<StuffEffect> > (new Core::List<StuffEffect>()))
{
    _id = id;
    _pEffects->push_back(effect1);
    _pEffects->push_back(effect2);
}

Stuff::Stuff(int32 id, EStuffSlot slot, StuffEffect effect1, StuffEffect effect2, StuffEffect effect3) :
    _slot(slot),
    _pEffects(Ptr<Core::List<StuffEffect> > (new Core::List<StuffEffect>()))
{
    _id = id;
    _pEffects->push_back(effect1);
    _pEffects->push_back(effect2);
    _pEffects->push_back(effect3);
}

const String & Stuff::getStuffsPrefix()
{
    return _sPREFIX;
}

const String & Stuff::getPrefix() const
{
    return _sPREFIX;
}

EStuffSlot Stuff::getSlot()
{
    return _slot;
}

const StuffEffect & Stuff::getEffect(int32 i)
{
    return (*_pEffects)[i];
}

bool Stuff::hasEffect(EStuffEffectType type) const
{
    bool result = false;

    for(int32 i = 0; i < _pEffects->size(); i++)
        if((*_pEffects)[i].getEffect() == type)
            result = true;

    return result;
}

void Stuff::load(const Ptr<Core::VirtualFileSystem> & vfs, const String & path)
{
    Ptr<Core::InputStream> is = vfs->readFile(path);

    Core::XMLDocument doc = Core::XMLDocument();
    doc.loadDocument(*is);

    String stringRead;

    doc.getAttribute(L"stuff", L"id", stringRead);
    this->_id = Core::toInteger(stringRead);

    doc.getValue(L"stuff.slot", stringRead);
    stringRead.toUpper();
    this->_slot = GameEngine::toStuffSlot(stringRead);

    Core::List<Ptr<Core::XMLNode> > effectsNodes = Core::List<Ptr<Core::XMLNode> > ();
    doc.getNodes(L"stuff.effects", L"effect", effectsNodes);

    _pEffects->clear();
    int32 effectMagnitude;
    EStuffCondition effectCondition;
    EStuffEffectType effectType;
    for(int32 i = 0; i < effectsNodes.size(); i++)
    {
        effectsNodes[i]->getValue(L"type", stringRead);
        stringRead.toUpper();
        effectType = toStuffEffectType(stringRead);

        effectsNodes[i]->getValue(L"condition", stringRead);
        stringRead.toUpper();
        effectCondition = toStuffCondition(stringRead);

        effectsNodes[i]->getValue(L"magnitude", effectMagnitude);

        _pEffects->push_back(StuffEffect(effectType, effectMagnitude, effectCondition));
    }
}
}
