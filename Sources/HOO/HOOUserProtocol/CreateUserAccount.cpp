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
#include "CreateUserAccount.h"

namespace HOOUserProtocol
{
//-----------------------------------------------------------------------------
//  CreateUserAccount
//-----------------------------------------------------------------------------
void CreateUserAccount::read(Core::InputStream & inputStream)
{
    inputStream.readSafe(_login, SAFE_STRING_LENGTH);
    inputStream.readSafe(_password, SAFE_STRING_LENGTH);
    inputStream.readSafe(_mail, SAFE_STRING_LENGTH);
    inputStream.read(_newsletter);
}

void CreateUserAccount::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
    outputStream.write(_login);
    outputStream.write(_password);
    outputStream.write(_mail);
    outputStream.write(_newsletter);
}
//-----------------------------------------------------------------------------
//  CreateUserAccountAnswer
//-----------------------------------------------------------------------------
void CreateUserAccountAnswer::read(Core::InputStream & inputStream)
{
    inputStream.readData(&_answerType, sizeof(ECreateUserAccountAnswerType));

    if(_answerType == USER_ACCOUNT_CREATED)
        _accountInfos.read(inputStream);
}
void CreateUserAccountAnswer::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
    outputStream.write(_answerType);

    if(_answerType == USER_ACCOUNT_CREATED)
        _accountInfos.write(outputStream);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace HOOUserProtocol