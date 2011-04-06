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
#include <HOOUserProtocol/GameMessage/GameData.h>
#include <Core/IO/Tools.h>

namespace HOOUserProtocol
{
//-----------------------------------------------------------------------------
ActionCommand::ActionCommand() : actionType(2), actionValue(0)
{
}
//-----------------------------------------------------------------------------
void ActionCommand::read(Core::InputStream & input)
{
    actionType = int32(input.readUInt8());
    if(actionType != 2)
        actionValue = input.readInt32();
}
//-----------------------------------------------------------------------------
void ActionCommand::write(Core::OutputStream & output) const
{
    output.writeUInt8(actionType);
    if(actionType != 2)
        output.write(actionValue);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
PlayerSphereStatus::PlayerSphereStatus() : mana(0), absorb(0), chaining(0), impair(0)
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PlayerSphereStatusUpdate::PlayerSphereStatusUpdate() : _flags(0)
{
}
//-----------------------------------------------------------------------------
PlayerSphereStatusUpdate::PlayerSphereStatusUpdate(const PlayerSphereStatus & newStatus) : _flags(0xFF), _status(newStatus)
{
}
//-----------------------------------------------------------------------------
PlayerSphereStatusUpdate::PlayerSphereStatusUpdate(const PlayerSphereStatus & oldStatus, const PlayerSphereStatus & newStatus) : _flags(0)
{
    setUpdateAsDiff(oldStatus, newStatus);
}
//-----------------------------------------------------------------------------
void PlayerSphereStatusUpdate::setUpdateAsDiff(const PlayerSphereStatus & oldStatus, const PlayerSphereStatus & newStatus)
{
    _flags = 0;
    _status = newStatus;

    if(oldStatus.mana != newStatus.mana)            _flags |= UPDATE_FLAG_MANA;
    if(oldStatus.absorb != newStatus.absorb)        _flags |= UPDATE_FLAG_ABSORB;
    if(oldStatus.chaining != newStatus.chaining)    _flags |= UPDATE_FLAG_CHAINING;
    if(oldStatus.impair != newStatus.impair)        _flags |= UPDATE_FLAG_IMPAIR;
}
//-----------------------------------------------------------------------------
void PlayerSphereStatusUpdate::applyUpdate(PlayerSphereStatus & status) const
{
    if((_flags & UPDATE_FLAG_MANA) != 0)            status.mana = _status.mana;
    if((_flags & UPDATE_FLAG_ABSORB) != 0)          status.absorb = _status.absorb;
    if((_flags & UPDATE_FLAG_CHAINING) != 0)        status.chaining = _status.chaining;
    if((_flags & UPDATE_FLAG_IMPAIR) != 0)          status.impair = _status.impair;
}
//-----------------------------------------------------------------------------
void PlayerSphereStatusUpdate::read(Core::InputStream & input)
{
    _flags = input.readUInt8();

    if((_flags & UPDATE_FLAG_MANA) != 0)            _status.mana     = int32(input.readInt8());
    if((_flags & UPDATE_FLAG_ABSORB) != 0)          _status.absorb   = int32(input.readInt8());
    if((_flags & UPDATE_FLAG_CHAINING) != 0)        _status.chaining = int32(input.readInt8());
    if((_flags & UPDATE_FLAG_IMPAIR) != 0)          _status.impair   = int32(input.readInt8());
}
//-----------------------------------------------------------------------------
void PlayerSphereStatusUpdate::write(Core::OutputStream & output) const
{
    output.writeUInt8(_flags);

    if((_flags & UPDATE_FLAG_MANA) != 0)            output.writeInt8(_status.mana);
    if((_flags & UPDATE_FLAG_ABSORB) != 0)          output.writeInt8(_status.absorb);
    if((_flags & UPDATE_FLAG_CHAINING) != 0)        output.writeInt8(_status.chaining);
    if((_flags & UPDATE_FLAG_IMPAIR) != 0)          output.writeInt8(_status.impair);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
SpellCooldown::SpellCooldown() : spellId(0), cooldown(0)
{
}
//-----------------------------------------------------------------------------
void SpellCooldown::read(Core::InputStream & input)
{
    input.read(spellId);
    input.read(cooldown);
}
//-----------------------------------------------------------------------------
void SpellCooldown::write(Core::OutputStream & output) const
{
    output.write(spellId);
    output.write(cooldown);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
PlayerStatus::PlayerStatus() : health(1), willpower(0), nextTurnsToSee(0), hasVantage(false)
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PlayerStatusUpdate::PlayerStatusUpdate() : _flags(0)
{
}
//-----------------------------------------------------------------------------
PlayerStatusUpdate::PlayerStatusUpdate(const PlayerStatus & newStatus) : _flags(0xFF), _status(newStatus)
{
    for(int32 ii=0; ii < 3; ii++)
        _spheres[ii] = PlayerSphereStatusUpdate(newStatus.spheres[ii]);
}
//-----------------------------------------------------------------------------
PlayerStatusUpdate::PlayerStatusUpdate(const PlayerStatus & oldStatus, const PlayerStatus & newStatus)
{
    setUpdateAsDiff(oldStatus, newStatus);

    for(int32 ii=0; ii < 3; ii++)
        _spheres[ii] = PlayerSphereStatusUpdate(oldStatus.spheres[ii], newStatus.spheres[ii]);
}
//-----------------------------------------------------------------------------
void PlayerStatusUpdate::setUpdateAsDiff(const PlayerStatus & oldStatus, const PlayerStatus & newStatus)
{
    _flags = 0;
    _status = newStatus;

    for(int32 ii=0; ii < 3; ii++)
        _spheres[ii].setUpdateAsDiff(oldStatus.spheres[ii], newStatus.spheres[ii]);

    if(newStatus.health != oldStatus.health) _flags |= UPDATE_FLAG_HEALTH;
    if(newStatus.willpower != oldStatus.willpower) _flags |= UPDATE_FLAG_WILLPOWER;
    if(newStatus.willpower != oldStatus.willpower) _flags |= UPDATE_FLAG_SEE_NEXT_TURN;

    if(!_spheres[0].isNull()) _flags |= UPDATE_FLAG_SPHERE_1;
    if(!_spheres[1].isNull()) _flags |= UPDATE_FLAG_SPHERE_2;
    if(!_spheres[2].isNull()) _flags |= UPDATE_FLAG_SPHERE_3;
}
//-----------------------------------------------------------------------------
void PlayerStatusUpdate::applyUpdate(PlayerStatus & status) const
{
    if((_flags & UPDATE_FLAG_HEALTH) != 0)          status.health = _status.health;
    if((_flags & UPDATE_FLAG_WILLPOWER) != 0)       status.willpower = _status.willpower;
    if((_flags & UPDATE_FLAG_SEE_NEXT_TURN) != 0)   status.nextTurnsToSee = _status.nextTurnsToSee;

    if((_flags & UPDATE_FLAG_SPHERE_1) != 0)        _spheres[0].applyUpdate(status.spheres[0]);
    if((_flags & UPDATE_FLAG_SPHERE_2) != 1)        _spheres[1].applyUpdate(status.spheres[1]);
    if((_flags & UPDATE_FLAG_SPHERE_3) != 2)        _spheres[2].applyUpdate(status.spheres[2]);

    status.hasVantage = _status.hasVantage;
    status.cooldowns = _status.cooldowns;
}
//-----------------------------------------------------------------------------
void PlayerStatusUpdate::read(Core::InputStream & input)
{
    _flags = input.readUInt8();

    if((_flags & UPDATE_FLAG_HEALTH) != 0)            _status.health     = int32(input.readInt16());
    if((_flags & UPDATE_FLAG_WILLPOWER) != 0)         _status.willpower   = int32(input.readInt16());
    if((_flags & UPDATE_FLAG_SEE_NEXT_TURN) != 0)     _status.nextTurnsToSee = int32(input.readInt8());

    if((_flags & UPDATE_FLAG_SPHERE_1) != 0)          _spheres[0].read(input);
    if((_flags & UPDATE_FLAG_SPHERE_2) != 1)          _spheres[1].read(input);
    if((_flags & UPDATE_FLAG_SPHERE_3) != 2)          _spheres[2].read(input);

    input.read(_status.hasVantage);

    Core::readSafe(input, _status.cooldowns, SAFE_LIST_SIZE);
}
//-----------------------------------------------------------------------------
void PlayerStatusUpdate::write(Core::OutputStream & output) const
{
    output.writeUInt8(_flags);

    if((_flags & UPDATE_FLAG_HEALTH) != 0)            output.writeInt16(_status.health);
    if((_flags & UPDATE_FLAG_WILLPOWER) != 0)         output.writeInt16(_status.willpower);
    if((_flags & UPDATE_FLAG_SEE_NEXT_TURN) != 0)     output.writeInt8(_status.nextTurnsToSee);

    if((_flags & UPDATE_FLAG_SPHERE_1) != 0)          _spheres[0].write(output);
    if((_flags & UPDATE_FLAG_SPHERE_2) != 1)          _spheres[1].write(output);
    if((_flags & UPDATE_FLAG_SPHERE_3) != 2)          _spheres[2].write(output);

    output.write(_status.hasVantage);

    Core::write(output, _status.cooldowns);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
//OpponentSphereStatus::OpponentSphereStatus() : mana(0), absorb(0), chaining(0), impair(0)
//{
//}
////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
//OpponentSphereStatusUpdate::OpponentSphereStatusUpdate() : _flags(0)
//{
//}
////-----------------------------------------------------------------------------
//OpponentSphereStatusUpdate::OpponentSphereStatusUpdate(const OpponentSphereStatus & newStatus) : _flags(0xFF), _status(newStatus)
//{
//}
////-----------------------------------------------------------------------------
//OpponentSphereStatusUpdate::OpponentSphereStatusUpdate(const OpponentSphereStatus & oldStatus, const OpponentSphereStatus & newStatus) : _flags(0)
//{
//    setUpdateAsDiff(oldStatus, newStatus);
//}
////-----------------------------------------------------------------------------
//void OpponentSphereStatusUpdate::setUpdateAsDiff(const OpponentSphereStatus & oldStatus, const OpponentSphereStatus & newStatus)
//{
//    _flags = 0;
//    _status = newStatus;
//
//    if(oldStatus.mana != newStatus.mana)            _flags |= UPDATE_FLAG_MANA;
//    if(oldStatus.absorb != newStatus.absorb)        _flags |= UPDATE_FLAG_ABSORB;
//    if(oldStatus.chaining != newStatus.chaining)    _flags |= UPDATE_FLAG_CHAINING;
//    if(oldStatus.impair != newStatus.impair)        _flags |= UPDATE_FLAG_IMPAIR;
//}
////-----------------------------------------------------------------------------
//void OpponentSphereStatusUpdate::applyUpdate(OpponentSphereStatus & status) const
//{
//    if((_flags & UPDATE_FLAG_MANA) != 0)            status.mana = _status.mana;
//    if((_flags & UPDATE_FLAG_ABSORB) != 0)          status.absorb = _status.absorb;
//    if((_flags & UPDATE_FLAG_CHAINING) != 0)        status.chaining = _status.chaining;
//    if((_flags & UPDATE_FLAG_IMPAIR) != 0)          status.impair = _status.impair;
//}
////-----------------------------------------------------------------------------
//void OpponentSphereStatusUpdate::read(Core::InputStream & input)
//{
//    _flags = input.readUInt8();
//
//    if((_flags & UPDATE_FLAG_MANA) != 0)            _status.mana     = int32(input.readInt8());
//    if((_flags & UPDATE_FLAG_ABSORB) != 0)          _status.absorb   = int32(input.readInt8());
//    if((_flags & UPDATE_FLAG_CHAINING) != 0)        _status.chaining = int32(input.readInt8());
//    if((_flags & UPDATE_FLAG_IMPAIR) != 0)          _status.impair   = int32(input.readInt8());
//}
////-----------------------------------------------------------------------------
//void OpponentSphereStatusUpdate::write(Core::OutputStream & output) const
//{
//    output.writeUInt8(_flags);
//
//    if((_flags & UPDATE_FLAG_MANA) != 0)            output.writeInt8(_status.mana);
//    if((_flags & UPDATE_FLAG_ABSORB) != 0)          output.writeInt8(_status.absorb);
//    if((_flags & UPDATE_FLAG_CHAINING) != 0)        output.writeInt8(_status.chaining);
//    if((_flags & UPDATE_FLAG_IMPAIR) != 0)          output.writeInt8(_status.impair);
//}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
OpponentStatus::OpponentStatus() : health(1), hasVantage(false)
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
OpponentStatusUpdate::OpponentStatusUpdate() : _flags(0)
{
}
//-----------------------------------------------------------------------------
OpponentStatusUpdate::OpponentStatusUpdate(const OpponentStatus & newStatus) : _flags(0xFF), _status(newStatus)
{
    for(int32 ii=0; ii < 3; ii++)
        _spheres[ii] = PlayerSphereStatusUpdate(newStatus.spheres[ii]);
}
//-----------------------------------------------------------------------------
OpponentStatusUpdate::OpponentStatusUpdate(const OpponentStatus & oldStatus, const OpponentStatus & newStatus)
{
    setUpdateAsDiff(oldStatus, newStatus);
}
//-----------------------------------------------------------------------------
void OpponentStatusUpdate::setUpdateAsDiff(const OpponentStatus & oldStatus, const OpponentStatus & newStatus)
{
    _flags = 0;
    _status = newStatus;

    for(int32 ii=0; ii < 3; ii++)
        _spheres[ii].setUpdateAsDiff(oldStatus.spheres[ii], newStatus.spheres[ii]);

    if(newStatus.health != oldStatus.health) _flags |= UPDATE_FLAG_OPPONENT_HEALTH;
    if(!_spheres[0].isNull()) _flags |= UPDATE_FLAG_SPHERE_1;
    if(!_spheres[1].isNull()) _flags |= UPDATE_FLAG_SPHERE_2;
    if(!_spheres[2].isNull()) _flags |= UPDATE_FLAG_SPHERE_3;
}
//-----------------------------------------------------------------------------
void OpponentStatusUpdate::applyUpdate(OpponentStatus & status) const
{
    if((_flags & UPDATE_FLAG_OPPONENT_HEALTH) != 0)      status.health = _status.health;

    if((_flags & UPDATE_FLAG_SPHERE_1) != 0)        _spheres[0].applyUpdate(status.spheres[0]);
    if((_flags & UPDATE_FLAG_SPHERE_2) != 1)        _spheres[1].applyUpdate(status.spheres[1]);
    if((_flags & UPDATE_FLAG_SPHERE_3) != 2)        _spheres[2].applyUpdate(status.spheres[2]);

    status.hasVantage = _status.hasVantage;
}
//-----------------------------------------------------------------------------
void OpponentStatusUpdate::read(Core::InputStream & input)
{
    _flags = input.readUInt8();

    if((_flags & UPDATE_FLAG_OPPONENT_HEALTH) != 0)       _status.health     = int32(input.readUInt8());
    if((_flags & UPDATE_FLAG_SPHERE_1) != 0)          _spheres[0].read(input);
    if((_flags & UPDATE_FLAG_SPHERE_2) != 1)          _spheres[1].read(input);
    if((_flags & UPDATE_FLAG_SPHERE_3) != 2)          _spheres[2].read(input);

    input.read(_status.hasVantage);
}
//-----------------------------------------------------------------------------
void OpponentStatusUpdate::write(Core::OutputStream & output) const
{
    output.writeUInt8(_flags);

    if((_flags & UPDATE_FLAG_OPPONENT_HEALTH) != 0)       output.writeInt8(_status.health);
    if((_flags & UPDATE_FLAG_SPHERE_1) != 0)          _spheres[0].write(output);
    if((_flags & UPDATE_FLAG_SPHERE_2) != 1)          _spheres[1].write(output);
    if((_flags & UPDATE_FLAG_SPHERE_3) != 2)          _spheres[2].write(output);

    output.write(_status.hasVantage);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
GameStatus::GameStatus()
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GameStatusUpdate::GameStatusUpdate() : _flags(0)
{
}
//-----------------------------------------------------------------------------
GameStatusUpdate::GameStatusUpdate(const GameStatus & newStatus) : _flags(0xFF), _player(newStatus.player), _opponent(newStatus.opponent)
{
}
//-----------------------------------------------------------------------------
GameStatusUpdate::GameStatusUpdate(const GameStatus & oldStatus, const GameStatus & newStatus)
{
    setUpdateAsDiff(oldStatus, newStatus);
}
//-----------------------------------------------------------------------------
void GameStatusUpdate::setUpdateAsDiff(const GameStatus & oldStatus, const GameStatus & newStatus)
{
    _flags = 0;

    _player.setUpdateAsDiff(oldStatus.player, newStatus.player);
    _opponent.setUpdateAsDiff(oldStatus.opponent, newStatus.opponent);

    if(!_player.isNull()) _flags |= UPDATE_FLAG_PLAYER;
    if(!_opponent.isNull()) _flags |= UPDATE_FLAG_OPPONENT;
}
//-----------------------------------------------------------------------------
void GameStatusUpdate::applyUpdate(GameStatus & status) const
{
    if((_flags & UPDATE_FLAG_PLAYER) != 0)      _player.applyUpdate(status.player);
    if((_flags & UPDATE_FLAG_OPPONENT) != 0)    _opponent.applyUpdate(status.opponent);
}
//-----------------------------------------------------------------------------
void GameStatusUpdate::read(Core::InputStream & input)
{
    _flags = input.readUInt8();

    if((_flags & UPDATE_FLAG_PLAYER) != 0)      _player.read(input);
    if((_flags & UPDATE_FLAG_OPPONENT) != 0)    _opponent.read(input);
}
//-----------------------------------------------------------------------------
void GameStatusUpdate::write(Core::OutputStream & output) const
{
    output.writeUInt8(_flags);

    if((_flags & UPDATE_FLAG_PLAYER) != 0)      _player.write(output);
    if((_flags & UPDATE_FLAG_OPPONENT) != 0)    _opponent.write(output);
}
//-----------------------------------------------------------------------------
}



