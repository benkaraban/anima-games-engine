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
#include "BanUsers.h"

namespace HOOAdminProtocol
{

//-----------------------------------------------------------------------------
//  BanUsers
//-----------------------------------------------------------------------------
void BanUsers::read(Core::InputStream & inputStream)
{
    inputStream.read(_nbDay);
    int32 nbLogin;
    inputStream.read(nbLogin);

    _logins.clear();
    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        String l;
        inputStream.read(l);
        _logins.push_back(l);
    }
}

void BanUsers::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
    
    outputStream.write(_nbDay);

    int32 nbLogin = _logins.size(); 
    outputStream.write(nbLogin);

    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        outputStream.write(_logins[ii]);
    }
}

//-----------------------------------------------------------------------------
//  BanUsersAnswer
//-----------------------------------------------------------------------------
void BanUsersAnswer::read(Core::InputStream & inputStream)
{
    int32 nbLogin;
    inputStream.read(nbLogin);

    _logins.clear();
    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        String l;
        inputStream.read(l);
        _logins.push_back(l);
    }

    int32 nbBanned;
    inputStream.read(nbBanned);
    
    LM_ASSERT(nbLogin == nbBanned);

    _banned.clear();
    for (int32 ii=0; ii<nbBanned; ++ii)
    {
        bool banned;
        inputStream.read(banned);
        _banned.push_back(banned);
    }
}

void BanUsersAnswer::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());

    int32 nbLogin = _logins.size(); 
    outputStream.write(nbLogin);

    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        outputStream.write(_logins[ii]);
    }

    int32 nbBanned = _banned.size(); 

    LM_ASSERT(nbLogin == nbBanned);

    outputStream.write(nbBanned);
    for (int32 ii=0; ii<nbBanned; ++ii)
    {
        outputStream.write(_banned[ii]);
    }
}


//-----------------------------------------------------------------------------
//  UnbanUsers
//-----------------------------------------------------------------------------
void UnbanUsers::read(Core::InputStream & inputStream)
{
    int32 nbLogin;
    inputStream.read(nbLogin);

    _logins.clear();
    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        String l;
        inputStream.read(l);
        _logins.push_back(l);
    }
}

void UnbanUsers::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());
   
    int32 nbLogin = _logins.size(); 
    outputStream.write(nbLogin);

    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        outputStream.write(_logins[ii]);
    }
}
//-----------------------------------------------------------------------------
//  UnbanUsersAnswer
//-----------------------------------------------------------------------------
void UnbanUsersAnswer::read(Core::InputStream & inputStream)
{
    int32 nbLogin;
    inputStream.read(nbLogin);

    _logins.clear();
    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        String l;
        inputStream.read(l);
        _logins.push_back(l);
    }

    int32 nbUnbanned;
    inputStream.read(nbUnbanned);

    _unbanned.clear();
    for (int32 ii=0; ii<nbUnbanned; ++ii)
    {
        bool unbanned;
        inputStream.read(unbanned);
        _unbanned.push_back(unbanned);
    }
}

void UnbanUsersAnswer::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());

    int32 nbLogin = _logins.size(); 
    outputStream.write(nbLogin);

    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        outputStream.write(_logins[ii]);
    }

    int32 nbUnbanned = _unbanned.size(); 

    LM_ASSERT(nbLogin == nbUnbanned);

    outputStream.write(nbUnbanned);
    for (int32 ii=0; ii<nbUnbanned; ++ii)
    {
        outputStream.write(_unbanned[ii]);
    }
}
}//namespace HOOAdminProtocol