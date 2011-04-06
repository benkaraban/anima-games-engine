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
#include "NavigationStyle.h"

#include <Universe/NodeCamera.h>

#include <QMouseEvent>
#include <QKeyEvent>

#include <Core/Math/MathTools.h>
#include <Core/Math/Transform.h>
#include <Universe/NodeCamera.h>
#include <Renderer/Common/Camera.h>
#include <EPI/GUI/WorldOverlay/NodeCameraEdition.h>

#include <EPI/Document/Property.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NavigationStyleQuake::NavigationStyleQuake():
    INavigationStyle(),
    _sensibilityRot(f_PI/1000),
    _moveSpeed(0.05f),
    _isPressed(false),
    _rightIsPressed(false),
    _slowSpeedFactor(1),
    _normalSpeedFactor(10),
    _extraSpeedFactor(40),
    _tdForResetRoll(0,0,0,180000),
    _lastRollRight (Core::Clock::universalTime()),
    _lastRollLeft (Core::Clock::universalTime())
{
    for (int32 ii=0; ii<ECameraQuakeMove_COUNT; ++ii)
    {
        _moveActivate[ii] = false;
    }

    _ctrl.setSlowSpeedFactor(_slowSpeedFactor);
    _ctrl.setNormalSpeedFactor(_normalSpeedFactor);
    _ctrl.setExtraSpeedFactor(_extraSpeedFactor);
}
//-----------------------------------------------------------------------------
NavigationStyleQuake::~NavigationStyleQuake()
{
}
//-----------------------------------------------------------------------------
int32 KEY_QUAKE_FORWARD              = Qt::Key_Z;
int32 KEY_QUAKE_BACKWARD             = Qt::Key_S;
int32 KEY_QUAKE_STRAFE_LEFT          = Qt::Key_Q;
int32 KEY_QUAKE_STRAFE_RIGHT         = Qt::Key_D;
int32 KEY_QUAKE_DOWN                 = Qt::Key_A;
int32 KEY_QUAKE_UP                   = Qt::Key_E;
int32 KEY_QUAKE_EXTRA_SPEED_FACTOR   = Qt::Key_Space;
int32 KEY_QUAKE_SLOW_ENGAGE          = Qt::Key_twosuperior;
int32 KEY_QUAKE_ORBIT                = Qt::Key_Alt;
int32 KEY_QUAKE_ROLL_RIGHT           = Qt::Key_R;
int32 KEY_QUAKE_ROLL_LEFT            = Qt::Key_F;
//-----------------------------------------------------------------------------
void NavigationStyleQuake::keyPressEvent(QKeyEvent* event,   const DrawContext& dc)
{
    if (event->key() == KEY_QUAKE_EXTRA_SPEED_FACTOR)
    {
        _ctrl.setSpeedMode(EXTRA_SPEED_MODE);
        event->accept();
    }

    if (event->key() == KEY_QUAKE_SLOW_ENGAGE)
    {
        _ctrl.engageSlow();
        event->accept();
    }

    if (event->key() == KEY_QUAKE_ORBIT)
    {
        _ctrl.setModeOrbit(true);
        event->accept();
    }

    if (event->key() == KEY_QUAKE_FORWARD)
    {
        _moveActivate[QUAKE_FORWARD] = true;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_BACKWARD)
    {
        _moveActivate[QUAKE_BACKWARD] = true;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_STRAFE_LEFT)
    {
        _moveActivate[QUAKE_STRAFE_LEFT] = true;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_STRAFE_RIGHT)
    {
        _moveActivate[QUAKE_STRAFE_RIGHT] = true;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_DOWN)
    {
        _moveActivate[QUAKE_DOWN] = true;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_UP)
    {
        _moveActivate[QUAKE_UP] = true;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_ROLL_RIGHT)
    {
        _moveActivate[QUAKE_ROLL_RIGHT] = true;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_ROLL_LEFT)
    {
        _moveActivate[QUAKE_ROLL_LEFT] = true;
        event->accept();
    }
}
//----------------------------------------------------------------------------
void NavigationStyleQuake::keyReleaseEvent(QKeyEvent* event,   const DrawContext& dc)
{
    if (event->key() == KEY_QUAKE_EXTRA_SPEED_FACTOR)
    {
        _ctrl.setSpeedMode(NORMAL_MODE);
        event->accept();
    }
    
    if (event->key() == KEY_QUAKE_FORWARD)
    {
        _moveActivate[QUAKE_FORWARD] = false;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_ORBIT)
    {
        _ctrl.setModeOrbit(false);
        event->accept();
    }

    if (event->key() == KEY_QUAKE_BACKWARD)
    {
        _moveActivate[QUAKE_BACKWARD] = false;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_STRAFE_LEFT)
    {
        _moveActivate[QUAKE_STRAFE_LEFT] = false;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_STRAFE_RIGHT)
    {
        _moveActivate[QUAKE_STRAFE_RIGHT] = false;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_DOWN)
    {
        _moveActivate[QUAKE_DOWN] = false;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_UP)
    {
        _moveActivate[QUAKE_UP] = false;
        event->accept();
    }

    if (event->key() == KEY_QUAKE_ROLL_RIGHT)
    {
        _moveActivate[QUAKE_ROLL_RIGHT] = false;
        
        if (event->isAutoRepeat() == false)
        {
            Core::TimeDuration  tdDelta = Core::Clock::universalTime() - _lastRollRight;
            if (tdDelta < _tdForResetRoll)
            {
                resetRoll();
            }
            _lastRollRight = Core::Clock::universalTime();
        }

        

        event->accept();
    }

    if (event->key() == KEY_QUAKE_ROLL_LEFT)
    {
        _moveActivate[QUAKE_ROLL_LEFT] = false;


        if (event->isAutoRepeat() == false)
        {
            Core::TimeDuration  tdDelta = Core::Clock::universalTime() - _lastRollLeft;
            if (tdDelta < _tdForResetRoll)
            {
                resetRoll();
            }
            _lastRollLeft = Core::Clock::universalTime();
        }

        
        

        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleQuake::mousePressEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (event->button() & Qt::LeftButton)
    {
        _isPressed = true;
        _lastPosition = Core::Vector2f(event->globalPos().x(), event->globalPos().y());
        event->accept();
    }
    else if (event->button() & Qt::MidButton)
    {
        _isPressed = true;
        _lastPosition = Core::Vector2f(event->globalPos().x(), event->globalPos().y());
        event->accept();
    }
    else if (event->button() & Qt::RightButton)
    {
        bool isInAction = false;
        for (int32 ii=0; ii<ECameraQuakeMove_COUNT; ++ii)
        {
            if (_moveActivate[ii] == true)
            {
                isInAction = true;
                break;
            }
        }

        if (isInAction==true)   _rightIsPressed = true;
        else                    _rightIsPressed = false;

        if (_rightIsPressed == true)   _ctrl.setSpeedMode(SLOW_MODE);
        else                           _ctrl.setSpeedMode(NORMAL_MODE);
    }

}
//-----------------------------------------------------------------------------
void NavigationStyleQuake::mouseMoveEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (_isPressed==true)
    {
         Core::Vector2f delta = Core::Vector2f(event->globalPos().x(), event->globalPos().y()) - _lastPosition;

        if (event->buttons() & Qt::LeftButton)
        {
            _ctrl.turnLeftRight(delta.x * _sensibilityRot);
            _ctrl.turnUpDown(delta.y * _sensibilityRot);

            event->accept();
        }
        else if(event->buttons() & Qt::MidButton)
        {
            _ctrl.strafeLeftRight(-1*delta.x*_moveSpeed/2);
            _ctrl.moveUpDown(delta.y*_moveSpeed/2);
            event->accept();
        }

        _lastPosition = Core::Vector2f(event->globalPos().x(), event->globalPos().y());
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleQuake::wheelEvent (QWheelEvent* event, const DrawContext& dc)
{
    if (_isPressed==false)
    {
        int32 numDegrees = event->delta() / 8;
        int32 numSteps = numDegrees / 15;

        _ctrl.forwardBackward(numSteps*_moveSpeed*4);

        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleQuake::mouseReleaseEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (event->button() & Qt::LeftButton || event->button() & Qt::MidButton)
    {
        _isPressed = false;
        event->accept();
    }
    else if (event->button() & Qt::RightButton)
    {
        if (_rightIsPressed==true)  {_rightIsPressed = false;_ctrl.setSpeedMode(NORMAL_MODE);}
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleQuake::turnOffNavigation()
{
    for (int32 ii=0; ii<ECameraQuakeMove_COUNT; ++ii)
    {
        _moveActivate[ii] = false;
    }
    _ctrl.turnOff();
    _ctrl.setSpeedMode(NORMAL_MODE);
    _isPressed      = false;
    _rightIsPressed = false;
}
//-----------------------------------------------------------------------------
bool NavigationStyleQuake::isInAction() const
{
    //key
    bool isKeyInAction = false;
    for (int32 ii=0; ii<ECameraQuakeMove_COUNT; ++ii)
    {
        if (_moveActivate[ii] == true)
        {
            isKeyInAction = true;
        }
    }

    return (isKeyInAction == true || _isPressed == true || _rightIsPressed == true);
}
//-----------------------------------------------------------------------------
void NavigationStyleQuake::updateCamera(Ptr<Universe::NodeCamera>& pNodeCam)
{
    if(_moveActivate[QUAKE_FORWARD])        _ctrl.forwardBackward(_moveSpeed);
    if(_moveActivate[QUAKE_BACKWARD])       _ctrl.forwardBackward(-_moveSpeed);
    if(_moveActivate[QUAKE_STRAFE_LEFT])    _ctrl.strafeLeftRight(-_moveSpeed);
    if(_moveActivate[QUAKE_STRAFE_RIGHT])   _ctrl.strafeLeftRight(_moveSpeed);
    if(_moveActivate[QUAKE_DOWN])           _ctrl.moveUpDown(-_moveSpeed);
    if(_moveActivate[QUAKE_UP])             _ctrl.moveUpDown(_moveSpeed);
    if(_moveActivate[QUAKE_ROLL_RIGHT])     _ctrl.rollRightLeft(-(_moveSpeed/2.f));
    if(_moveActivate[QUAKE_ROLL_LEFT])      _ctrl.rollRightLeft(_moveSpeed/2.f);

    _ctrl.updateCamera(pNodeCam);
}
//-----------------------------------------------------------------------------
void NavigationStyleQuake::zoomExtend(const Core::AABoxf& box, Ptr<Universe::NodeCamera>& pNodeCam)
{
    Core::Vector3f dir = pNodeCam->getWorldDirection().getNormalizedSafe();
    float radius = box.getSize().getLengthSafe();

    Core::Vector3f pos = (-dir) * radius + box.getCenter();

    pNodeCam->beginMatrixUpdate();
        pNodeCam->setLocalPosition(pos);
    pNodeCam->endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void NavigationStyleQuake::selectionChanged(const Core::List<Ptr<Property> >& ptiesSelected)
{
    setTargetPosition(ptiesSelected);
}
//-----------------------------------------------------------------------------
void NavigationStyleQuake::resetRoll()
{
    _ctrl.resetRoll();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NavigationStyleOrbital::NavigationStyleOrbital():
    INavigationStyle(),
    _sensibilityRot(0.005f),
    _moveSpeed(0.05f),
    _isPressed(false),
    _maxSpeedFactor(12)
{
    for (int32 ii=0; ii<ECameraOrbitalMove_COUNT; ++ii)
    {
        _moveActivate[ii] = false;
    }
}
//-----------------------------------------------------------------------------
NavigationStyleOrbital::~NavigationStyleOrbital()
{
}
//-----------------------------------------------------------------------------
int32 KEY_ORBITAL_FORWARD         = Qt::Key_Z;
int32 KEY_ORBITAL_BACKWARD        = Qt::Key_S;
int32 KEY_ORBITAL_TURN_LEFT       = Qt::Key_Q;
int32 KEY_ORBITAL_TURN_RIGHT      = Qt::Key_D;
int32 KEY_ORBITAL_SPEED_FACTOR    = Qt::Key_Space;

void NavigationStyleOrbital::keyPressEvent(QKeyEvent* event,   const DrawContext& dc)
{
    if (event->key() == KEY_ORBITAL_SPEED_FACTOR)
    {
        _ctrl.setSpeedFactor(_maxSpeedFactor);
        event->accept();
    }

    if (event->key() == KEY_ORBITAL_FORWARD)
    {
        _moveActivate[OR_FORWARD] = true;
        event->accept();
    }

    if (event->key() == KEY_ORBITAL_BACKWARD)
    {
        _moveActivate[OR_BACKWARD] = true;
        event->accept();
    }

    if (event->key() == KEY_ORBITAL_TURN_LEFT)
    {
        _moveActivate[OR_TURN_LEFT] = true;
        event->accept();
    }

    if (event->key() == KEY_ORBITAL_TURN_RIGHT)
    {
        _moveActivate[OR_TURN_RIGHT] = true;
        event->accept();
    }
}
//----------------------------------------------------------------------------
void NavigationStyleOrbital::keyReleaseEvent(QKeyEvent* event,   const DrawContext& dc)
{
    if (event->key() == KEY_ORBITAL_SPEED_FACTOR)
    {
        _ctrl.setSpeedFactor(1);
        event->accept();
    }

    if (event->key() == KEY_ORBITAL_FORWARD)
    {
        _moveActivate[OR_FORWARD] = false;
        event->accept();
    }

    if (event->key() == KEY_ORBITAL_BACKWARD)
    {
        _moveActivate[OR_BACKWARD] = false;
        event->accept();
    }

    if (event->key() == KEY_ORBITAL_TURN_LEFT)
    {
        _moveActivate[OR_TURN_LEFT] = false;
        event->accept();
    }

    if (event->key() == KEY_ORBITAL_TURN_RIGHT)
    {
        _moveActivate[OR_TURN_RIGHT] = false;
        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleOrbital::turnOffNavigation()
{
    for (int32 ii=0; ii<ECameraOrbitalMove_COUNT; ++ii)
    {
        _moveActivate[ii] = false;
    }
}
//-----------------------------------------------------------------------------
bool NavigationStyleOrbital::isInAction() const
{
    //key
    bool isKeyInAction = false;
    for (int32 ii=0; ii<ECameraQuakeMove_COUNT; ++ii)
    {
        if (_moveActivate[ii] == true)
        {
            isKeyInAction = true;
        }
    }

    return (isKeyInAction == true || _isPressed);
}
//-----------------------------------------------------------------------------
void NavigationStyleOrbital::mousePressEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (event->button() & Qt::LeftButton || event->button() & Qt::MidButton)
    {
        _isPressed = true;
        _lastPosition = Core::Vector2f(event->globalPos().x(), event->globalPos().y());
        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleOrbital::mouseMoveEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (_isPressed == true)
    {
        Core::Vector2f delta = Core::Vector2f(event->globalPos().x(), event->globalPos().y()) - _lastPosition;
        _lastPosition = Core::Vector2f(event->globalPos().x(), event->globalPos().y());

        if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::MidButton)
        {
            _ctrl.turnLeftRight(_sensibilityRot * delta.x);
            _ctrl.turnUpDown(_sensibilityRot * delta.y);

            event->accept();
        }
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleOrbital::wheelEvent (QWheelEvent* event, const DrawContext& dc)
{
    int32 numDegrees = event->delta() / 8;
    int32 numSteps = numDegrees / 15;

    _ctrl.zoomInOut(-numSteps*_moveSpeed*4);

    event->accept();
}
//-----------------------------------------------------------------------------
void NavigationStyleOrbital::mouseReleaseEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (event->button() & Qt::LeftButton || event->buttons() & Qt::MidButton)
    {
        _isPressed = false;
        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleOrbital::updateCamera(Ptr<Universe::NodeCamera>& pNodeCam)
{
    if(_moveActivate[OR_FORWARD])       _ctrl.zoomInOut(-_moveSpeed);
    if(_moveActivate[OR_BACKWARD])      _ctrl.zoomInOut(_moveSpeed);
    if(_moveActivate[OR_TURN_LEFT])     _ctrl.turnLeftRight(-_sensibilityRot);
    if(_moveActivate[OR_TURN_RIGHT])    _ctrl.turnLeftRight(_sensibilityRot);

    _ctrl.updateCamera(pNodeCam);
}
//-----------------------------------------------------------------------------
const Core::Vector3f& NavigationStyleOrbital::getTargetPosition()
{
    return _ctrl.getTargetPosition();
}
//-----------------------------------------------------------------------------
void NavigationStyleOrbital::setTargetPosition(const Core::Vector3f& target)
{
    _ctrl.setTargetPosition(target);
}
//-----------------------------------------------------------------------------
void NavigationStyleOrbital::zoomExtend(const Core::AABoxf& box, Ptr<Universe::NodeCamera>& pNodeCam)
{
    Core::Vector3f targetPos = box.getCenter();//Core::Vector3f::ZERO;
    setTargetPosition(targetPos);
    float distanceTarget =  (targetPos - pNodeCam->getWorldPosition()).getLengthSafe();

    Core::Vector3f dirTarget = pNodeCam->getWorldDirection().getNormalizedSafe();

    float radius = box.getSize().getLengthSafe();

    Core::Vector3f newPos = radius*(-dirTarget) + targetPos;

    pNodeCam->beginMatrixUpdate();
        pNodeCam->setWorldPosition(newPos);
    pNodeCam->endMatrixUpdate();
}
//-----------------------------------------------------------------------------
void NavigationStyleOrbital::selectionChanged(const Core::List<Ptr<Property> >& ptiesSelected)
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NavigationStyleEditionOrthogonal::NavigationStyleEditionOrthogonal():
    INavigationStyle(),
    _zoomFactor(1.f),
    _isPressed(false),
    _lastPosition(0,0),
    _moveSpeed(1.f),
    _maxSpeedFactor(6)
{
    for (int32 ii=0; ii<ECameraEditionOrthogonalMove_COUNT; ++ii)
    {
        _moveActivate[ii] = false;
    }
}
//-----------------------------------------------------------------------------
NavigationStyleEditionOrthogonal::~NavigationStyleEditionOrthogonal()
{
}
//-----------------------------------------------------------------------------
int32 KEY_EDITION_ORTHOGONAL_PAN_UP         = Qt::Key_Z;
int32 KEY_EDITION_ORTHOGONAL_PAN_DOWN       = Qt::Key_S;
int32 KEY_EDITION_ORTHOGONAL_PAN_LEFT       = Qt::Key_Q;
int32 KEY_EDITION_ORTHOGONAL_PAN_RIGHT      = Qt::Key_D;
int32 KEY_EDITION_ORTHOGONAL_SPEED_FACTOR   = Qt::Key_Space;

void NavigationStyleEditionOrthogonal::keyPressEvent(QKeyEvent* event,   const DrawContext& dc)
{
    if (event->key() == KEY_EDITION_ORTHOGONAL_SPEED_FACTOR)
    {
        _ctrl.setSpeedFactor(_maxSpeedFactor);
        event->accept();
    }

    if (event->key() == KEY_EDITION_ORTHOGONAL_PAN_UP)
    {
        _moveActivate[EO_PAN_UP] = true;
        event->accept();
    }

    if (event->key() == KEY_EDITION_ORTHOGONAL_PAN_DOWN)
    {
        _moveActivate[EO_PAN_DOWN] = true;
        event->accept();
    }

    if (event->key() == KEY_EDITION_ORTHOGONAL_PAN_LEFT)
    {
        _moveActivate[EO_PAN_LEFT] = true;
        event->accept();
    }

    if (event->key() == KEY_EDITION_ORTHOGONAL_PAN_RIGHT)
    {
        _moveActivate[EO_PAN_RIGHT] = true;
        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleEditionOrthogonal::keyReleaseEvent(QKeyEvent* event,   const DrawContext& dc)
{
    if (event->key() == KEY_EDITION_ORTHOGONAL_SPEED_FACTOR)
    {
        _ctrl.setSpeedFactor(1);
        event->accept();
    }

    if (event->key() == KEY_EDITION_ORTHOGONAL_PAN_UP)
    {
        _moveActivate[EO_PAN_UP] = false;
        event->accept();
    }

    if (event->key() == KEY_EDITION_ORTHOGONAL_PAN_DOWN)
    {
        _moveActivate[EO_PAN_DOWN] = false;
        event->accept();
    }

    if (event->key() == KEY_EDITION_ORTHOGONAL_PAN_LEFT)
    {
        _moveActivate[EO_PAN_LEFT] = false;
        event->accept();
    }

    if (event->key() == KEY_EDITION_ORTHOGONAL_PAN_RIGHT)
    {
        _moveActivate[EO_PAN_RIGHT] = false;
        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleEditionOrthogonal::mousePressEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (event->button() & Qt::LeftButton || event->button() & Qt::MidButton)
    {
        _isPressed = true;
        _lastPosition = Core::Vector2f(event->globalPos().x(), event->globalPos().y());
        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleEditionOrthogonal::turnOffNavigation()
{
    for (int32 ii=0; ii<ECameraEditionOrthogonalMove_COUNT; ++ii)
    {
        _moveActivate[ii] = false;
    }
}
//-----------------------------------------------------------------------------
bool NavigationStyleEditionOrthogonal::isInAction() const
{
    //key
    bool isKeyInAction = false;
    for (int32 ii=0; ii<ECameraQuakeMove_COUNT; ++ii)
    {
        if (_moveActivate[ii] == true)
        {
            isKeyInAction = true;
        }
    }

    return (isKeyInAction == true || _isPressed);
}
//-----------------------------------------------------------------------------
void NavigationStyleEditionOrthogonal::mouseMoveEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (_isPressed==true)
    {
        Core::Vector2f delta = Core::Vector2f(event->globalPos().x(), event->globalPos().y()) - _lastPosition;
        _lastPosition = Core::Vector2f(event->globalPos().x(), event->globalPos().y());

        if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::MidButton)
        {
            _ctrl.strafeLeftRight(delta.x);
            _ctrl.moveUpDown(delta.y);

            event->accept();
        }
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleEditionOrthogonal::mouseReleaseEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (event->button() & Qt::LeftButton || event->button() & Qt::MidButton)
    {
        _isPressed = false;
        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleEditionOrthogonal::wheelEvent (QWheelEvent* event, const DrawContext& dc)
{
    if (_isPressed == false)
    {
        int32 numDegrees = event->delta() / 8;
        int32 numSteps = numDegrees / 15;
        float deltaWorkDistance = numSteps * _zoomFactor;

        _ctrl.zoomInOut(-deltaWorkDistance);

        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NavigationStyleEditionOrthogonal::updateCamera(Ptr<Universe::NodeCamera>& pNodeCam)
{
    if(_moveActivate[EO_PAN_UP])       _ctrl.moveUpDown(-_moveSpeed);
    if(_moveActivate[EO_PAN_DOWN])     _ctrl.moveUpDown(_moveSpeed);
    if(_moveActivate[EO_PAN_LEFT])     _ctrl.strafeLeftRight(-_moveSpeed);
    if(_moveActivate[EO_PAN_RIGHT])    _ctrl.strafeLeftRight(_moveSpeed);

    _ctrl.updateCamera(pNodeCam);
}
//-----------------------------------------------------------------------------
void NavigationStyleEditionOrthogonal::zoomExtend(const Core::AABoxf& box, Ptr<Universe::NodeCamera>& pNodeCam)
{
    Ptr<NodeCameraEdition> pNodeCamEdit = Core::ptrDynamicCast<NodeCameraEdition>(pNodeCam);

    Core::Vector3f posNode = pNodeCamEdit->getWorldPosition();
    Core::Vector3f centerBox = box.getCenter();

    pNodeCamEdit->beginMatrixUpdate();
        pNodeCamEdit->setWorldPosition(centerBox);
    pNodeCamEdit->endMatrixUpdate();
    
    float radius = box.getSize().getLengthSafe()/2;

    pNodeCamEdit->setWorkdistance(radius/Core::L_tan(f_PI/2/2));
}
//-----------------------------------------------------------------------------
void NavigationStyleEditionOrthogonal::selectionChanged(const Core::List<Ptr<Property> >& ptiesSelected)
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ControllerQuake::ControllerQuake() :
    _deltaPosition(Core::ZERO),
    _yaw(0),
    _roll(0),
    _pitch(0),
    _speedFactor(40),
    _slowSpeedFactor(1),
    _normalSpeedFactor(10),
    _mode(NORMAL_MODE),
    _slowEngage(false),
    _slowEngageFct(10.f),
    _modeOrbit(false),
    _resetRoll(false),
    _makeTargetPosition(false),
    pPtiesSelected(NULL)
{

}
//-----------------------------------------------------------------------------
void ControllerQuake::turnOff()
{
    _modeOrbit          = false;
    _deltaPosition      = Core::Vector3f (Core::ZERO);
    _yaw                = 0;
    _roll               = 0;
    _pitch              = 0;
}
//-----------------------------------------------------------------------------
ControllerQuake::~ControllerQuake()
{
}
//-----------------------------------------------------------------------------
void ControllerQuake::engageSlow()
{
    _slowEngage = !_slowEngage;
}
//-----------------------------------------------------------------------------
void ControllerQuake::turnLeftRight (float delta)
{
    _yaw += delta;
    _yaw = fmod(_yaw, f_PI_MUL_2);
}
//-----------------------------------------------------------------------------
void ControllerQuake::turnUpDown (float delta)
{
    _pitch += delta;
    _pitch = std::max(-f_PI_DIV_2, std::min(f_PI_DIV_2, _pitch));
}
//-----------------------------------------------------------------------------
void ControllerQuake::setTargetPosition(const Core::List<Ptr<Property> >& ptiesSelected)
{
    _makeTargetPosition = true;
    pPtiesSelected = &ptiesSelected;
}
//-----------------------------------------------------------------------------
void ControllerQuake::forwardBackward (float delta)
{
    _deltaPosition.z += delta*getSpeedFactor();
}
//-----------------------------------------------------------------------------
void ControllerQuake::strafeLeftRight (float delta)
{
    _deltaPosition.x += delta*getSpeedFactor();
}
//-----------------------------------------------------------------------------
void ControllerQuake::moveUpDown (float delta)
{
    _deltaPosition.y += delta*getSpeedFactor();
}
//-----------------------------------------------------------------------------
void ControllerQuake::rollRightLeft (float delta)
{
    _roll += delta;
    _roll = fmod(_roll, f_PI_MUL_2);
}
//-----------------------------------------------------------------------------
void ControllerQuake::setExtraSpeedFactor(float fct)
{
    _speedFactor = fct;
}
//-----------------------------------------------------------------------------
void ControllerQuake::setSlowSpeedFactor(float fct)
{
    _slowSpeedFactor = fct;
}
//-----------------------------------------------------------------------------
void ControllerQuake::setNormalSpeedFactor(float fct)
{
    _normalSpeedFactor = fct;
}
//-----------------------------------------------------------------------------
void ControllerQuake::setSpeedMode(ENavigationSpeedMode mode)
{
    _mode = mode;
}
//-----------------------------------------------------------------------------
float ControllerQuake::getSpeedFactor()
{
    float ret;

    switch(_mode)
    {
        case SLOW_MODE:         ret = _slowSpeedFactor; break;
        case NORMAL_MODE:       ret = _normalSpeedFactor; break;
        case EXTRA_SPEED_MODE:  ret = _speedFactor; break;
    }

    if (_slowEngage==true)
        ret /= _slowEngageFct;

    return ret;
}
//-----------------------------------------------------------------------------
void ControllerQuake::resetRoll()
{
    _resetRoll = true;
}
//-----------------------------------------------------------------------------
void ControllerQuake::computeTargetPosition()
{
    if (_makeTargetPosition == true)
    {
        Core::Vector3f pos (Core::ZERO);
        int32           nbPos = 0;

        if (pPtiesSelected!=NULL)
        {
            for (int32 ii=0; ii<pPtiesSelected->size(); ++ii)
            {
                Ptr<PtyTransform> pPtyT   = LM_DEBUG_PTR_CAST<PtyTransform>((*pPtiesSelected)[ii]->getPtyTransform());


                if (pPtyT!= null)
                {
                    pos += pPtyT->getCenterBoundingBoxInWorldAxis();
                    nbPos++;
                }
            }

            if (nbPos>0)
            {
                pos /= nbPos;
            }
        }

        _targetPos = pos;
    }
}
//-----------------------------------------------------------------------------
void ControllerQuake::updateCamera (Ptr<Universe::NodeCamera>& pNodeCam)
{
    if(_modeOrbit==false || pPtiesSelected == NULL)
    {
        Core::Vector3f newPos =  pNodeCam->getLocalMatrix().apply3x3(_deltaPosition) + pNodeCam->getWorldPosition();

        pNodeCam->beginMatrixUpdate();
            pNodeCam->setWorldPosition(newPos);
            pNodeCam->localPitch(_pitch);
            pNodeCam->worldYaw(_yaw);
            pNodeCam->localRoll(_roll);
        pNodeCam->endMatrixUpdate();

        if (_resetRoll==true)
        {
            pNodeCam->beginMatrixUpdate();
                pNodeCam->setUpVector(Core::Vector3f(0,1,0));
                pNodeCam->setWorldDirection(pNodeCam->getWorldDirection());
            pNodeCam->endMatrixUpdate();

            _resetRoll=false;
        }
    }
    else
    {
        computeTargetPosition();


        Core::Vector3f  dirTarget       = (_targetPos - pNodeCam->getWorldPosition()).getNormalizedSafe();
        float           distanceTarget  =  (_targetPos - pNodeCam->getWorldPosition()).getLengthSafe() - _deltaPosition.z;
        

        if (Core::L_abs(distanceTarget) < f_COMMON_EPSILON)
        {
            Core::Quaternionf orient = pNodeCam->getLocalOrient();
            pNodeCam->beginMatrixUpdate();
                pNodeCam->setWorldPosition(_targetPos +  Core::Vector3f(0,0,100));
                pNodeCam->setLocalOrientation(orient);
            pNodeCam->endMatrixUpdate();

            dirTarget = pNodeCam->getLocalDirection().getNormalizedSafe();
            distanceTarget  =  (_targetPos - pNodeCam->getWorldPosition()).getLengthSafe() + _deltaPosition.getLengthSafe();
        }

        distanceTarget = Core::max(0.2f, distanceTarget);

        Core::Rotationf rotationP (_targetPos, pNodeCam->getLocalMatrix().getXVector(), _pitch);
        Core::Rotationf rotationY (_targetPos, Core::Vector3f(0,1,0), _yaw);

        Core::Transformf transformCam (pNodeCam->getLocalPosition(), pNodeCam->getLocalOrient(), pNodeCam->getLocalScale());
        transformCam.rotateInGlobalAxis(rotationP);
        transformCam.rotateInGlobalAxis(rotationY);


        pNodeCam->beginMatrixUpdate();
            pNodeCam->setLocalPosition(transformCam._translation);
            pNodeCam->setLocalOrientation(transformCam._rotation);
        pNodeCam->endMatrixUpdate();

        dirTarget = pNodeCam->getLocalDirection().getNormalizedSafe();
        Core::Vector3f  newPos          = distanceTarget*(-dirTarget) + _targetPos;

        pNodeCam->beginMatrixUpdate();
            pNodeCam->setWorldPosition(newPos);
        pNodeCam->endMatrixUpdate();
    }

    _pitch = 0;
    _yaw   = 0;
    _roll  = 0;
    _deltaPosition.setZero();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ControllerOrbital::ControllerOrbital() :
    _deltaYaw(0),
    _deltaPitch(0),
    _deltaDistanceTarget(0),
    _targetPos(Core::ZERO),
    _speedFactor(1)
{
}
//-----------------------------------------------------------------------------
ControllerOrbital::~ControllerOrbital()
{
}
//-----------------------------------------------------------------------------
void ControllerOrbital::turnLeftRight(float delta)
{
    _deltaYaw +=delta;
    _deltaYaw = fmod(_deltaYaw, f_PI_MUL_2);
}
//-----------------------------------------------------------------------------
void ControllerOrbital::turnUpDown(float delta)
{
    _deltaPitch += delta;
    _deltaPitch = std::max(-f_PI_DIV_2, std::min(f_PI_DIV_2, _deltaPitch));
}
//-----------------------------------------------------------------------------
void ControllerOrbital::zoomInOut(float delta)
{
    _deltaDistanceTarget += delta*_speedFactor;
}
//-----------------------------------------------------------------------------
void ControllerOrbital::updateCamera (Ptr<Universe::NodeCamera>& pNodeCam)
{

    Core::Vector3f  dirTarget       = (_targetPos - pNodeCam->getWorldPosition()).getNormalizedSafe();
    float           distanceTarget  =  (_targetPos - pNodeCam->getWorldPosition()).getLengthSafe() + _deltaDistanceTarget;
    

    if (Core::L_abs(distanceTarget) < f_COMMON_EPSILON)
    {
        if ((_targetPos - pNodeCam->getWorldPosition()).getLengthSafe() > f_COMMON_EPSILON)
        {
            distanceTarget = (_targetPos - pNodeCam->getWorldPosition()).getLengthSafe();
        }
        else
        {
            Core::Quaternionf orient = pNodeCam->getLocalOrient();
            pNodeCam->beginMatrixUpdate();
                pNodeCam->setWorldPosition(_targetPos +  Core::Vector3f(0,0,100));
                pNodeCam->setLocalOrientation(orient);
            pNodeCam->endMatrixUpdate();

            dirTarget = pNodeCam->getLocalDirection().getNormalizedSafe();
            distanceTarget  =  (_targetPos - pNodeCam->getWorldPosition()).getLengthSafe() + _deltaDistanceTarget;
        }
    }

    distanceTarget = Core::max(0.2f, distanceTarget);



    Core::Rotationf rotationP (_targetPos, pNodeCam->getLocalMatrix().getXVector(), _deltaPitch);
    Core::Rotationf rotationY (_targetPos, Core::Vector3f(0,1,0)/*pNodeCam->getLocalMatrix().getYVector()*/, _deltaYaw);

    Core::Transformf transformCam (pNodeCam->getLocalPosition(), pNodeCam->getLocalOrient(), pNodeCam->getLocalScale());
    transformCam.rotateInGlobalAxis(rotationP);
    transformCam.rotateInGlobalAxis(rotationY);


    pNodeCam->beginMatrixUpdate();
        pNodeCam->setLocalPosition(transformCam._translation);
        pNodeCam->setLocalOrientation(transformCam._rotation);
    pNodeCam->endMatrixUpdate();

    dirTarget = pNodeCam->getLocalDirection().getNormalizedSafe();
    Core::Vector3f  newPos          = distanceTarget*(-dirTarget) + _targetPos;

    pNodeCam->beginMatrixUpdate();
        pNodeCam->setWorldPosition(newPos);
    pNodeCam->endMatrixUpdate();


    _deltaPitch          = 0;
    _deltaYaw            = 0;
    _deltaDistanceTarget = 0;
}
//-----------------------------------------------------------------------------
void ControllerOrbital::setSpeedFactor(float fct)
{
    _speedFactor = fct;
}
//-----------------------------------------------------------------------------
const Core::Vector3f& ControllerOrbital::getTargetPosition()
{
    return _targetPos;
}
//-----------------------------------------------------------------------------
void ControllerOrbital::setTargetPosition(const Core::Vector3f& target)
{
    _targetPos = target;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ControllerEditionOrthogonal::ControllerEditionOrthogonal() :
    _deltaWorkDistance(0),
    _deltaLeftRight(0),
    _deltaUpDown(0),
    _speedFactor(1),
    _angularDisplacement(Core::deg2rad(.1f))
{
}
//-----------------------------------------------------------------------------
ControllerEditionOrthogonal::~ControllerEditionOrthogonal()
{
}
//-----------------------------------------------------------------------------
void ControllerEditionOrthogonal::strafeLeftRight    (float delta)
{
    _deltaLeftRight += delta*_speedFactor;
}
//-----------------------------------------------------------------------------
void ControllerEditionOrthogonal::moveUpDown         (float delta)
{
    _deltaUpDown += delta*_speedFactor;
}
//-----------------------------------------------------------------------------
void ControllerEditionOrthogonal::zoomInOut         (float delta)
{
    _deltaWorkDistance += delta*_speedFactor;
}
//-----------------------------------------------------------------------------
void ControllerEditionOrthogonal::updateCamera      (Ptr<Universe::NodeCamera>& pNodeCam)
{
    Ptr<NodeCameraEdition> pCamEdition = LM_DEBUG_PTR_CAST<NodeCameraEdition>(pNodeCam);

    if (pCamEdition!=null)
    {
        pCamEdition->beginMatrixUpdate();
            pCamEdition->setWorkdistance(pCamEdition->getWorkdistance() + _deltaWorkDistance);
        pCamEdition->endMatrixUpdate();

        Core::Vector3f pos   = pNodeCam->getLocalPosition();
        Core::Vector3f Zaxis = pNodeCam->getLocalToWorldMatrix().getZVector();
        Core::Vector3f Yaxis = pNodeCam->getLocalToWorldMatrix().getYVector();
        Core::Vector3f Xaxis = pNodeCam->getLocalToWorldMatrix().getXVector();
        Core::Vector3f deltaPos (Core::ZERO);

            
        float deltaDistance = pCamEdition->getWorkdistance() * Core::L_tan(_angularDisplacement);
        Core::Vector2f delta2D (_deltaLeftRight*deltaDistance, _deltaUpDown*deltaDistance);

        switch (pCamEdition->getActiveView())
        {
            case CAM_EDIT_TOP:
                deltaPos = Xaxis*delta2D.x*-1 + Zaxis*delta2D.y;
                break;
            case CAM_EDIT_BOTTOM:
                deltaPos = Xaxis*delta2D.x*-1 + Zaxis*delta2D.y*-1;
                break;
            case CAM_EDIT_BACK:
                deltaPos = Xaxis*delta2D.x + Yaxis*delta2D.y;
                break;
            case CAM_EDIT_FRONT:
                deltaPos = Xaxis*delta2D.x*-1 + Yaxis*delta2D.y;
                break;
            case CAM_EDIT_RIGHT:
                deltaPos = Zaxis*delta2D.x*-1 + Yaxis*delta2D.y;
                break;
            case CAM_EDIT_LEFT:
                deltaPos = Zaxis*delta2D.x + Yaxis*delta2D.y;
                break;
        }

        pNodeCam->setLocalPosition(pos+deltaPos);
    }

    _deltaWorkDistance  = 0;
    _deltaLeftRight     = 0;
    _deltaUpDown        = 0;
}
//-----------------------------------------------------------------------------
void ControllerEditionOrthogonal::setSpeedFactor(float fct)
{
    _speedFactor = fct;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace EPI
