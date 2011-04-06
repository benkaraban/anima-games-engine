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
#include "FoeText.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>
#include <Core/Strings/Tools.h>

#include <Modes/GameMode.h>

namespace ChronoRage
{
Ptr<VoxelFont> FoeText::spVoxelCreditFont;
//
FoeText::FoeText(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode, const String & text)
:   FoeCommon(TEXT, pWorld, pPlayer, gameMode)
{
    if(spVoxelCreditFont == null)
    {
        VoxelFontInfos vfi;
        vfi.pBitmap = Core::loadTGA(*pWorld->getRessourcesPool()->getVFS()->readFile(L"/ChronoRageData/Font/credit-font.tga"));
        
        vfi.charWidth = 8;
        vfi.charHeight = 12;
        vfi.charSpacing = 8;

        vfi.uRes = 75;
        vfi.vRes = 1;

        vfi.characters = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789: ";

        vfi.cubeSize = 0.2f;
        vfi.padding = 0.05f;
        vfi.glowFactor = 0.1f;

        spVoxelCreditFont = Ptr<VoxelFont>(new VoxelFont(vfi));
    }

    gameMode.killAllFoes();

    _minRange = 5.0f;
    _maxRange = 15.0f;
    _speed = 5.0f;
    _radialSpeed = 1.0f;
    _maxHealthPoint = _healthPoint = 40.0f;
    _createShockWave = false;

    Core::List<String> lines;
    Core::tokenize(text, lines, L"\n");
    Core::Vector3f pos(0.0f, 0.0f, 0.0f);

    for(int32 iLine=0; iLine < lines.size(); iLine++)
    {
        pos.y = (lines.size() * 0.5f - float(iLine)) * spVoxelCreditFont->getCharHeight();

        for(int32 ii=0; ii < lines[iLine].size(); ii++)
        {
            if(lines[iLine][ii] != L' ')
            {
                pos.x = (ii - lines[iLine].size() * 0.5f) * spVoxelCreditFont->getCharSpacing();

                Ptr<FoeCharacter> pChar(new FoeCharacter(pWorld, pPlayer, gameMode, spVoxelCreditFont->getTemplate(lines[iLine][ii]), pos));

                _chars.push_back(pChar);
                gameMode.addFoe(pChar);
            }
        }
    }

    _pTemplate = spVoxelCreditFont->getTemplate(L' ');
}

void FoeText::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape());

    _approachPoint.x = Core::frand(_gameMode.getLevelXMin(), _gameMode.getLevelXMax());
    _approachPoint.y = Core::frand(_gameMode.getLevelYMin(), _gameMode.getLevelYMax());
    _approachPoint.z = _pPlayer->getPosition().z;

    for(int32 ii=0; ii < _chars.size(); ii++)
        _chars[ii]->initialize(position);
}

void FoeText::update(double elapsed)
{
    FoeCommon::update(elapsed);

    bool oneAlive = false;

    for(int32 ii=0; ii < _chars.size(); ii++)
    {
        _chars[ii]->setTextPos(getPosition());
        oneAlive = oneAlive || (_chars[ii]->getState() < FOE_DELETE_PENDING);
    }

    if(!oneAlive)
        _state = FOE_DELETE_PENDING;
}

void FoeText::updateCreation(double elapsed)
{
    if(_chars[0]->getState() == FOE_ACTIVE)
        _state = FOE_ACTIVE;
}

void FoeText::updateActive(double elapsed)
{
    Core::Vector3f target = _approachPoint;
    _gameMode.movePointInPlayground(target);

    Core::Vector3f distance = target - getPosition();
    
    if(!distance.isZero() && distance.getLengthFast() > _tolerance)
    {
        Core::Vector3f localTarget(_pNodeFoe->getParentWorldToLocalMatrix().apply(target));
        Core::Vector3f localDir(localTarget - _pNodeFoe->getLocalPosition());
        Core::Matrix3f mat;
        mat.setZVec(localDir.getNormalizedSafe(), _pNodeFoe->getUpVector(), _pNodeFoe->getLocalMatrix().getYVector());

        Core::Quaternionf q1(_pNodeFoe->getLocalOrient());
        Core::Quaternionf q2(mat);

        double rotTime = elapsed;
        _angleWithPlayer = 2.0f * Core::L_acosSafe(Core::L_abs(Core::dot(q1, q2)));
        LM_ASSERT(Core::isValid(_angleWithPlayer));
        float maxAngle = float(_radialSpeed * rotTime);
        float alpha = 1.0;

        if(_angleWithPlayer != 0.0)
            alpha = std::min(1.0f, maxAngle / _angleWithPlayer);

        Core::Quaternionf newOrient(q1, q2, alpha);

        _pNodeFoe->setLocalOrientation(newOrient);
    }
    else
    {
        _approachPoint.x = Core::frand(_gameMode.getLevelXMin(), _gameMode.getLevelXMax());
        _approachPoint.y = Core::frand(_gameMode.getLevelYMin(), _gameMode.getLevelYMax());
        _approachPoint.z = _pPlayer->getPosition().z;
    }

    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f direction = _pNodeFoe->getWorldDirection();

    Core::Vector3f newPosition = currentPosition + (direction * _speed * (float)elapsed);

    _pNodeFoe->setWorldPosition(newPosition);

    Foe::updateActive(elapsed);
}

FoeCharacter::FoeCharacter(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode, 
                           const Ptr<VoxelSpriteTemplate> & pTemplate,
                           const Core::Vector3f & deltaPos)
:   FoeCommon(TEXT, pWorld, pPlayer, gameMode),
    _pTemplate(pTemplate),
    _deltaPos(deltaPos)
{
    _createShockWave = true;

    _minRange = 0.0f;
    _maxRange = 0.0f;
    _speed = 0.0f;
    _radialSpeed = 0.0f;
    _maxHealthPoint = _healthPoint = 15.0f;
}

int32 FoeCharacter::foeCount() const
{
    return 0;
}

void FoeCharacter::setTextPos(const Core::Vector3f & pos)
{
    setPosition(pos + _deltaPos);
}

void FoeCharacter::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pNodeFoe->setWorldDirection(Core::Vector3f(0.0f, 1.0f, 0.0f));

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape());
    _pCollisionShape->addSphere(Core::Sphere(Core::Vector3f(0.0f,  0.5f, 0.0f), 0.75f));
    _pCollisionShape->addSphere(Core::Sphere(Core::Vector3f(0.0f, -0.5f, 0.0f), 0.75f));
}

}//namespace ChronoRage