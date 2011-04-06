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
#ifndef HOOUSERPROTOCOL_QUICKMATCH_H_
#define HOOUSERPROTOCOL_QUICKMATCH_H_

#include <HOOUserProtocol/IUserMessage.h>

namespace HOOUserProtocol
{
//-----------------------------------------------------------------------------
//  QuickMatch
//-----------------------------------------------------------------------------
class LM_API_HUP QuickMatch : public Core::Object, public IUserRequest
{
public:
    QuickMatch()
    {}

    virtual ~QuickMatch(){}

    virtual EUserRequestType getType() const { return USER_QUICK_MATCH; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
};

LM_ENUM_3(EQuickMatchAnswerType,    LOOKING_FOR_OPPONENT,
                                    OPPONENT_FOUND,
                                    ALREADY_LOOKING_FOR_OPPONENT)

class LM_API_HUP QuickMatchAnswer : public Core::Object, public IUserAnswer
{
public:
    QuickMatchAnswer()
    {}

    QuickMatchAnswer(EQuickMatchAnswerType type)
    :   _answerType(type)
    {}

    virtual ~QuickMatchAnswer(){}

    virtual EUserAnswerType getType() const { return USER_QUICK_MATCH_ANSWER; }

    EQuickMatchAnswerType getQuickMatchAnswerType() const { return _answerType; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    EQuickMatchAnswerType   _answerType;
};

//-----------------------------------------------------------------------------
//  CancelQuickMatch
//-----------------------------------------------------------------------------
class LM_API_HUP CancelQuickMatch : public Core::Object, public IUserRequest
{
public:
    CancelQuickMatch()
    {}

    virtual ~CancelQuickMatch(){}

    virtual EUserRequestType getType() const { return USER_CANCEL_QUICK_MATCH; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
};

LM_ENUM_2(ECancelQuickMatchAnswerType,  QUICK_MATCH_CANCELLED,
                                        ALREADY_IN_MATCH)

class LM_API_HUP CancelQuickMatchAnswer : public Core::Object, public IUserAnswer
{
public:
    CancelQuickMatchAnswer()
    {}

    CancelQuickMatchAnswer(ECancelQuickMatchAnswerType type)
    :   _answerType(type)
    {}

    virtual ~CancelQuickMatchAnswer(){}

    virtual EUserAnswerType getType() const { return USER_CANCEL_QUICK_MATCH_ANSWER; }

    virtual ECancelQuickMatchAnswerType getCancelQuickMatchAnswerType() const { return _answerType; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    ECancelQuickMatchAnswerType _answerType;
};

}//namespace HOOUserProtocol

#endif/*HOOUSERPROTOCOL_QUICKMATCH_H_*/