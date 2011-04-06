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
#include "LockUsers.h"

namespace HOOAdminProtocol
{

//-----------------------------------------------------------------------------
//  LockUsers
//-----------------------------------------------------------------------------
void LockUsers::read(Core::InputStream & inputStream)
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

void LockUsers::write(Core::OutputStream & outputStream) const
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
//  LockUsersAnswer
//-----------------------------------------------------------------------------
void LockUsersAnswer::read(Core::InputStream & inputStream)
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

    int32 nbLocked;
    inputStream.read(nbLocked);

    LM_ASSERT(nbLogin == nbLocked);

    _locked.clear();
    for (int32 ii=0; ii<nbLocked; ++ii)
    {
        bool locked;
        inputStream.read(locked);
        _locked.push_back(locked);
    }
}

void LockUsersAnswer::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());

    int32 nbLogin = _logins.size(); 
    outputStream.write(nbLogin);

    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        outputStream.write(_logins[ii]);
    }

    int32 nbLocked = _locked.size(); 

    LM_ASSERT(nbLogin == nbLocked);

    outputStream.write(nbLocked);
    for (int32 ii=0; ii<nbLocked; ++ii)
    {
        outputStream.write(_locked[ii]);
    }
}


//-----------------------------------------------------------------------------
//  UnlockUsers
//-----------------------------------------------------------------------------
void UnlockUsers::read(Core::InputStream & inputStream)
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

void UnlockUsers::write(Core::OutputStream & outputStream) const
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
//  UnlockUsersAnswer
//-----------------------------------------------------------------------------
void UnlockUsersAnswer::read(Core::InputStream & inputStream)
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

    int32 nbUnlocked;
    inputStream.read(nbUnlocked);

    LM_ASSERT(nbLogin == nbUnlocked);

    _unlocked.clear();
    for (int32 ii=0; ii<nbUnlocked; ++ii)
    {
        bool unlocked;
        inputStream.read(unlocked);
        _unlocked.push_back(unlocked);
    }
}

void UnlockUsersAnswer::write(Core::OutputStream & outputStream) const
{
    outputStream.write(getType());

    int32 nbLogin = _logins.size(); 
    outputStream.write(nbLogin);

    for (int32 ii=0; ii<nbLogin; ++ii)
    {
        outputStream.write(_logins[ii]);
    }

    int32 nbUnlocked = _unlocked.size(); 

    LM_ASSERT(nbLogin == nbUnlocked);

    outputStream.write(nbUnlocked);
    for (int32 ii=0; ii<nbUnlocked; ++ii)
    {
        outputStream.write(_unlocked[ii]);
    }
}

}//namespace HOOAdminProtocol