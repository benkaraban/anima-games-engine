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
#include "ConnectedUsersInfos.h"

namespace HOOAdminProtocol
{
//-----------------------------------------------------------------------------
//  UserInfos
//-----------------------------------------------------------------------------
UserInfos::UserInfos() :
    loggedIn(false),
    login(L""),
    qMatchStatus(-1)
{
}

void UserInfos::read(Core::InputStream & inputStream)
{
    loggedIn        = inputStream.readBool();
    login           = inputStream.readString();
    qMatchStatus    = inputStream.readInt32();
}

void UserInfos::write(Core::OutputStream & outputStream) const
{
    outputStream.write(loggedIn);
    outputStream.write(login);
    outputStream.write(qMatchStatus);
}
//-----------------------------------------------------------------------------
//  ConnectedUsersInfos
//-----------------------------------------------------------------------------
void ConnectedUsersInfos::read(Core::InputStream & inputStream)
{
}

void ConnectedUsersInfos::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
}
//-----------------------------------------------------------------------------
//  ConnectedUsersInfosAnswer
//-----------------------------------------------------------------------------
void ConnectedUsersInfosAnswer::read(Core::InputStream & inputStream)
{
    int32 nbUsers = 0;
    inputStream.readData(&nbUsers, sizeof(int32));
    _infos.resize(nbUsers);

    for (int32 ii=0; ii<_infos.size(); ++ii)
    {
        _infos[ii].read(inputStream);
    }
}
void ConnectedUsersInfosAnswer::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());

    int32 nbUsers = _infos.size();
    outputStream.write(nbUsers);
    for (int32 ii=0; ii<_infos.size(); ++ii)
    {
        _infos[ii].write(outputStream);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace HOOAdminProtocol