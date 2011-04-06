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
#ifndef CHRONORAGE_FORMATION_H_
#define CHRONORAGE_FORMATION_H_

#include <Foes/Foe.h>
#include <Core/LinkedList.h>

namespace ChronoRage
{

struct LM_API_CHR FoeRequest
{
    FoeRequest(EFoeType foeType, const Core::Vector2f & position, const String & arg, float timer);
    FoeRequest(){}

    EFoeType        foeType;
    String          arg;
    Core::Vector2f  position;
    float           timer;

};

LM_ENUM_3(EFormationType,   FORMATION_FIXED,
                            FORMATION_PLAYER,
                            FORMATION_RANDOM);

class LM_API_CHR Formation : public Core::Object
{
public:
    Formation(EFormationType type, const Core::Vector2f & position);
    virtual ~Formation(){}

     void addFoeRequest(EFoeType foeType, const Core::Vector2f & position, const String & arg, float timer);
     void getFoeRequest(Core::List<Ptr<FoeRequest> > & foeRequests, float elapsed);

     bool empty() const { return _foeRequests.empty(); }

     EFormationType getType() const { return _type; }
     bool playerPositionSet() const { return _playerPositionSet; }
     const Core::Vector2f & getPosition() const { return _position; }
     void setPosition(const Core::Vector2f & position);

private:
    EFormationType  _type;
    Core::Vector2f  _position;
    Core::LinkedList<Ptr<FoeRequest> > _foeRequests;

    bool            _playerPositionSet;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_FORMATION_H_*/