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
#ifndef CHRONORAGE_STARTMODE_H_
#define CHRONORAGE_STARTMODE_H_


#include <Modes/IGameState.h>

namespace ChronoRage
{

LM_ENUM_5(EAnimaState,
          ANIMA_UNINITIALIZED,
          ANIMA_CREATION_IN_PROGRESS,
          ANIMA_ACTIVE,
          ANIMA_DESTRUCTION_IN_PROGRESS,
          ANIMA_DELETE_PENDING);

const Core::String ANIMA_GAMES_FILE_01 = L"/ChronoRageData/voxels-anima-games-01.tga";
const Core::String ANIMA_GAMES_FILE_02 = L"/ChronoRageData/voxels-anima-games-02.tga";

class LM_API_CHR StartMode : public IGameState
{
public:
    StartMode(AppContext & appContext);

    virtual ~StartMode()
    {}

    inline virtual EGameStateMode getMode() const { return START_MODE; }

    virtual void initialize();
    
    virtual void update(double elapsed);

#if LM_DISPLAY_FPS
    virtual int32 getFoesNumber(){ return 0;  }
    virtual float getTimeWarpNumber(){ return 0.0f;  }
    virtual float getPowerLevel(){ return 0.0f;  }


    virtual void powerUp(){}
    virtual void powerDown(){}
    virtual void createFoe(int32 foeId){}
#endif

protected:
    virtual void updateCreation(double elapsed);
    virtual void updateActive(double elapsed);
    virtual void updateDestruction(double elapsed);

protected:
    virtual Ptr<VoxelSpriteTemplate> getTemplate01() const;
    virtual Ptr<VoxelSpriteTemplate> getTemplate02() const;
    static Ptr<VoxelSpriteTemplate> spTemplate01;
    static Ptr<VoxelSpriteTemplate> spTemplate02;

protected:
    EAnimaState         _state;

    Ptr<Universe::Node> _pNodeChronoRage;

    Ptr<VoxelSprite>    _pVoxelSprite01;
    Ptr<VoxelSprite>    _pVoxelSprite02;

    float               _totalElapsed;
    Core::Vector4f      _fogColor;
};


}//namespace ChronoRage


#endif/*CHRONORAGE_STARTMODE_H_*/