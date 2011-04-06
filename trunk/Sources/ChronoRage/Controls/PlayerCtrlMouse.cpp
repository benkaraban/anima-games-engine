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
#include <Controls/PlayerCtrlMouse.h>
#include <Core/Math/GeometryComputations.h>
#include <Core/Math/GeometryIntersect.h>

namespace ChronoRage
{

const float DEFAULT_RADIAL_SPEED = 15.0f;
//-----------------------------------------------------------------------------
PlayerCtrlMouse::PlayerCtrlMouse(const Ptr<Window::InputManager> & pInput, const Ptr<Universe::NodeCamera> & pCamera, const Ptr<GameSettings> & pGameSettings) :
    _pInput(pInput),
    _pCamera(pCamera),
    _pGameSettings(pGameSettings),
    _dir(0.0f, 1.0f, 0.0f)
{
    setDefaultRadialSpeed();
}
//-----------------------------------------------------------------------------
void PlayerCtrlMouse::setDefaultRadialSpeed()
{
    setRadialSpeed(DEFAULT_RADIAL_SPEED);
}
//-----------------------------------------------------------------------------
Core::Vector3f PlayerCtrlMouse::getMoveDir() const
{
    Core::Vector3f direction(0.0f, 0.0f, 0.0f);

    int32 up;
    int32 down;
    int32 left;
    int32 right;
    _pGameSettings->getMovementKeyboardControls(up, down, left, right);

    if(_pInput->isKeyDown(up))
        direction.y = 1.0f;
    else if(_pInput->isKeyDown(down))
        direction.y = -1.0f;

    if(_pInput->isKeyDown(left))
        direction.x = -1.0f;
    else if(_pInput->isKeyDown(right))
        direction.x = 1.0f;

    if(!direction.isZero())
        direction.normalizeFast();

    return direction;
}
//-----------------------------------------------------------------------------
void PlayerCtrlMouse::update(double elapsed)
{
    Core::Vector3f keyDir(getShootTarget() - _playerPos);

    if(!keyDir.isZero())
    {
        keyDir.normalizeSafe();

        float angle = Core::L_acosSafe(Core::dot(_dir, keyDir));
        angle = std::min(angle, float(elapsed) * getRadialSpeed());

        Core::Vector3f zVec(Core::cross(_dir, keyDir));

        if(zVec.isZero())
            zVec = Core::Vector3f(0.0f, 0.0f, 1.0f);
        
        if(!Core::isEquivalent(angle, 0.0f))
        {
            zVec.normalizeSafe();
            Core::Matrix4f matR(Core::ROTATION, angle, zVec);
            _dir = matR.apply3x3(_dir);
            _dir.normalizeSafe();
        }
    }
}
//-----------------------------------------------------------------------------
Core::Vector3f PlayerCtrlMouse::getShootDir() const
{
    return _dir;
}
//-----------------------------------------------------------------------------
void PlayerCtrlMouse::setViewDimensions(int32 width, int32 height)
{
    _viewWidth = float(width);
    _viewHeight = float(height);
}
//-----------------------------------------------------------------------------
void PlayerCtrlMouse::setPlayerPos(const Core::Vector3f & pos)
{
    _playerPos = pos;
}
//-----------------------------------------------------------------------------
Core::Vector3f PlayerCtrlMouse::getShootTarget() const
{
    float mouseX = _pInput->getMouseXPos() / _viewWidth;
    float mouseY = _pInput->getMouseYPos() / _viewHeight;

    Core::Rayf mouseRay = _pCamera->getBaseCamera()->createViewVolume()->makeRayFrom2DPoint(Core::Vector2f(mouseX, mouseY));
    Core::Planef visiblePlane(Core::Plane_NormalPoint, Core::Vector3f(0.0f, 0.0f, 1.0f), _playerPos);
    Core::Vector3f target(0.0f, 0.0f, 0.0f);

    Core::intersect_RayPlane(mouseRay, visiblePlane, target);

    return target;
}
//-----------------------------------------------------------------------------
bool PlayerCtrlMouse::fire() const
{
    EMouseButton shoot;
    EMouseButton timeWarp;
    _pGameSettings->getShootingMouseControls(shoot, timeWarp);

    bool mouseDown = false;
    switch(shoot)
    {
    case MOUSE_LEFT_B:
        mouseDown = _pInput->isLMBDown();
        break;
    case MOUSE_RIGHT_B:
        mouseDown = _pInput->isRMBDown();
        break;
    case MOUSE_MIDDLE_B:
        mouseDown = _pInput->isMMBDown();
        break;
    }
    
    if(_pGameSettings->getMouseAutoFire())
        return !mouseDown;
    else
        return mouseDown;
}
//-----------------------------------------------------------------------------
bool PlayerCtrlMouse::timeWarp() const
{
    EMouseButton shoot;
    EMouseButton timeWarp;
    _pGameSettings->getShootingMouseControls(shoot, timeWarp);

    bool mouseDown = false;
    switch(timeWarp)
    {
    case MOUSE_LEFT_B:
        mouseDown = _pInput->isLMBDown();
        break;
    case MOUSE_RIGHT_B:
        mouseDown = _pInput->isRMBDown();
        break;
    case MOUSE_MIDDLE_B:
        mouseDown = _pInput->isMMBDown();
        break;
    }

    return mouseDown;
}
//-----------------------------------------------------------------------------
}
