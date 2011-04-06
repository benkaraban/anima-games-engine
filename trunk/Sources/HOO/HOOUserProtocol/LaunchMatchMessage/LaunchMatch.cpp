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
#include "LaunchMatch.h"

namespace HOOUserProtocol
{
EUserLoadingAnswerType transmitToOpponent(EUserLoadingType loadingType)
{
    EUserLoadingAnswerType returnCode = OPPONENT_LOADING_0_PERCENT;
    switch(loadingType)
    {
    case USER_LOADING_0_PERCENT:
        returnCode = OPPONENT_LOADING_0_PERCENT;
        break;
    case USER_LOADING_25_PERCENT:
        returnCode = OPPONENT_LOADING_25_PERCENT;
        break;
    case USER_LOADING_50_PERCENT:
        returnCode = OPPONENT_LOADING_50_PERCENT;
        break;
    case USER_LOADING_75_PERCENT:
        returnCode = OPPONENT_LOADING_75_PERCENT;
        break;
    case USER_LOADING_FINISHED:
        returnCode = OPPONENT_LOADING_FINISHED;
        break;
    }
    return returnCode;
}
//-----------------------------------------------------------------------------
//  LaunchMatch
//-----------------------------------------------------------------------------
void LaunchMatch::read(Core::InputStream & inputStream)
{
}

void LaunchMatch::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
}
//-----------------------------------------------------------------------------
//  LaunchMatchAnswer
//-----------------------------------------------------------------------------
void LaunchMatchAnswer::read(Core::InputStream & inputStream)
{
    inputStream.readData(&_answerType, sizeof(ELaunchMatchAnswerType));

    if(_answerType == LAUNCH_MATCH)
    {
        inputStream.read(_playerId);
        inputStream.read(_levelId);
        inputStream.read(_opponent);
    }
}

void LaunchMatchAnswer::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
    outputStream.write(_answerType);

    if(_answerType == LAUNCH_MATCH)
    {
        outputStream.write(_playerId);
        outputStream.write(_levelId);
        outputStream.write(_opponent);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  UserLoading
//-----------------------------------------------------------------------------
void UserLoading::read(Core::InputStream & inputStream)
{
    inputStream.readData(&_requestType, sizeof(EUserLoadingType));
}

void UserLoading::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
    outputStream.write(_requestType);
}
//-----------------------------------------------------------------------------
//  UserLoadingAnswer
//-----------------------------------------------------------------------------
void UserLoadingAnswer::read(Core::InputStream & inputStream)
{
    inputStream.readData(&_answerType, sizeof(EUserLoadingAnswerType));
}
void UserLoadingAnswer::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
    outputStream.write(_answerType);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}//namespace HOOUserProtocol