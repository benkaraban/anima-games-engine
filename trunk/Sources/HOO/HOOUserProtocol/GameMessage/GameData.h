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
#ifndef HOOUSERPROTOCOL_GAMEDATA_H_
#define HOOUSERPROTOCOL_GAMEDATA_H_

#include <Core/List.h>
#include <HOOUserProtocol/IUserMessage.h>

namespace HOOUserProtocol
{
// Player action --------------------------------------------------------------

struct LM_API_HUP ActionCommand : public Core::Object, public Core::Serializable
{
    ActionCommand();

    int32       actionType; // 0 => cast, 1 => bid, 2 => pass, 3 => synchro
    int32       actionValue;  // => id spell / bid value

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;
};

// Game status ----------------------------------------------------------------

struct LM_API_HUP PlayerSphereStatus : public Core::Object
{
    PlayerSphereStatus();

    int32       mana;
    int32       absorb;
    int32       chaining;
    int32       impair;
};

//struct LM_API_HUP OpponentSphereStatus : public Core::Object
//{
//    OpponentSphereStatus();
//
//    int32       mana;
//    int32       absorb;
//    int32       chaining;
//    int32       impair;
//};

struct LM_API_HUP SpellCooldown : public Core::Object, public Core::Serializable
{
    SpellCooldown();

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    int32       spellId;
    int32       cooldown;
};

struct LM_API_HUP PlayerStatus : public Core::Object
{
    PlayerStatus();

    int32               health;
    int32               willpower;
    int32               nextTurnsToSee;
    bool                hasVantage;

    PlayerSphereStatus  spheres[3];

    Core::List<SpellCooldown>   cooldowns;
};

struct LM_API_HUP OpponentStatus : public Core::Object
{
    OpponentStatus();

    int32               health;
    bool                hasVantage;

    PlayerSphereStatus  spheres[3];
};

struct LM_API_HUP GameStatus : public Core::Object
{
    GameStatus();

    PlayerStatus    player;
    OpponentStatus  opponent;
};

// Game updates ---------------------------------------------------------------

struct LM_API_HUP PlayerSphereStatusUpdate : public Core::Object, public Core::Serializable
{
public:
    PlayerSphereStatusUpdate();
    explicit PlayerSphereStatusUpdate(const PlayerSphereStatus & newStatus);
    explicit PlayerSphereStatusUpdate(const PlayerSphereStatus & oldStatus, const PlayerSphereStatus & newStatus);

    bool isNull() { return _flags == 0; }

    void setUpdateAsDiff(const PlayerSphereStatus & oldStatus, const PlayerSphereStatus & newStatus);
    void applyUpdate(PlayerSphereStatus & status) const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

protected:
    static const int32 UPDATE_FLAG_MANA            = 0x00000001;
    static const int32 UPDATE_FLAG_ABSORB          = 0x00000002;
    static const int32 UPDATE_FLAG_CHAINING        = 0x00000004;
    static const int32 UPDATE_FLAG_IMPAIR          = 0x00000008;

    uint8                   _flags;
    PlayerSphereStatus      _status;
};

//struct LM_API_HUP OpponentSphereStatusUpdate : public Core::Object, public Core::Serializable
//{
//public:
//    OpponentSphereStatusUpdate();
//    explicit OpponentSphereStatusUpdate(const OpponentSphereStatus & newStatus);
//    explicit OpponentSphereStatusUpdate(const OpponentSphereStatus & oldStatus, const OpponentSphereStatus & newStatus);
//
//    bool isNull() { return _flags == 0; }
//
//    void setUpdateAsDiff(const OpponentSphereStatus & oldStatus, const OpponentSphereStatus & newStatus);
//    void applyUpdate(OpponentSphereStatus & status) const;
//
//    virtual void read(Core::InputStream & input);
//    virtual void write(Core::OutputStream & output) const;
//
//protected:
//    static const int32 UPDATE_FLAG_MANA            = 0x00000001;
//    static const int32 UPDATE_FLAG_ABSORB          = 0x00000002;
//    static const int32 UPDATE_FLAG_CHAINING        = 0x00000004;
//    static const int32 UPDATE_FLAG_IMPAIR          = 0x00000008;
//
//    uint8                   _flags;
//    OpponentSphereStatus      _status;
//};

struct LM_API_HUP PlayerStatusUpdate : public Core::Object, public Core::Serializable
{
public:
    PlayerStatusUpdate();
    explicit PlayerStatusUpdate(const PlayerStatus & newStatus);
    explicit PlayerStatusUpdate(const PlayerStatus & oldStatus, const PlayerStatus & newStatus);

    bool isNull() { return _flags == 0; }

    void setUpdateAsDiff(const PlayerStatus & oldStatus, const PlayerStatus & newStatus);
    void applyUpdate(PlayerStatus & status) const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;
    
protected:
    static const int32 UPDATE_FLAG_HEALTH          = 0x00000001;
    static const int32 UPDATE_FLAG_WILLPOWER       = 0x00000002;
    static const int32 UPDATE_FLAG_SEE_NEXT_TURN   = 0x00000004;
    static const int32 UPDATE_FLAG_SPHERE_1        = 0x00000008;
    static const int32 UPDATE_FLAG_SPHERE_2        = 0x00000010;
    static const int32 UPDATE_FLAG_SPHERE_3        = 0x00000020;
    static const int32 UPDATE_FLAG_VANTAGE         = 0x00000040;

    uint8                       _flags;
    PlayerStatus                _status;
    PlayerSphereStatusUpdate    _spheres[3];
};

struct LM_API_HUP OpponentStatusUpdate : public Core::Object, public Core::Serializable
{
public:
    OpponentStatusUpdate();
    explicit OpponentStatusUpdate(const OpponentStatus & newStatus);
    explicit OpponentStatusUpdate(const OpponentStatus & oldStatus, const OpponentStatus & newStatus);

    bool isNull() { return _flags == 0; }

    void setUpdateAsDiff(const OpponentStatus & oldStatus, const OpponentStatus & newStatus);
    void applyUpdate(OpponentStatus & status) const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;
    
protected:
    static const int32 UPDATE_FLAG_OPPONENT_HEALTH     = 0x00000001;
    static const int32 UPDATE_FLAG_OPPONENT_VANTAGE    = 0x00000002;
    static const int32 UPDATE_FLAG_SPHERE_1            = 0x00000008;
    static const int32 UPDATE_FLAG_SPHERE_2            = 0x00000010;
    static const int32 UPDATE_FLAG_SPHERE_3            = 0x00000020;

    uint8                       _flags;
    OpponentStatus              _status;
    PlayerSphereStatusUpdate    _spheres[3];
};

struct LM_API_HUP GameStatusUpdate : public Core::Object, public Core::Serializable
{
    GameStatusUpdate();
    explicit GameStatusUpdate(const GameStatus & newStatus);
    explicit GameStatusUpdate(const GameStatus & oldStatus, const GameStatus & newStatus);

    bool isNull() { return _flags == 0; }

    void setUpdateAsDiff(const GameStatus & oldStatus, const GameStatus & newStatus);
    void applyUpdate(GameStatus & status) const;

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;
    
protected:
    static const int32 UPDATE_FLAG_PLAYER              = 0x00000001;
    static const int32 UPDATE_FLAG_OPPONENT            = 0x00000002;

    uint8                       _flags;
    PlayerStatusUpdate          _player;
    OpponentStatusUpdate        _opponent;
};


}//namespace HOOUserProtocol

#endif/*HOOUSERPROTOCOL_CHATMESSAGE_H_*/