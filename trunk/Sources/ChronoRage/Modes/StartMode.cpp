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
#include "StartMode.h"

#include <Core/TGA.h>

namespace ChronoRage
{

const Core::String MENUMODE_WORLD_FILE = L"/ChronoRageLevels/start-world.world";
Ptr<VoxelSpriteTemplate> StartMode::spTemplate01;
Ptr<VoxelSpriteTemplate> StartMode::spTemplate02;

const float TITLE_TIME_PRESENCE = 3.0f;
const float TITLE_TIME_PRESENCE_FAST = 0.5f;
const float FADE_TO_BLACK_TIMER = 1.0f;

StartMode::StartMode(AppContext & appContext)
:   IGameState(appContext),
    _state(ANIMA_UNINITIALIZED),
    _totalElapsed(0.0f)
{
}

void StartMode::initialize()
{    
    _totalElapsed = 0.0f;

    _appContext.pGUI->reset();
    _appContext.pGUI->setCursor(_appContext.pCursorBlank, 16, 16);

    Ptr<Core::InputStream> pStreamWorldFile = _appContext.pVFS->readFile(MENUMODE_WORLD_FILE);
    if(pStreamWorldFile == null)
    {
        Core::String message;
        message << L"Failed to load GameModeLevels World : "<<MENUMODE_WORLD_FILE;
        throw Core::Exception(message);
    }
    
    Ptr<Core::XMLDocument> pXmlWorld(new Core::XMLDocument());
    pXmlWorld->loadDocument(*pStreamWorldFile);
    
    _appContext.pWorld->importWorldXML(*pXmlWorld);

    _fogColor = _appContext.pWorld->getFogColor();
    
    if(!initCamera())
    {
        Core::String message;
        message << L"Failed to get camera in GameModeLevels World : "<<MENUMODE_WORLD_FILE;
        throw Core::Exception(message);
    }

    _pNodeChronoRage = _appContext.pWorld->createNode();
    _pNodeChronoRage->setUpVector(Core::Vector3f(0.0f, 0.0f, -1.0f));
    _pNodeChronoRage->setWorldDirection(Core::Vector3f(0.0f, 1.0f, 0.0f));
    _pNodeChronoRage->setWorldPosition(Core::Vector3f(0.0f, 0.0f, -35.0f));

    _pVoxelSprite01 = Ptr<VoxelSprite>(new VoxelSprite(_appContext.pWorld, getTemplate01(), Core::Vector4f::ONE, false));

    _pVoxelSprite01->setMatrices(_pNodeChronoRage->getLocalToWorldMatrix(), _pNodeChronoRage->getWorldToLocalMatrix());

    _pVoxelSprite02 = Ptr<VoxelSprite>(new VoxelSprite(_appContext.pWorld, getTemplate02(), Core::Vector4f::ONE, false));

    _pVoxelSprite01->setMatrices(_pNodeChronoRage->getLocalToWorldMatrix(), _pNodeChronoRage->getWorldToLocalMatrix());

    VoxelCloud cloud;
    cloud.position      = _pNodeChronoRage->getWorldToLocalMatrix().apply(Core::Vector3f(0.0f, 0.0f, 0.0f));
    cloud.cosAlpha      = Core::L_cos(f_PI);
    cloud.alpha         = f_PI;
    cloud.beta          = f_PI;
    cloud.dispRadius    = 50.0f;
    _pVoxelSprite01->cloudToSprite(cloud, 1.5f, 2.0f);
    _pVoxelSprite02->cloudToSprite(cloud, 1.5f, 2.0f);

    _state = ANIMA_CREATION_IN_PROGRESS;
}

Ptr<VoxelSpriteTemplate> StartMode::getTemplate01() const
{
    if(spTemplate01 == null)
    {
        Ptr<Core::InputStream> pInput01 = _appContext.pWorld->getRessourcesPool()->getVFS()->readFile(ANIMA_GAMES_FILE_01);
        Ptr<Core::Bitmap> pBitmap01 = Core::loadTGA(*pInput01);
        spTemplate01 = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap01, 0.2f, 0.05f));
    }

    return spTemplate01;
}

Ptr<VoxelSpriteTemplate> StartMode::getTemplate02() const
{
    if(spTemplate02 == null)
    {
        Ptr<Core::InputStream> pInput02 = _appContext.pWorld->getRessourcesPool()->getVFS()->readFile(ANIMA_GAMES_FILE_02);
        Ptr<Core::Bitmap> pBitmap02 = Core::loadTGA(*pInput02);
        spTemplate02 = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap02, 0.2f, 0.05f));
    }

    return spTemplate02;
}

void StartMode::update(double elapsed)
{
    _appContext.pWorld->update(elapsed);
    _appContext.pGUI->update(elapsed);
    _appContext.pSoundLibrary->update(elapsed);

    switch(_state)
    {
    case ANIMA_CREATION_IN_PROGRESS:
        updateCreation(elapsed);
        break;
    case ANIMA_ACTIVE:
        updateActive(elapsed);
        break;
    case ANIMA_DESTRUCTION_IN_PROGRESS:
        updateDestruction(elapsed);
        break;
    }

    _pVoxelSprite01->setMatrices(_pNodeChronoRage->getLocalToWorldMatrix(), _pNodeChronoRage->getWorldToLocalMatrix());
    _pVoxelSprite01->update(elapsed);

    _pVoxelSprite02->setMatrices(_pNodeChronoRage->getLocalToWorldMatrix(), _pNodeChronoRage->getWorldToLocalMatrix());
    _pVoxelSprite02->update(elapsed);

    if(_appContext.pInputManager->isKeyTyped(VK_ESCAPE))
        _appContext.fastTitle = true;
}

void StartMode::updateCreation(double elapsed)
{
    if(_pVoxelSprite01->isAnimationFinished() && _pVoxelSprite02->isAnimationFinished())
    {
        _state = ANIMA_ACTIVE;
    }
}

void StartMode::updateActive(double elapsed)
{
    _totalElapsed = std::min(_totalElapsed + float(elapsed), TITLE_TIME_PRESENCE);

    float titlePresence = TITLE_TIME_PRESENCE;
    if(_appContext.fastTitle)
        titlePresence = TITLE_TIME_PRESENCE_FAST;
    else
        titlePresence = TITLE_TIME_PRESENCE;

    if(_totalElapsed >= titlePresence)
    {
        VoxelExplosion explo;
        explo.dir           = Core::Vector3f(0.0f, 0.0f, 1.0f);
        explo.gravityHole   = Core::Vector3f(0.0f, 0.0f, 150.0f);
        explo.minSpeed      = 30.0f;
        explo.maxSpeed      = 60.0f;
        explo.clampSpeed    = 5000.0f;
        explo.dispAngle     = 1.5f;
        explo.selfRotSpeed  = 10.0f * f_PI;

        _pVoxelSprite01->explosion(explo);
        _pVoxelSprite02->explosion(explo);

        _state = ANIMA_DESTRUCTION_IN_PROGRESS;
        _totalElapsed = 0.0f;
    }
}

void StartMode::updateDestruction(double elapsed)
{
    if(_pVoxelSprite01->isAnimationFinished() && _pVoxelSprite02->isAnimationFinished())
    {
        _state = ANIMA_DELETE_PENDING;
        _appContext.wantedMode = MENU_MODE;
    }

    _totalElapsed += float(elapsed);

    Core::Vector4f fogColor;
    fogColor.setLerp(_fogColor, Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f), std::min(1.0f, _totalElapsed/FADE_TO_BLACK_TIMER));
    _appContext.pWorld->setFogColor(fogColor);
}

}//namespace ChronoRage