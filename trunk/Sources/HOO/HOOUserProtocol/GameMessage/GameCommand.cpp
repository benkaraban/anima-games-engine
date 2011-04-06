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
#include <Core/IO/Tools.h>
#include <HOOUserProtocol/GameMessage/GameCommand.h>

namespace HOOUserProtocol
{
//-----------------------------------------------------------------------------
void GameCommand::read(Core::InputStream & inputStream)
{
    inputStream.readData(&_command, sizeof(_command));
    inputStream.read(_action);
}
//-----------------------------------------------------------------------------
void GameCommand::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
    outputStream.writeData(&_command, sizeof(_command));
    outputStream.write(_action);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
void GameCommandAnswer::read(Core::InputStream & inputStream)
{
    inputStream.readData(&_command, sizeof(_command));

    switch(_command)
    {
    case GAME_START:
        inputStream.read(_status1);
        break;
    case PLAYER_LEFT:
    case GAME_FINISHED:
        inputStream.read(_casterId);
        break;
    case WILL_CONTEST:
        inputStream.read(_manaSphere);
        inputStream.read(_manaCount);
        break;
    case WILL_RESOLVE:
        inputStream.read(_bidWinner);
        inputStream.read(_status1);
        break;
    case PLAYER_TURN:
        inputStream.read(_casterId);
        break;
    case PLAYER_START_PASS:
        inputStream.read(_casterId);
        break;
    case PLAYER_CAST_SPELL:
    case PLAYER_CAST_TRICK:
    case PLAYER_CAST_RETRIBUTION:
        inputStream.read(_casterId);
        inputStream.read(_spellId);
        inputStream.read(_status1);
        break;
    case PLAYER_PASS_FINISH:
    case PLAYER_SPELL_HIT:
    case PLAYER_TRICK_HIT:
    case PLAYER_RETRIBUTION_HIT:
        inputStream.read(_status1);
        break;
    case PLAYER_PASS_FINISH_AFTER_TRICK:
    case PLAYER_SPELL_HIT_AFTER_TRICK:
    case PLAYER_TRICK_HIT_AFTER_TRICK:
    case PLAYER_RETRIBUTION_HIT_AFTER_TRICK:
        inputStream.read(_casterId);
        inputStream.read(_spellId);
        inputStream.read(_status1);
        inputStream.read(_status2);
        break;
    case PLAYER_SPELL_COUNTERED:
        inputStream.read(_casterId);
        inputStream.read(_spellId);
        inputStream.read(_status1);
        inputStream.read(_status2);
        break;
    default:
        LM_ASSERT(false);
    };

    Core::readSafe(inputStream, _availableActions, SAFE_LIST_SIZE);
}
//-----------------------------------------------------------------------------
void GameCommandAnswer::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
    outputStream.writeData(&_command, sizeof(_command));

    switch(_command)
    {
    case GAME_START:
        outputStream.write(_status1);
        break;
    case PLAYER_LEFT:
    case GAME_FINISHED:
        outputStream.write(_casterId);
        break;
    case WILL_CONTEST:
        outputStream.write(_manaSphere);
        outputStream.write(_manaCount);
        break;
    case WILL_RESOLVE:
        outputStream.write(_bidWinner);
        outputStream.write(_status1);
        break;
    case PLAYER_TURN:
        outputStream.write(_casterId);
        break;
    case PLAYER_START_PASS:
        outputStream.write(_casterId);
        break;
    case PLAYER_CAST_SPELL:
    case PLAYER_CAST_TRICK:
    case PLAYER_CAST_RETRIBUTION:
        outputStream.write(_casterId);
        outputStream.write(_spellId);
        outputStream.write(_status1);
        break;
    case PLAYER_PASS_FINISH:
    case PLAYER_SPELL_HIT:
    case PLAYER_TRICK_HIT:
    case PLAYER_RETRIBUTION_HIT:
        outputStream.write(_status1);
        break;
    case PLAYER_PASS_FINISH_AFTER_TRICK:
    case PLAYER_SPELL_HIT_AFTER_TRICK:
    case PLAYER_TRICK_HIT_AFTER_TRICK:
    case PLAYER_RETRIBUTION_HIT_AFTER_TRICK:
        outputStream.write(_casterId);
        outputStream.write(_spellId);
        outputStream.write(_status1);
        outputStream.write(_status2);
        break;
    case PLAYER_SPELL_COUNTERED:
        outputStream.write(_casterId);
        outputStream.write(_spellId);
        outputStream.write(_status1);
        outputStream.write(_status2);
        break;
    default:
        LM_ASSERT(false);
    };

    Core::write(outputStream, _availableActions);
}
//-----------------------------------------------------------------------------
}



