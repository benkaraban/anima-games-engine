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
#ifndef HOOUPDATERPROTOCOL_UPDATERMESSAGE_H_
#define HOOUPDATERPROTOCOL_UPDATERMESSAGE_H_

#include <Core/Standard.h>
#include <Core/IO/Streams.h>

namespace HOOUpdaterProtocol
{

/**
 * Taille maximale "par défaut" d'une chaîne quand on faire un readSafe.
 * Ca ne veut pas dire que la chaîne sera acceptée pour autant (elle peut être trop
 * longue pour le login par exemple). C'est juste une protection pour éviter qu'un
 * message corrompu/élaboré par un hacker ne fasse exploser la consommation mémoire.
 */
    
const int32 UPDATER_SAFE_STRING_LENGTH  = 1024;
const int32 UPDATER_SAFE_LIST_SIZE      = 1024;

LM_ENUM_2(EUpdaterRequestType,    NULL_REQUEST,
                                  UPDATER_CONNECT)

class LM_API_HUDP IUpdaterRequest : public Core::Serializable
{
public:
    virtual ~IUpdaterRequest(){}

    virtual EUpdaterRequestType getType() const = 0;
};

LM_ENUM_2(EUpdaterAnswerType, NULL_ANSWER,
                              UPDATER_CONNECT_ANSWER)

class LM_API_HUDP IUpdaterAnswer : public Core::Serializable
{
public:
    virtual ~IUpdaterAnswer(){}

    virtual EUpdaterAnswerType getType() const = 0;
};

}

#endif