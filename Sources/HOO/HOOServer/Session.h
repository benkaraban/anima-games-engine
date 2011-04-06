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
#ifndef HOOSERVER_USERSESSION_H_
#define HOOSERVER_USERSESSION_H_

#include <Core/Standard.h>
#include <Core/Queue.h>
#include <Core/RT/Mutex.h>

#include <Network/Message.h>

#include <HOOServer/UserAccount.h>

namespace HOOServer
{

LM_ENUM_4(ESessionStatus,   SESSION_CLOSED,
                            SESSION_CONNECTING,
                            SESSION_CONNECTED,
                            SESSION_LOGGED);

LM_ENUM_7(EQuickMatchStatus,    QM_NOT_WAITING_FOR_QUICKMATCH,
                                QM_LOOKING_FOR_OPPONENT,
                                QM_OPPONENT_FOUND,
                                QM_WAITING_FOR_OPPONENT,
                                QM_LOADING,
                                QM_LOADING_FINISHED,
                                QM_FIGHTING)

class Game;
class UserSessionManager;

class LM_API_HOS Session : public Core::Object
{
public:
    Session();

    inline void setIdSession(int32 idSession) { _idSession = idSession; } 
    inline int32 getIdSession() const { return _idSession; }
    
    inline bool isClosed() const { return _sessionStatus == SESSION_CLOSED; }
    inline bool isConnecting() const { return _sessionStatus == SESSION_CONNECTING; }
    inline bool isConnected() const { return (_sessionStatus == SESSION_CONNECTED || _sessionStatus == SESSION_LOGGED); }
    inline bool isLoggedIn () const { return _sessionStatus == SESSION_LOGGED; }

          UserAccount & account()       { return _account; }
    const UserAccount & account() const { return _account; }

    void close();
    void open();

    void connect();

    inline EQuickMatchStatus getQuickMatchStatus() const { return _quickMatchStatus; }
    inline void setQuickMatchStatus(EQuickMatchStatus status) { _quickMatchStatus = status; }

    void opponentFound(Game * game);
    inline Game * getGame() { return _game; }
    inline void resetGame() { _game = NULL; }
            
    Core::Mutex & getGeneralMutex(){ return _generalMutex; }
    
    Core::Mutex & getReadWriteMutex(){ return _readWriteMutex; }

    void pushMessage(Network::Message * message);
    Network::Message * popMessage();

private:
    friend class UserSessionManager; 
    friend class AdminSessionManager;
    
    // ------

    void login(const UserAccount & account);
    void logout();

    // ----- end SessionManager methods

    Core::Queue<Network::Message *>     _messages;
    Core::Mutex                         _queueMutex;

    Core::Mutex                         _generalMutex;
    Core::Mutex                         _readWriteMutex;

    int32               _idSession;
    ESessionStatus      _sessionStatus;
    UserAccount         _account;

    EQuickMatchStatus   _quickMatchStatus;
    Game *              _game;
};

}//namespace HOOServer

#endif/*TVNETWORK_USERSESSION_H_*/
