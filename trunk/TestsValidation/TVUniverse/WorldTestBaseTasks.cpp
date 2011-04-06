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
#include <Core/Math/MathTools.h>

#include <Universe/Tasks/Fade.h>
#include <Universe/Tasks/LookAt.h>
#include <Universe/Tasks/Move.h>
#include <Universe/Tasks/MoveTrajectory.h>
#include <Universe/Tasks/Rotate.h>
#include <Universe/Tasks/Scale.h>
#include <Universe/Tasks/Sequence.h>
#include <Universe/Tasks/TurnAround.h>
#include <Universe/Tasks/PostFX.h>

#include "WorldTestBaseTasks.h"

#include <math.h>

#define TEST_2_WORLDS 0

const float X_START  = -200.0;
const float X_END    =  200.0;
const float X_MIDDLE =    0.0;

const float Y1 =  150.0;
const float Y2 =   50.0;
const float Y3 =  -50.0;
const float Y4 = -150.0;

const float DECAL_SIZE = 100.0;

using namespace Core;
using namespace Universe;

//-----------------------------------------------------------------------------
void WorldTestBaseTasks::initalise(const Ptr<Renderer::IRenderer> & pRenderer, 
                                   const Ptr<Core::VirtualFileSystem> & pVFS, 
                                   const Ptr<Window::InputManager> & pInput, 
                                   const Ptr<Audio::IDevice> & pDevice)
{
    _pRenderer = pRenderer;
    _pVFS = pVFS;
    _pWorld = Ptr<Universe::World>(new Universe::World(pRenderer, pVFS));
    _pInput = pInput;

    // Ciel

    _pWorld->setSkyTexture(L"/TUData/Textures/FullskiesSunset0024_1_S.tex");
    _pWorld->setSkyColor(Core::Vector4f(1.0, 1.0, 1.0, 0.0));
    _pWorld->setSkyHorizon(-0.25);

    // Fog

    _pWorld->setFogColor(Core::Vector4f(0.15f, 0.1f, 0.1f, 0.0f));
    _pWorld->setFogRange(75.0, 500.0);
    _pWorld->setFogSkyOpacity(0.0);

    // Decals

    _pDecal1 = _pWorld->createDecal(L"/TUData/Textures/decal00.tex", Renderer::DECAL_ADD);
    _pDecal2 = _pWorld->createDecal(L"/TUData/Textures/decal00.tex", Renderer::DECAL_ADD);
    _pDecal3 = _pWorld->createDecal(L"/TUData/Textures/decal00.tex", Renderer::DECAL_ADD);
    _pDecal4 = _pWorld->createDecal(L"/TUData/Textures/decal00.tex", Renderer::DECAL_ADD);

    _pDecal1->setSize(DECAL_SIZE);
    _pDecal2->setSize(DECAL_SIZE);
    _pDecal3->setSize(DECAL_SIZE);
    _pDecal4->setSize(DECAL_SIZE);

#if TEST_2_WORLDS
    _pWorld2 = Ptr<Universe::World>(new Universe::World(pRenderer, pVFS, _pWorld->getRessourcesPool()));
    _pWorld2->createMesh(L"/TUData/Models/teapot.mdl", L"Teapot01");
#endif

    // Teapot

    _pMesh1 = _pWorld->createMesh(L"/TUData/Models/teapot.mdl", L"Teapot01");
    _pMesh2 = _pWorld->createMesh(L"/TUData/Models/teapot.mdl", L"Teapot01");
    _pMesh3 = _pWorld->createMesh(L"/TUData/Models/teapot.mdl", L"Teapot01");
    _pMesh4 = _pWorld->createMesh(L"/TUData/Models/teapot.mdl", L"Teapot01");

    alignMeshes(X_START, Vector3f(0.0, 0.0, 1.0), Vector3f(1.0, 1.0, 1.0));

    // Caméra

    _camQuake.setPosition(Core::Vector3f(0.0, 0.0, -600.0));
    _pCamera = _pWorld->createPerspectiveCamera(f_PI * 0.25f, 1.0, 10.0, 10000.0);
}
//-----------------------------------------------------------------------------
void WorldTestBaseTasks::alignMeshes(float x, const Core::Vector3f & dir, const Core::Vector3f & scale)
{
    for(Core::List<Ptr<Universe::ITask> >::const_iterator ipTask = _tasks.begin(); ipTask != _tasks.end(); ++ipTask)
        (*ipTask)->kill();

    _tasks.clear();

    _pMesh1->setVisible(true);
    _pMesh2->setVisible(true);
    _pMesh3->setVisible(true);
    _pMesh4->setVisible(true);

    _pDecal1->setVisible(false);
    _pDecal2->setVisible(false);
    _pDecal3->setVisible(false);
    _pDecal4->setVisible(false);

    _pMesh1->beginMatrixUpdate();
    _pMesh1->setLocalOrientation(Core::Quaternionf(Core::IDENTITY));
    _pMesh1->endMatrixUpdate();

    _pMesh2->beginMatrixUpdate();
    _pMesh2->setLocalOrientation(Core::Quaternionf(Core::IDENTITY));
    _pMesh2->endMatrixUpdate();

    _pMesh3->beginMatrixUpdate();
    _pMesh3->setLocalOrientation(Core::Quaternionf(Core::IDENTITY));
    _pMesh3->endMatrixUpdate();

    _pMesh4->beginMatrixUpdate();
    _pMesh4->setLocalOrientation(Core::Quaternionf(Core::IDENTITY));
    _pMesh4->endMatrixUpdate();

    _pMesh1->beginMatrixUpdate();
    _pMesh1->setWorldPosition(Core::Vector3f(x, Y1, 0.0));
    _pMesh1->setWorldDirection(dir);
    _pMesh1->setLocalScale(scale);
    _pMesh1->endMatrixUpdate();

    _pMesh2->beginMatrixUpdate();
    _pMesh2->setWorldPosition(Core::Vector3f(x, Y2, 0.0));
    _pMesh2->setWorldDirection(dir);
    _pMesh2->setLocalScale(scale);
    _pMesh2->endMatrixUpdate();

    _pMesh3->beginMatrixUpdate();
    _pMesh3->setWorldPosition(Core::Vector3f(x, Y3, 0.0));
    _pMesh3->setWorldDirection(dir);
    _pMesh3->setLocalScale(scale);
    _pMesh3->endMatrixUpdate();

    _pMesh4->beginMatrixUpdate();
    _pMesh4->setWorldPosition(Core::Vector3f(x, Y4, 0.0));
    _pMesh4->setWorldDirection(dir);
    _pMesh4->setLocalScale(scale);
    _pMesh4->endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void WorldTestBaseTasks::alignDecals()
{
    for(Core::List<Ptr<Universe::ITask> >::const_iterator ipTask = _tasks.begin(); ipTask != _tasks.end(); ++ipTask)
        (*ipTask)->kill();

    _tasks.clear();

    _pDecal1->setVisible(true);
    _pDecal2->setVisible(true);
    _pDecal3->setVisible(true);
    _pDecal4->setVisible(true);

    _pMesh1->setVisible(false);
    _pMesh2->setVisible(false);
    _pMesh3->setVisible(false);
    _pMesh4->setVisible(false);

    _pDecal1->beginMatrixUpdate();
    _pDecal1->setWorldPosition(Core::Vector3f(X_START, Y1, 0.0));
    _pDecal1->endMatrixUpdate();

    _pDecal2->beginMatrixUpdate();
    _pDecal2->setWorldPosition(Core::Vector3f(X_END, Y1, 0.0));
    _pDecal2->endMatrixUpdate();

    _pDecal3->beginMatrixUpdate();
    _pDecal3->setWorldPosition(Core::Vector3f(X_START, Y4, 0.0));
    _pDecal3->endMatrixUpdate();

    _pDecal4->beginMatrixUpdate();
    _pDecal4->setWorldPosition(Core::Vector3f(X_END, Y4, 0.0));
    _pDecal4->endMatrixUpdate();
}
//------------------------------------------------------------------------------
void WorldTestBaseTasks::update(double elapsed)
{
    static const int32 TASK_COUNT = 5;
    Ptr<ITask> pTask[TASK_COUNT];

    if(_pInput->isKeyDown(VK_LCONTROL))
    {
        if(_pInput->isKeyTyped('1'))
        {
            pTask[0] = Ptr<ITask>(new TaskFXSaturate(_pCamera, 0.0f, 1.0f, 2.0f, INTER_LINEAR));
        }
        else if(_pInput->isKeyTyped('2'))
        {
            pTask[0] = Ptr<ITask>(new TaskFXFade(_pCamera, Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f), 0.5f, INTER_LINEAR));
        }
        else if(_pInput->isKeyTyped('3'))
        {
            pTask[0] = Ptr<ITask>(new TaskFXBlur(_pCamera, 1.0f, 0.0f, 2.0f, INTER_LINEAR));
        }
        else if(_pInput->isKeyTyped('4'))
        {
            pTask[0] = Ptr<ITask>(new TaskFXDirBlur(_pCamera, Core::Vector2f(1.0f, 0.0f), 1.0f, 0.0f, 2.0f, INTER_LINEAR));
        }
        else if(_pInput->isKeyTyped('5'))
        {
            pTask[0] = Ptr<ITask>(new TaskFXRadialBlur(_pCamera, Core::Vector2f(0.5f, 0.5f), 1.0f, 0.0f, 2.0f, INTER_LINEAR));
        }
        else if(_pInput->isKeyTyped('6'))
        {
            pTask[0] = Ptr<ITask>(new TaskFXRadialBlur(_pCamera, _pMesh1, 0.0f, 0.0f, 2.0f, INTER_LINEAR));
        }
    }
    else
    {
        if(_pInput->isKeyTyped('1')) //---------------------------------------- MOVE
        {
            alignMeshes(X_START, Vector3f(0.0, 0.0, 1.0), Vector3f(1.0, 1.0, 1.0));
            const float TIME = 3.0;
            const float SPEED = (X_END - X_START) / TIME;

            pTask[0] = Ptr<ITask>(new TaskMove(DURATION_DEPENDANT, _pMesh1, Vector3f(X_END, Y1, 0.0), TIME,  INTER_LINEAR));
            pTask[1] = Ptr<ITask>(new TaskMove(SPEED_DEPENDANT,    _pMesh2, Vector3f(X_END, Y2, 0.0), SPEED, INTER_FADE_IN));
            pTask[2] = Ptr<ITask>(new TaskMove(SPEED_DEPENDANT,    _pMesh3, Vector3f(X_END, Y3, 0.0), SPEED, INTER_FADE_OUT));
            pTask[3] = Ptr<ITask>(new TaskMove(DURATION_DEPENDANT, _pMesh4, Vector3f(X_END, Y4, 0.0), TIME,  INTER_FADE_IN_OUT));
        }
        else if(_pInput->isKeyTyped('2')) //--------------------------------- ROTATE
        {
            alignMeshes(0.0, Vector3f(0.0, 0.0, 1.0), Vector3f(1.0, 1.0, 1.0));
            const float ANGLE = f_PI_MUL_2;
            const float TIME = 3.0;
            const float SPEED = (ANGLE) / TIME;

            pTask[0] = Ptr<ITask>(new TaskRotate(DURATION_DEPENDANT, _pMesh1, 0.0, 0.0, ANGLE, TIME,  INTER_LINEAR, LOCAL_COORD));
            pTask[1] = Ptr<ITask>(new TaskRotate(DURATION_DEPENDANT, _pMesh2, 0.0, 0.0, ANGLE, TIME,  INTER_FADE_IN, LOCAL_COORD));
            pTask[2] = Ptr<ITask>(new TaskRotate(SPEED_DEPENDANT,    _pMesh3, 0.0, 0.0, ANGLE, SPEED, INTER_FADE_OUT, LOCAL_COORD));
            pTask[3] = Ptr<ITask>(new TaskRotate(SPEED_DEPENDANT,    _pMesh4, 0.0, 0.0, ANGLE, SPEED, INTER_FADE_IN_OUT, WORLD_COORD));
        }
        else if(_pInput->isKeyTyped('3')) //---------------------------- ROTATE LOOP
        {
            alignMeshes(0.0, Vector3f(0.0, 1.0, 0.0), Vector3f(1.0, 1.0, 1.0));

            pTask[0] = Ptr<ITask>(new TaskRotateLoop(_pMesh1, 1.0, 0.0, 0.0, 2.0, LOCAL_COORD));
            pTask[1] = Ptr<ITask>(new TaskRotateLoop(_pMesh2, 1.0, 0.0, 0.0, 3.0, LOCAL_COORD));
            pTask[2] = Ptr<ITask>(new TaskRotateLoop(_pMesh3, 1.0, 0.0, 0.0, 4.0, LOCAL_COORD));
            pTask[3] = Ptr<ITask>(new TaskRotateLoop(_pMesh4, 0.0, 0.0, 1.0, 5.0, WORLD_COORD));
        }
        else if(_pInput->isKeyTyped('4')) //-------------------------------- LOOK AT
        {
            alignMeshes(0.0, Vector3f(0.0, 0.0, 1.0), Vector3f(1.0, 1.0, 1.0));
            const float TIME = 3.0;
            const float SPEED = (f_PI) / TIME;

            pTask[0] = Ptr<ITask>(new TaskLookAt(DURATION_DEPENDANT, _pMesh1, Vector3f(0.0, Y1, -100.0), TIME,  INTER_LINEAR));
            pTask[1] = Ptr<ITask>(new TaskLookAt(SPEED_DEPENDANT,    _pMesh2, Vector3f(0.0, Y2, -100.0), SPEED, INTER_FADE_IN));
            pTask[2] = Ptr<ITask>(new TaskLookAt(SPEED_DEPENDANT,    _pMesh3, Vector3f(0.0, Y3, -100.0), SPEED, INTER_FADE_OUT));
            pTask[3] = Ptr<ITask>(new TaskLookAt(DURATION_DEPENDANT, _pMesh4, Vector3f(0.0, Y4, -100.0), TIME,  INTER_FADE_IN_OUT));
        }
        else if(_pInput->isKeyTyped('5')) //--------------------------- LOCK LOOK AT
        {
            alignMeshes(X_START, Vector3f(1.0, 0.0, 0.0), Vector3f(1.0, 1.0, 1.0));
            const float TIME = 6.0;

            _pMesh1->beginMatrixUpdate();
            _pMesh1->setWorldPosition(Core::Vector3f(X_END, Y1, 0.0));
            _pMesh1->setWorldDirection(Vector3f(-1.0, 0.0, 0.0));
            _pMesh1->endMatrixUpdate();

            pTask[0] = Ptr<ITask>(new TaskMove(DURATION_DEPENDANT, _pMesh1, Vector3f(X_END, Y4, 0.0), 0.5f * TIME,  INTER_LINEAR));
            pTask[1] = Ptr<ITask>(new TaskLockLookAt(_pMesh1, Vector3f(X_START, Y4, 0.0), TIME));

            pTask[2] = Ptr<ITask>(new TaskLockLookAt(_pMesh2, _pMesh1, TIME, 1.0));
            pTask[3] = Ptr<ITask>(new TaskLockLookAt(_pMesh3, _pMesh1, TIME, INFINITY));
            pTask[4] = Ptr<ITask>(new TaskLockLookAt(_pMesh4, _pMesh1, TIME, 0.5));
        }
        else if(_pInput->isKeyTyped('6')) //---------------------------------- SCALE
        {
            alignMeshes(0.0, Vector3f(0.0, 0.0, 1.0), Vector3f(0.5, 1.0, 1.0));
            const float TIME = 2.0;

            pTask[0] = Ptr<ITask>(new TaskScale(_pMesh1, Vector3f(2.0, 1.0, 1.0), TIME, true, INTER_LINEAR));
            pTask[1] = Ptr<ITask>(new TaskScale(_pMesh2, Vector3f(2.0, 1.0, 1.0), TIME, true, INTER_FADE_IN));
            pTask[2] = Ptr<ITask>(new TaskScale(_pMesh3, Vector3f(2.0, 1.0, 1.0), TIME, false, INTER_FADE_OUT));
            pTask[3] = Ptr<ITask>(new TaskScale(_pMesh4, Vector3f(2.0, 1.0, 1.0), TIME, false, INTER_FADE_IN_OUT));
        }
        else if(_pInput->isKeyTyped('7')) //----------------------------- SCALE LOOP
        {
            alignMeshes(0.0, Vector3f(-1.0, 0.0, 0.0), Vector3f(1.0, 1.0, 1.0));
            const float PERIOD = 1.0;

            pTask[0] = Ptr<ITask>(new TaskScaleLoop(_pMesh1, Vector3f(1.0, 1.0, 1.0), Vector3f(2.0, 1.0, 1.0), PERIOD, 1.0));
            pTask[1] = Ptr<ITask>(new TaskScaleLoop(_pMesh2, Vector3f(1.0, 1.0, 1.0), Vector3f(1.0, 2.0, 1.0), PERIOD, 2.0));
            pTask[2] = Ptr<ITask>(new TaskScaleLoop(_pMesh3, Vector3f(1.0, 1.0, 1.0), Vector3f(1.0, 1.0, 2.0), PERIOD, 3.0));
            pTask[3] = Ptr<ITask>(new TaskScaleLoop(_pMesh4, Vector3f(0.5, 0.5, 0.5), Vector3f(1.0, 1.0, 1.0), PERIOD, 4.0));
        }
        else if(_pInput->isKeyTyped('8')) //---------------------------- TURN AROUND
        {
            alignMeshes(0.0, Vector3f(-1.0, 0.0, 0.0), Vector3f(1.0, 1.0, 1.0));
            const float TIME = 3.0;

            _pMesh1->setVisible(true);
            _pMesh2->setVisible(true);
            _pMesh3->setVisible(true);
            _pMesh4->setVisible(false);

            _pMesh3->beginMatrixUpdate();
            _pMesh3->setWorldPosition(Core::Vector3f(X_START, Y3, 0.0));
            _pMesh3->endMatrixUpdate();

            const Vector3f START_POS(X_END, Y1, 0.0);
            const Vector3f END_POS(X_START + 100.0f, Y3 + 50.0f, -100.0f);
            const Vector3f PIVOT(0.0f, Y2, 0.0f);

            pTask[0] = Ptr<ITask>(new TaskTurnAround(_pMesh1, START_POS, END_POS, PIVOT, TIME, INTER_FADE_IN_OUT));
            pTask[1] = Ptr<ITask>(new TaskLockLookAt(_pMesh1, PIVOT, TIME));

            pTask[2] = Ptr<ITask>(new TaskTurnAroundLoop(_pMesh3, PIVOT, 0.5, 1.0, 0.0));
        }
        else if(_pInput->isKeyTyped('9')) //----------------------------------- FADE
        {
            alignDecals();
            const float TIME = 2.0;
            const Vector4f COLOR_START(0.5, 0.5, 1.0, 0.0);
            const Vector4f COLOR_END(0.5, 0.5, 1.0, 1.0);

            pTask[0] = Ptr<ITask>(new TaskFade(_pDecal1, COLOR_START, COLOR_END, TIME, INTER_LINEAR));
            pTask[1] = Ptr<ITask>(new TaskFade(_pDecal2, COLOR_START, COLOR_END, TIME, INTER_FADE_IN));
            pTask[2] = Ptr<ITask>(new TaskFade(_pDecal3, COLOR_START, COLOR_END, TIME, INTER_FADE_OUT));
            pTask[3] = Ptr<ITask>(new TaskFade(_pDecal4, COLOR_START, COLOR_END, TIME, INTER_FADE_IN_OUT));
        }
        else if(_pInput->isKeyTyped('0')) //------------------------------ FADE LOOP
        {
            alignDecals();
            const float PERIOD = 1.0;
            const Vector4f COLOR_START(0.5, 0.5, 1.0, 0.0);
            const Vector4f COLOR_END(0.5, 0.5, 1.0, 1.0);

            pTask[0] = Ptr<ITask>(new TaskFadeLoop(_pDecal1, COLOR_START, COLOR_END, PERIOD, 1.5));
            pTask[1] = Ptr<ITask>(new TaskFadeLoop(_pDecal2, COLOR_START, COLOR_END, PERIOD, 2.5));
            pTask[2] = Ptr<ITask>(new TaskFadeLoop(_pDecal3, COLOR_START, COLOR_END, PERIOD, 3.5));
            pTask[3] = Ptr<ITask>(new TaskFadeLoop(_pDecal4, COLOR_START, COLOR_END, PERIOD, 4.5));
        }
        else if(_pInput->isKeyTyped('P')) //------------------------ SEQUENCE
        {
            alignMeshes(X_START, Vector3f(-1.0, 0.0, 0.0), Vector3f(1.0, 1.0, 1.0));

            _pMesh2->setVisible(false);
            _pMesh3->setVisible(false);
            _pMesh4->setVisible(false);

            Core::List<Ptr<ITask> > tasks;
            const float TIME = 1.0;

            tasks.push_back(Ptr<ITask>(new TaskMove(DURATION_DEPENDANT, _pMesh1, Vector3f(X_START, Y1, 0.0), Vector3f(X_END,   Y1, 0.0), TIME, INTER_FADE_IN_OUT)));
            tasks.push_back(Ptr<ITask>(new TaskMove(DURATION_DEPENDANT, _pMesh1, Vector3f(X_END,   Y1, 0.0), Vector3f(X_END,   Y4, 0.0), TIME, INTER_FADE_IN_OUT)));
            tasks.push_back(Ptr<ITask>(new TaskMove(DURATION_DEPENDANT, _pMesh1, Vector3f(X_END,   Y4, 0.0), Vector3f(X_START, Y4, 0.0), TIME, INTER_FADE_IN_OUT)));
            tasks.push_back(Ptr<ITask>(new TaskMove(DURATION_DEPENDANT, _pMesh1, Vector3f(X_START, Y4, 0.0), Vector3f(X_START, Y1, 0.0), TIME, INTER_FADE_IN_OUT)));

            pTask[0] = Ptr<ITask>(new TaskSequence(_pWorld, tasks));
        }
        else if(_pInput->isKeyTyped('O')) //---------------------- TRAJECTORY
        {
            alignMeshes(X_START, Vector3f(-1.0, 0.0, 0.0), Vector3f(1.0, 1.0, 1.0));

            _pMesh2->setVisible(false);
            _pMesh3->setVisible(false);
            _pMesh4->setVisible(false);

            List<CheckPoint> cp;

            cp.push_back(CheckPoint(Vector3f(X_START, Y1, 0.0), Quaternionf(IDENTITY),          0.0));
            cp.push_back(CheckPoint(Vector3f(X_START, Y2, 0.0), Quaternionf(YAW,   f_PI_DIV_2), 2.0));
            cp.push_back(CheckPoint(Vector3f(X_END,   Y2, 0.0), Quaternionf(ROLL,  f_PI_DIV_2), 4.0));
            cp.push_back(CheckPoint(Vector3f(X_START, Y4, 0.0), Quaternionf(PITCH, f_PI_DIV_2), 6.0));
            cp.push_back(CheckPoint(Vector3f(X_END,   Y4, 0.0), Quaternionf(IDENTITY),          8.0));

            pTask[0] = Ptr<ITask>(new TaskMoveTrajectory(_pMesh1, cp, 2.0));
        }
    }

    for(int32 ii=0; ii < TASK_COUNT; ii++)
        if(pTask[ii] != null)
            addTask(pTask[ii]);

    IWorldTest::update(elapsed);
}
//-----------------------------------------------------------------------------
void WorldTestBaseTasks::addTask(const Ptr<Universe::ITask> & pTask)
{
    _tasks.push_back(pTask);
    _pWorld->registerTask(pTask);
}
//-----------------------------------------------------------------------------
