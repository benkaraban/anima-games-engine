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
#ifndef HOOUSERPROTOCOL_LAUNCHMATCH_H_
#define HOOUSERPROTOCOL_LAUNCHMATCH_H_

#include <HOOUserProtocol/IUserMessage.h>
#include <HOOUserProtocol/UserAccountInfos.h>

namespace HOOUserProtocol
{

//-----------------------------------------------------------------------------
//  LaunchMatch
//-----------------------------------------------------------------------------
class LM_API_HUP LaunchMatch : public Core::Object, public IUserRequest
{
public:
    LaunchMatch()
    {}

    virtual ~LaunchMatch(){}

    virtual EUserRequestType getType() const { return USER_LAUNCH_MATCH; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
};

LM_ENUM_2(ELaunchMatchAnswerType,   LAUNCH_MATCH,
                                    OPPONENT_CANCELLED)

class LM_API_HUP LaunchMatchAnswer : public Core::Object, public IUserAnswer
{
public:
    LaunchMatchAnswer()
    {}

    LaunchMatchAnswer(ELaunchMatchAnswerType type)
    :   _answerType(type), _playerId(0)
    {}

    LaunchMatchAnswer(ELaunchMatchAnswerType type, int32 playerId, int32 levelId, const OpponentInfos & opponent)
    :   _answerType(type), _playerId(playerId), _levelId(levelId), _opponent(opponent)
    {}

    virtual ~LaunchMatchAnswer(){}

    virtual EUserAnswerType getType() const { return USER_LAUNCH_MATCH_ANSWER; }

    int32 getPlayerId() const { return _playerId; }
    int32 getLevelId() const { return _levelId; }
    const OpponentInfos & getOpponentInfos() const { return _opponent; }

    ELaunchMatchAnswerType getLaunchMatchAnswerType() const { return _answerType; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    ELaunchMatchAnswerType  _answerType;
    int32                   _playerId;
    int32                   _levelId;
    OpponentInfos           _opponent;
};
//-----------------------------------------------------------------------------
//  UserLoading
//-----------------------------------------------------------------------------
LM_ENUM_5(EUserLoadingType, USER_LOADING_0_PERCENT,
                            USER_LOADING_25_PERCENT,
                            USER_LOADING_50_PERCENT,
                            USER_LOADING_75_PERCENT,
                            USER_LOADING_FINISHED
                            )

class LM_API_HUP UserLoading : public Core::Object, public IUserRequest
{
public:
    UserLoading()
    {}

    UserLoading(EUserLoadingType type)
    :   _requestType(type)
    {}

    virtual ~UserLoading(){}

    virtual EUserRequestType getType() const { return USER_LOADING; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    EUserLoadingType getUserLoadingType() const {return _requestType;}

protected:
    EUserLoadingType    _requestType;
};

LM_ENUM_6(EUserLoadingAnswerType,   OPPONENT_LOADING_0_PERCENT,
                                    OPPONENT_LOADING_25_PERCENT,
                                    OPPONENT_LOADING_50_PERCENT,
                                    OPPONENT_LOADING_75_PERCENT,
                                    OPPONENT_LOADING_FINISHED,
                                    OPPONENT_DROPPED
                                    )

class LM_API_HUP UserLoadingAnswer : public Core::Object, public IUserAnswer
{
public:
    UserLoadingAnswer()
    {}

    UserLoadingAnswer(EUserLoadingAnswerType type)
    :   _answerType(type)
    {}

    virtual ~UserLoadingAnswer(){}

    virtual EUserAnswerType getType() const { return USER_LOADING_ANSWER; }
    EUserLoadingAnswerType getUserLoadingAnswerType() const { return _answerType; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    EUserLoadingAnswerType  _answerType;
};

LM_API_HUP EUserLoadingAnswerType transmitToOpponent(EUserLoadingType loadingType);

}//namespace HOOUserProtocol

#endif/*HOOUSERPROTOCOL_LAUNCHMATCH_H_*/