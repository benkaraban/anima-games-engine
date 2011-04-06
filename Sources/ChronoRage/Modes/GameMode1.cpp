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
#include "GameMode1.h"

#include <Foes/Grunt.h>
#include <Foes/Kamikaze.h>
#include <Foes/Teleporter.h>
#include <Foes/QuickTeleporter.h>
#include <Foes/Turret.h>
#include <Foes/LaserTurret.h>
#include <Foes/Buckler.h>
#include <Foes/Cruiser.h>
#include <Foes/Wasp.h>
#include <Foes/Charger.h>
#include <Foes/Boss.h>
#include <Foes/BonusCarrier.h>
#include <Foes/FoeText.h>

#include <Core/Math/Random.h>
#include <Assets/Tool.h>

#define USE_VORTEX 1

namespace ChronoRage
{

const Core::String GAMEMODE1_WORLD_FILE = L"/ChronoRageLevels/world01.world";

/*const String PLAN_MESH_NAME = L"$MoveToolDocMaterialPlanMesh";
const String PLAN_MODEL_NAME = L"$MoveToolDocMaterialPlan";
const float BACKGROUND_PLAN_WIDTH = 170.0f;
const float BACKGROUND_PLAN_HEIGHT = 120.0f;

void createBackGroundPlanModel(const Ptr<Universe::RessourcePool>& pPool)
{
    //plan
    Ptr<Assets::ModelMesh> plan (new Assets::ModelMesh());
    Assets::VerticeSet & vsetPlan = plan->getLODByID(0);
    Assets::createPlane(vsetPlan, BACKGROUND_PLAN_WIDTH, BACKGROUND_PLAN_HEIGHT, 2, 2);
    plan->cleanup();

    Ptr<Assets::Model> pPlanModel (new Assets::Model());
    pPlanModel->addMesh(PLAN_MESH_NAME, *plan);

    pPool->addModel(PLAN_MODEL_NAME, pPlanModel);
}
*/

void GameMode1::initialize()
{
    GameMode::initialize();

    /*_pWorld = Ptr<Universe::World>(new Universe::World(pRenderer, pVFS, null, _pAudioDevice));

    createBackGroundPlanModel(_pWorld->getRessourcesPool());

    _pCamera = _pWorld->createPerspectiveCamera(f_PI_DIV_4, 1.0, 1.0, 200.0);
    _pCamera->setWorldPosition(Core::Vector3f(0.0f, 0.0f, -65.0f));
    _pCamera->setWorldDirection(Core::Vector3f(0.0f, 0.0f, 1.0f));

    Ptr<Universe::NodeMesh> pNodeBackgroundMesh = _pWorld->createMesh(PLAN_MODEL_NAME, PLAN_MESH_NAME);
    pNodeBackgroundMesh->setDiffuseTexture(L"/ChronoRageLevels/space01.tex");
    pNodeBackgroundMesh->setWorldPosition(Core::Vector3f(0.0f, 0.0f, 40.0f));
    pNodeBackgroundMesh->localPitch(-f_PI_DIV_2);
    Assets::Material material;
    pNodeBackgroundMesh->getMaterial(material);
    material.specular = Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    pNodeBackgroundMesh->setMaterial(material);
    
    
    _pLevel = Ptr<Level>(new Level(BACKGROUND_PLAN_WIDTH, BACKGROUND_PLAN_HEIGHT, pNodeBackgroundMesh->getWorldPosition().z));

    _pPlayer = Ptr<Player>(new Player(pInputManager, _pWorld, _pLevel));
    _pPlayer->initialize();*/

    Ptr<Core::InputStream> pStreamWorldFile = _appContext.pVFS->readFile(GAMEMODE1_WORLD_FILE);
    if(pStreamWorldFile == null)
    {
        Core::String message;
        message << L"Failed to load GameMode1 World : "<<GAMEMODE1_WORLD_FILE;
        throw Core::Exception(message);
    }
    
    Ptr<Core::XMLDocument> pXmlWorld(new Core::XMLDocument());
    pXmlWorld->loadDocument(*pStreamWorldFile);
    
    _appContext.pWorld->importWorldXML(*pXmlWorld);
    
    if(!initCamera())
    {
        Core::String message;
        message << L"Failed to get camera in GameMode1 World : "<<GAMEMODE1_WORLD_FILE;
        throw Core::Exception(message);
    }

    _pLevel = Ptr<Level>(new Level(150, 100, 10, -1));
    initPlayer(true);

    _pTimeWarp = Ptr<TimeWarp>(new TimeWarp(_appContext.pWorld, _appContext.pCameraNode, _pPlayer));

    for(int32 ii=0; ii<HELP_FIND_REMAINING_FOES; ++ii)
    {
        _foeDirections[ii].initialize(_appContext.pWorld);
    }

    VortexSettings settings;
    settings.cubeSize       = 2.0;
    settings.cubeCount      = 2500;
    settings.cubeGlow       = 0.8f;

    settings.armCount        = 6;
    settings.armRelativeSize = 1.0f;

    settings.gaussianDensity = true;

    settings.color0          = Core::Vector4f(0.2f, 0.0f, 0.0f, 1.0f);
    settings.color1          = Core::Vector4f(0.0f, 0.1f, 0.3f, 1.0f),
    settings.colorByDepth    = true;

    settings.cubeSelfRotationSpeed = 0.2f;

    settings.start          = 10.0f;
    settings.startWidth     = 90.0f;

    settings.nearU          = 0.0f;
    settings.sharpness      = 1.0f;
    settings.sharpness2     = 1.0f;

    settings.near           = 20.0f;
    settings.nearMinWidth   = 100.0f;
    settings.nearMaxWidth   = 110.0f;

    // good
    //settings.near           = 20.0f;
    //settings.nearMinWidth   = 100.0f;
    //settings.nearMaxWidth   = 100.0f;

    settings.far            = 150.0f;
    settings.farMinWidth    = 10.0f;
    settings.farMaxWidth    = 10.0f;

    settings.spiralCount    = 1.0f;//5.0f;

    settings.suckingSpeed   = 0.01f;

#if USE_VORTEX
    _pVortex = Ptr<Vortex>(new Vortex(_appContext.pWorld, settings, _pTimeWarp));
#endif

    settings.cubeSize       = 2.0;
    settings.cubeCount      = 2500;
    settings.cubeGlow       = 0.5f;

    settings.armCount        = 1;
    settings.armRelativeSize = 1.0f;

    settings.gaussianDensity = false;
    settings.cubeSelfRotationSpeed = 0.05f;

    settings.color0          = Core::Vector4f(0.1f, 0.0f, 0.0f, 1.0f);
    settings.color1          = Core::Vector4f(0.0f, 0.05f, 0.1f, 1.0f);
    settings.colorByDepth    = false;

    settings.start          = 10.0f;
    settings.startWidth     = 90.0f;

    settings.nearU          = 0.0f;
    settings.sharpness      = 1.0f;
    settings.sharpness2     = 1.0f;

    settings.near           = 50.0f;
    settings.nearMinWidth   = 90.0f;
    settings.nearMaxWidth   = 120.0f;

    settings.far            = 130.0f;
    settings.farMinWidth    = 30.0f;
    settings.farMaxWidth    = 80.0f;

    settings.spiralCount    = 0.0f;//5.0f;

    settings.suckingSpeed   = 0.00f;

#if USE_VORTEX
    _pVortex2 = Ptr<Vortex>(new Vortex(_appContext.pWorld, settings, _pTimeWarp));
    _pVortex2->update(0.0);
#endif


}

void GameMode1::update(double elapsed)
{
    static int32 modulo = 1;
    GameMode::update(elapsed);

#if USE_VORTEX
    _pVortex->startParallelUpdate(elapsed);
    //_pVortex2->update(elapsed);
#endif

    if(_pPlayer != null && _pPlayer->isInitialized() && _pLevel != null)
    {
        if(_pPlayer->isCannonReady(elapsed) && _pPlayer->isShooting() && !_pPlayer->isDead())
        {
            _pPlayer->fire();
        }

        manageTimeWarp(elapsed);
        
        _pPlayer->killDeadShots();

        _createWave -= (float)elapsed;

        /*if(_createWave <= 0)
        {
            Core::Vector3f foePosition = _pLevel->getFoePosition();
            foePosition.z = _pLevel->playgoundDepth();

            float angle = Core::frand() * f_PI_MUL_2;

            Core::Matrix4f rotationMatrix(Core::ROTATION, angle, Core::Vector3f(0.0f, 0.0f, 1.0f));

            foePosition = rotationMatrix.apply(foePosition);
            for(int32 iFoe=0; iFoe<50; ++iFoe)
            {

                Ptr<Foe> foe = Ptr<Foe>(new Invador(_pWorld, _pPlayer, *this));
                foe->initialize(foePosition);
                _foes.push_back(foe);
            }
            _createWave = 5.0f;
        }*/
        
        /*if(_createWave <= 0)
        {
            Ptr<Foe> foe = null;
            if(modulo == 0)
            {
                foe = Ptr<Foe>(new Grunt(_pWorld, _pPlayer, *this));
                modulo = 1;
            }
            else if(modulo == 1)
            {
                foe = Ptr<Foe>(new Kamikaze(_pWorld, _pPlayer, *this));
                modulo = 2;
            }
            else if(modulo == 2)
            {
                foe = Ptr<Foe>(new Teleporter(_pWorld, _pPlayer, *this));
                modulo = 3;
            }
            else if(modulo == 3)
            {
                foe = Ptr<Foe>(new Turret(_pWorld, _pPlayer, *this));
                modulo = 0;
            }

            Core::Vector3f foePosition(0.95f * Core::frand(_pLevel->xMin(), _pLevel->xMax()),
                                       0.95f * Core::frand(_pLevel->yMin(), _pLevel->yMax()),
                                       _pLevel->playgoundDepth());

            foe->initialize(foePosition);
            _foes.push_back(foe);

            _createWave = 1.0f;
        }*/ 

        /*if(_foes.size() == 0)
        {
            Ptr<Foe> foe = Ptr<Foe>(new Boss(_pWorld, _pPlayer, *this));

            Core::Vector3f foePosition(Core::frand(_pLevel->xMin(), _pLevel->xMax()),
                                       Core::frand(_pLevel->yMin(), _pLevel->yMax()),
                                       _pLevel->playgroundDepth());

            movePointInPlayground(foePosition);

            foe->initialize(foePosition);
            _foes.push_back(foe);

        }*/

        Core::List<Ptr<Wave> >::iterator iWave = _waves.begin();
        while(iWave != _waves.end())
        {
            (*iWave)->update(elapsed);
            if((*iWave)->finished())
                iWave = _waves.erase(iWave);
            else
                ++iWave;
        }
    }

    _pVortex->endParallelUpdate();
}

void GameMode1::createFoe(int32 foeId)
{
    Ptr<Foe> foe = null;

    switch(foeId)
    {
    case 0:
        //foe = Ptr<Foe>(new Grunt(_appContext.pWorld, _pPlayer, *this));
        //foe = Ptr<Foe>(new FoeText(_appContext.pWorld, _pPlayer, *this, L"BENJAMIN KARABAN\nJEREMIE COMARMOND"));
        foe = Ptr<Foe>(new QuickTeleporter(_appContext.pWorld, _pPlayer, *this));
        break;
    case 1:
        foe = Ptr<Foe>(new LaserTurret(_appContext.pWorld, _pPlayer, *this));
        //foe = Ptr<Foe>(new Kamikaze(_appContext.pWorld, _pPlayer, *this));
        break;
    case 2:
        foe = Ptr<Foe>(new Teleporter(_appContext.pWorld, _pPlayer, *this));
        break;
    case 3:
        foe = Ptr<Foe>(new BucklerCarrier(_appContext.pWorld, _pPlayer, *this));
        break;
    case 4:
        foe = Ptr<Foe>(new Cruiser(_appContext.pWorld, _pPlayer, *this));
        break;
    case 5:
        foe = Ptr<Foe>(new Wasp(_appContext.pWorld, _pPlayer, *this));
        break;
    case 6:
        foe = Ptr<Foe>(new Charger(_appContext.pWorld, _pPlayer, *this));
        break;
    case 7:
        foe = Ptr<Foe>(new Turret(_appContext.pWorld, _pPlayer, *this));
        break;
    case 8:
        foe = Ptr<Foe>(new Boss(_appContext.pWorld, _pPlayer, *this));
        break;
    case 9:
        foe = Ptr<Foe>(new BonusCarrier(_appContext.pWorld, _pPlayer, *this, true));
        break;
    }

    Core::Vector3f foePosition(Core::frand(_pLevel->xMin(), _pLevel->xMax()),
                               Core::frand(_pLevel->yMin(), _pLevel->yMax()),
                               _pLevel->playgroundDepth());

    movePointInPlayground(foePosition);

    foe->initialize(foePosition);
    _foes.push_back(foe);
}

void GameMode1::displayPowerUp()
{}

void GameMode1::displayPowerDown()
{}

void GameMode1::displayTimeWarpBonus()
{}

void GameMode1::deathPenalty()
{}

void GameMode1::loadWave()
{
    Core::XMLDocument xmlDocument;
    xmlDocument.loadDocument(*(_appContext.pWorld->getRessourcesPool()->getVFS()->readFile(L"/ChronoRageLevels/Waves/wave-test.xml")));

    Ptr<Core::XMLNode> pNodeWaves = xmlDocument.firstChildNode(L"waves");

    Core::List<Ptr<Core::XMLNode> > pXMLWaveList;
    pNodeWaves->getNodes(L"wave", pXMLWaveList);

    for(int32 ii=0; ii<pXMLWaveList.size(); ++ii)
    {
        Ptr<Wave> pWave(new Wave(_appContext.pWorld, _pPlayer, *this, _pLevel));
        pWave->importXML(pXMLWaveList[ii]);
        _waves.push_back(pWave);
        
    }
}

}//namespace ChronoRage