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
#include "Wave.h"

#include <Modes/GameMode.h>
#include <Foes/Grunt.h>
#include <Foes/Kamikaze.h>
#include <Foes/Teleporter.h>
#include <Foes/QuickTeleporter.h>
#include <Foes/Turret.h>
#include <Foes/LaserTurret.h>
#include <Foes/Buckler.h>
#include <Foes/BonusCarrier.h>
#include <Foes/MineLayer.h>
#include <Foes/Cruiser.h>
#include <Foes/Wasp.h>
#include <Foes/Charger.h>
#include <Foes/Boss.h>
#include <Foes/UberBoss.h>
#include <Foes/FoeText.h>

#include <Core/Strings/Tools.h>
#include <Core/Math/Random.h>
#include <Core/XML/XMLMath.h>

namespace ChronoRage
{

Wave::Wave(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode, const Ptr<Level> & pLevel)
:   _pWorld(pWorld),
    _pPlayer(pPlayer),
    _gameMode(gameMode),
    _pLevel(pLevel),
    _waveId(0),
    _totalElapsed(0.0),
    _powerLevel(0),
    _duration(0)
{}

Wave::Wave(const Ptr<Wave> & wave)
:   _pWorld(wave->_pWorld),
    _pPlayer(wave->_pPlayer),
    _gameMode(wave->_gameMode),
    _pLevel(wave->_pLevel),
    _waveId(wave->_waveId),
    _totalElapsed(0.0),
    _formations(wave->_formations),
    _powerLevel(wave->_powerLevel),
    _duration(wave->_duration)
{
    for(int32 iFormation = 0;  iFormation < _formations.size(); ++iFormation)
    {
        if(_formations[iFormation].getType() == FORMATION_RANDOM)
        {
            Core::Vector2f position(0.0f, 0.0f);
            position.x = Core::frand(_pLevel->xMin(), _pLevel->xMax());
            position.y = Core::frand(_pLevel->yMin(), _pLevel->yMax());
            _formations[iFormation].setPosition(position);
        }
        else if(_formations[iFormation].getType() == FORMATION_PLAYER)
        {
            Core::Vector2f position;
            position.x = _pPlayer->getPosition().x;
            position.y = _pPlayer->getPosition().y;
            _formations[iFormation].setPosition(position);
        }
    }
}

Wave::~Wave()
{}

void Wave::update(double elapsed)
{
    _totalElapsed += elapsed;

    Core::List<Formation>::iterator iFormation = _formations.begin();

    Core::List<Ptr<FoeRequest> > foeRequests;
    while(iFormation != _formations.end())
    {
        foeRequests.clear();

        iFormation->getFoeRequest(foeRequests, (float)_totalElapsed);

        Core::List<Ptr<FoeRequest> >::iterator iFoeRequest = foeRequests.begin();
        while(iFoeRequest != foeRequests.end())
        {
            if(iFormation->playerPositionSet())
            {
                Core::Vector2f position;
                position.x = _pPlayer->getPosition().x;
                position.y = _pPlayer->getPosition().y;
                iFormation->setPosition(position);
            }
            createFoe(*iFoeRequest, *iFormation);

            ++iFoeRequest;
        }

        if(iFormation->empty())
            iFormation = _formations.erase(iFormation);
        else
            ++iFormation;
    }
}

void Wave::createFoe(const Ptr<FoeRequest> & pFoeRequest, const Formation & formation)
{
    Ptr<Foe> pFoe = null;

    switch(pFoeRequest->foeType)
    {
    case GRUNT:
        pFoe = Ptr<Foe>(new Grunt(_pWorld, _pPlayer, _gameMode));
        break;
    case KAMIKAZE:
        pFoe = Ptr<Foe>(new Kamikaze(_pWorld, _pPlayer, _gameMode));
        break;
    case TELEPORTER:
        pFoe = Ptr<Foe>(new Teleporter(_pWorld, _pPlayer, _gameMode));
        break;
    case WASP:
        pFoe = Ptr<Foe>(new Wasp(_pWorld, _pPlayer, _gameMode));
        break;
    case TURRET:
        pFoe = Ptr<Foe>(new Turret(_pWorld, _pPlayer, _gameMode));
        break;
#ifndef CHRONORAGE_DEMO
    case QUICK_TELEPORTER:
        pFoe = Ptr<Foe>(new QuickTeleporter(_pWorld, _pPlayer, _gameMode));
        break;
    case CHARGER:
        pFoe = Ptr<Foe>(new Charger(_pWorld, _pPlayer, _gameMode));
        break;
    case BUCKLER:
        pFoe = Ptr<Foe>(new BucklerCarrier(_pWorld, _pPlayer, _gameMode));
        break;
    case BONUS_CARRIER:
        pFoe = Ptr<Foe>(new BonusCarrier(_pWorld, _pPlayer, _gameMode, false));
        break;
    case BONUS_CARRIER_W_SHIELD:
        pFoe = Ptr<Foe>(new BonusCarrier(_pWorld, _pPlayer, _gameMode, true));
        break;
    case MINELAYER:
        pFoe = Ptr<Foe>(new Minelayer(_pWorld, _pPlayer, _gameMode));
        break;
    case CRUISER:
        pFoe = Ptr<Foe>(new Cruiser(_pWorld, _pPlayer, _gameMode));
        break;
    case LASER_TURRET:
        pFoe = Ptr<Foe>(new LaserTurret(_pWorld, _pPlayer, _gameMode));
        break;
    case BOSS:
        pFoe = Ptr<Foe>(new Boss(_pWorld, _pPlayer, _gameMode));
        break;
    case UBER_BOSS:
        pFoe = Ptr<Foe>(new UberBoss(_pWorld, _pPlayer, _gameMode));
        break;
    case TEXT:
        pFoe = Ptr<Foe>(new FoeText(_pWorld, _pPlayer, _gameMode, pFoeRequest->arg));
        break;
#endif
    }

    Core::Vector3f foePosition;
    Core::Vector3f formationPosition;
    formationPosition.x = formation.getPosition().x;
    formationPosition.y = formation.getPosition().y;
    formationPosition.z = 0.0f;

    foePosition.x = formationPosition.x + pFoeRequest->position.x;
    foePosition.y = formationPosition.y + pFoeRequest->position.y;
    foePosition.z = _pLevel->playgroundDepth();

    _gameMode.movePointInPlayground(foePosition);    

    pFoe->initialize(foePosition);
    
    _gameMode.addFoe(pFoe);
}

void Wave::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    Core::String strWaveId;
    pXMLNode->getAttribute(L"id", strWaveId);
    _waveId = Core::toInteger(strWaveId);

    Core::String strPowerLevel;
    pXMLNode->getAttribute(L"power-level", strPowerLevel);
    _powerLevel = Core::toInteger(strPowerLevel);

    _duration = 0.0f;
    Core::String strDuration;
    pXMLNode->getAttribute(L"duration", strDuration);
    _duration = float(Core::toFloat(strDuration));

    Core::List<Ptr<Core::XMLNode> > xmlFormations;
    pXMLNode->getNodes(L"formation", xmlFormations);

    Core::Map<String, EFoeType> foeDic;

    for(int32 iFoe=0; iFoe < EFoeType_COUNT; iFoe++)
        foeDic[ChronoRage::toString(EFoeType(iFoe))] = EFoeType(iFoe);

    Core::List<Ptr<Core::XMLNode> >::iterator iXMLFormation = xmlFormations.begin();

    while(iXMLFormation != xmlFormations.end())
    {
        Core::String strFormationType;
        EFormationType formationType = FORMATION_FIXED;

        Core::String strPosition;
        Core::Vector2f position(0.0f, 0.0f);

        (*iXMLFormation)->getAttribute(L"type", strFormationType);
        if(strFormationType == ChronoRage::toString(FORMATION_FIXED))
        {
            formationType = FORMATION_FIXED;
            (*iXMLFormation)->getAttribute(L"position", strPosition);
            Core::fromXMLString(strPosition, position);
        }
        else if(strFormationType == ChronoRage::toString(FORMATION_PLAYER))
        {
            formationType = FORMATION_PLAYER;
        }
        else if(strFormationType == ChronoRage::toString(FORMATION_RANDOM))
        {
            formationType = FORMATION_RANDOM;
            position.x = Core::frand(_pLevel->xMin(), _pLevel->xMax());
            position.y = Core::frand(_pLevel->yMin(), _pLevel->yMax());
        }

        Formation formation(formationType, position);

        Core::List<Ptr<Core::XMLNode> > xmlFoeRequests;

        (*iXMLFormation)->getNodes(L"foe-request", xmlFoeRequests);

        Core::List<Ptr<Core::XMLNode> >::iterator iFoeRequest = xmlFoeRequests.begin();
        while(iFoeRequest != xmlFoeRequests.end())
        {
            Core::String strFoeType;
            (*iFoeRequest)->getAttribute(L"type",strFoeType);

            EFoeType foeType = foeDic[strFoeType];
            
            Core::String strFoePosition;
            Core::Vector2f foePosition(0.0f, 0.0f);
            (*iFoeRequest)->getAttribute(L"position", strFoePosition);

            Core::fromXMLString(strFoePosition, foePosition);

            float timer = 0.0f;
            Core::String strTimer, arg;
            (*iFoeRequest)->getAttribute(L"timer", strTimer);
            (*iFoeRequest)->getAttribute(L"value", arg);
            timer = (float)Core::toFloat(strTimer);

            Core::replaceChar(arg, L'_', L' ');
            Core::replaceChar(arg, L'$', L'\n');

            formation.addFoeRequest(foeType, foePosition, arg, timer);

            ++iFoeRequest;
        }

        _formations.push_back(formation);

        ++iXMLFormation;
    }
}

void Wave::addFormation(const Formation & formation)
{
    _formations.push_back(formation);
}

}