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
#ifndef HOOUSERPROTOCOL_GAMECOMMANDMESSAGE_H_
#define HOOUSERPROTOCOL_GAMECOMMANDMESSAGE_H_

#include <Core/List.h>
#include <HOOUserProtocol/IUserMessage.h>
#include <HOOUserProtocol/GameMessage/GameData.h>

namespace HOOUserProtocol
{
LM_ENUM_3(EGameCommand,
        PLAYER_ACTION,
        SYNCHRONISATION,
        LEAVE_GAME);

class LM_API_HUP GameCommand : public Core::Object, public IUserRequest
{
public:
    GameCommand() : _command(SYNCHRONISATION)
    {}

    GameCommand(EGameCommand command) : _command(command)
    {}

    GameCommand(const ActionCommand & action) : _command(PLAYER_ACTION), _action(action)
    {}

    EGameCommand getCommand() const { return _command; }

    const ActionCommand & getAction() const { return _action; }

    virtual EUserRequestType getType() const { return USER_GAME_COMMAND; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    EGameCommand    _command;
    ActionCommand   _action;
};

LM_ENUM_19(EGameCommandAnswer,

    GAME_START,                     // + status
    GAME_FINISHED,

    PLAYER_LEFT,

    WILL_CONTEST,                   // + mana
    WILL_RESOLVE,                   // + winner + status
    
    PLAYER_TURN,

    PLAYER_START_PASS,
    PLAYER_PASS_FINISH,
    PLAYER_PASS_FINISH_AFTER_TRICK,
    
    PLAYER_CAST_SPELL,              // + id spell
    PLAYER_SPELL_HIT,               // + status
    PLAYER_SPELL_HIT_AFTER_TRICK,   // + id trick
    PLAYER_SPELL_COUNTERED,         // + id counter
    
    PLAYER_CAST_TRICK,              // + id trick
    PLAYER_TRICK_HIT,               // + status
    PLAYER_TRICK_HIT_AFTER_TRICK,   // + id trick, + status strick, + status after hit

    PLAYER_CAST_RETRIBUTION,        // + id retribution
    PLAYER_RETRIBUTION_HIT,         // + status after hit
    PLAYER_RETRIBUTION_HIT_AFTER_TRICK // + id trick, + status after trick, + status after hit
    );

    
class LM_API_HUP GameCommandAnswer : public Core::Object, public IUserAnswer
{
public:
    GameCommandAnswer()
    {}

    GameCommandAnswer(EGameCommandAnswer command) : _command(command)
    {}

    GameCommandAnswer(EGameCommandAnswer command, int32 casterId) : _command(command), _casterId(casterId)
    {}

    GameCommandAnswer(EGameCommandAnswer command, const GameStatusUpdate & status) : _command(command), _status1(status)
    {}

    GameCommandAnswer(EGameCommandAnswer command, int32 mana, int32 sphere) : _command(command), _manaSphere(sphere), _manaCount(mana)
    {}

    virtual EUserAnswerType getType() const { return USER_GAME_COMMAND_ANSWER; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    EGameCommandAnswer getCommand() const { return _command; }

    void setAvailableActions(const Core::List<ActionCommand> & actions) { _availableActions = actions; }
    void setStatusUpdate(const GameStatusUpdate & status) { _status1 = status; }
    void setStatusUpdateAfterOpponentTrick(const GameStatusUpdate & status) { _status2 = status; }
    void setStatusUpdateAfterOpponentCounter(const GameStatusUpdate & status) { _status2 = status; }

    void setCaster(int32 caster) { _casterId = caster; }
    void setSpell(int32 spell) { _spellId = spell; }

    void setManaSphere(int32 s) { _manaSphere = s; }
    void setManaCount(int32 c) { _manaCount = c; }

    void setBidWinner(int32 index) { _bidWinner = index; }


    int32 getCasterId() const { return _casterId; }
    int32 getSpellId() const { return _spellId; }

    int32 getManaSphere() const { return _manaSphere; }
    int32 getManaCount() const { return _manaCount; }

    int32 getBidWinner() const { return _bidWinner; }

    const Core::List<ActionCommand> & getAvailableActions() const { return _availableActions; }
    const GameStatusUpdate & getStatusUpdate() const { return _status1; }
    const GameStatusUpdate & getStatusUpdateAfterOpponentTrick() const { return _status2; }
    const GameStatusUpdate & getStatusUpdateAfterOpponentCounter() const { return _status2; }

public:
    EGameCommandAnswer          _command;
    
    union
    {
        struct // PLAYER_CAST_*
        {
            int32               _casterId;
            int32               _spellId;
        };
        
        struct // -> WILL_CONTEST
        {
            int32               _manaSphere;
            int32               _manaCount;
        };

        int32                   _bidWinner;
    };
    
    
    GameStatusUpdate            _status1;
    GameStatusUpdate            _status2;

    Core::List<ActionCommand>   _availableActions;
};

}//namespace HOOUserProtocol

#endif/*HOOUSERPROTOCOL_CHATMESSAGE_H_*/