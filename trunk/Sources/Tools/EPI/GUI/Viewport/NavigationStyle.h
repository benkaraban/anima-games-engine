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
#ifndef NAVIGATION_STYLE_H_
#define NAVIGATION_STYLE_H_

#include <EPI/Gui/Viewport/INavigationStyle.moc.h>


class QMouseEvent;
class QKeyEvent;
class QWheelEvent;


namespace Universe
{
    class NodeCamera;
}

namespace EPI
{
//-----------------------------------------------------------------------------


LM_ENUM_3(ENavigationSpeedMode,
          SLOW_MODE,
          NORMAL_MODE,
          EXTRA_SPEED_MODE)
//-----------------------------------------------------------------------------
LM_ENUM_8 (ECameraQuakeMove,
           QUAKE_FORWARD,
           QUAKE_BACKWARD,
           QUAKE_STRAFE_LEFT,
           QUAKE_STRAFE_RIGHT,
           QUAKE_DOWN,
           QUAKE_UP,
           QUAKE_ROLL_RIGHT,
           QUAKE_ROLL_LEFT)

LM_ENUM_4 (ECameraOrbitalMove,
           OR_FORWARD,
           OR_BACKWARD,
           OR_TURN_LEFT,
           OR_TURN_RIGHT)

LM_ENUM_4 (ECameraEditionOrthogonalMove,
           EO_PAN_UP,
           EO_PAN_DOWN,
           EO_PAN_LEFT,
           EO_PAN_RIGHT)
//-----------------------------------------------------------------------------
class ControllerQuake
{
public:
    ControllerQuake();
    ~ControllerQuake();

    void turnLeftRight  (float delta);
    void turnUpDown     (float delta);


    void forwardBackward(float delta);
    void strafeLeftRight(float delta);
    void moveUpDown     (float delta);
    
    void rollRightLeft   (float delta);

    void updateCamera      (Ptr<Universe::NodeCamera>& pNodeCam);
    void computeTargetPosition();
    
    void setSlowSpeedFactor     (float fct);
    void setNormalSpeedFactor   (float fct);
    void setExtraSpeedFactor    (float fct);
    void setSpeedMode(ENavigationSpeedMode mode);

    float getSpeedFactor();

    void engageSlow();
    void setModeOrbit(bool flag) {_makeTargetPosition=flag; _modeOrbit = flag;};
    const Core::Vector3f& getTargetPosition() {return _targetPos;}
    void setTargetPosition(const Core::List<Ptr<Property> >& ptiesSelected);

    void resetRoll();
    void turnOff();

private:
    const Core::List<Ptr<Property> >* pPtiesSelected;

    float                  _yaw;
    float                  _pitch;
    float                  _roll;
    Core::Vector3f        _deltaPosition;

    
    float                   _slowSpeedFactor;
    float                   _normalSpeedFactor;
    float                   _extraSpeedFactor;
    float                   _speedFactor;
    bool                    _slowEngage;
    bool                    _resetRoll;
    bool                    _modeOrbit;
    bool                    _makeTargetPosition;
    float                   _slowEngageFct;
    Core::Vector3f          _targetPos;
    ENavigationSpeedMode    _mode;
};
//-----------------------------------------------------------------------------
class ControllerOrbital
{
public:
    ControllerOrbital();
    ~ControllerOrbital();

    void turnLeftRight(float delta);
    void turnUpDown(float delta);
    void zoomInOut(float delta);

    void updateCamera      (Ptr<Universe::NodeCamera>& pNodeCam);

    void setSpeedFactor(float fct);

    const Core::Vector3f& getTargetPosition();
    void setTargetPosition(const Core::Vector3f& target);

private:
    float           _deltaYaw;
    float           _deltaPitch;
    float           _deltaDistanceTarget;

    Core::Vector3f  _targetPos;
    float           _speedFactor;
};
//-----------------------------------------------------------------------------
class ControllerEditionOrthogonal
{
public:
    ControllerEditionOrthogonal();
    ~ControllerEditionOrthogonal();

    void strafeLeftRight    (float delta);
    void moveUpDown         (float delta);
    void zoomInOut          (float delta);

    void updateCamera      (Ptr<Universe::NodeCamera>& pNodeCam);

    void setSpeedFactor(float fct);

private:
    float           _angularDisplacement;   //radian
    float           _deltaWorkDistance;
    float           _deltaLeftRight;
    float           _deltaUpDown;
    float           _speedFactor;
};
//-----------------------------------------------------------------------------
class LM_API_EPI NavigationStyleQuake : public INavigationStyle
{
public:
    NavigationStyleQuake();
    virtual ~NavigationStyleQuake();

    virtual ENavigationStyleType getType() {return NAVIGATION_QUAKE;}


    virtual void turnOffNavigation();
    virtual bool isInAction() const;

    virtual void keyPressEvent      (QKeyEvent* event,   const DrawContext& dc);
    virtual void keyReleaseEvent    (QKeyEvent* event,   const DrawContext& dc);
    virtual void mousePressEvent    (QMouseEvent* event, const DrawContext& dc);
    virtual void mouseMoveEvent     (QMouseEvent* event, const DrawContext& dc);
    virtual void mouseReleaseEvent  (QMouseEvent* event, const DrawContext& dc);
    virtual void wheelEvent         (QWheelEvent* event, const DrawContext& dc);

    virtual void updateCamera      (Ptr<Universe::NodeCamera>& pNodeCam);
    virtual void zoomExtend(const Core::AABoxf& box, Ptr<Universe::NodeCamera>& pNodeCam);
    
    const Core::Vector3f& getTargetPosition()               {return _ctrl.getTargetPosition();}
    void setTargetPosition(const Core::List<Ptr<Property> >& ptiesSelected)    {_ctrl.setTargetPosition(ptiesSelected);}

    virtual void selectionChanged(const Core::List<Ptr<Property> >& ptiesSelected);

    void resetRoll();

private:
    float                   _sensibilityRot;
    float                   _moveSpeed;

    bool                    _isPressed;
    bool                    _rightIsPressed;

    Core::Vector2f          _lastPosition;
    bool                    _moveActivate[ECameraQuakeMove_COUNT];

    ControllerQuake         _ctrl;
    float                   _slowSpeedFactor;
    float                   _normalSpeedFactor;
    float                   _extraSpeedFactor;

    Core::TimeValue         _lastRollRight;
    Core::TimeValue         _lastRollLeft;
    Core::TimeDuration      _tdForResetRoll;
};
//-----------------------------------------------------------------------------

class LM_API_EPI NavigationStyleOrbital : public INavigationStyle
{
public:
    NavigationStyleOrbital();
    virtual ~NavigationStyleOrbital();

    virtual ENavigationStyleType getType() {return NAVIGATION_ORBITAL;}


    virtual void turnOffNavigation();
    virtual bool isInAction() const;

    virtual void keyPressEvent      (QKeyEvent* event,   const DrawContext& dc);
    virtual void keyReleaseEvent    (QKeyEvent* event,   const DrawContext& dc);
    virtual void mousePressEvent    (QMouseEvent* event, const DrawContext& dc);
    virtual void mouseMoveEvent     (QMouseEvent* event, const DrawContext& dc);
    virtual void mouseReleaseEvent  (QMouseEvent* event, const DrawContext& dc);
    virtual void wheelEvent         (QWheelEvent* event, const DrawContext& dc);

    virtual void updateCamera      (Ptr<Universe::NodeCamera>& pNodeCam);
    virtual void zoomExtend(const Core::AABoxf& box, Ptr<Universe::NodeCamera>& pNodeCam);
    const Core::Vector3f& getTargetPosition();
    void setTargetPosition(const Core::Vector3f& target);

    virtual void selectionChanged(const Core::List<Ptr<Property> >& ptiesSelected);
private:
    float           _sensibilityRot;
    float           _moveSpeed;
    bool            _isPressed;
    Core::Vector2f  _lastPosition;
 
    bool                _moveActivate[ECameraOrbitalMove_COUNT];

    ControllerOrbital   _ctrl;
    float               _maxSpeedFactor;
};

//-----------------------------------------------------------------------------
class LM_API_EPI NavigationStyleEditionOrthogonal : public INavigationStyle
{
public:
    NavigationStyleEditionOrthogonal();
    virtual ~NavigationStyleEditionOrthogonal();

    virtual ENavigationStyleType getType() {return NAVIGATION_EDITION_ORTHOGONAL;}

    virtual void turnOffNavigation();
    virtual bool isInAction() const;

    virtual void keyPressEvent      (QKeyEvent* event,   const DrawContext& dc);
    virtual void keyReleaseEvent    (QKeyEvent* event,   const DrawContext& dc);
    virtual void mousePressEvent    (QMouseEvent* event, const DrawContext& dc);
    virtual void mouseMoveEvent     (QMouseEvent* event, const DrawContext& dc);
    virtual void mouseReleaseEvent  (QMouseEvent* event, const DrawContext& dc);
    virtual void wheelEvent         (QWheelEvent* event, const DrawContext& dc);

    virtual void updateCamera      (Ptr<Universe::NodeCamera>& pNodeCam);
    virtual void zoomExtend(const Core::AABoxf& box, Ptr<Universe::NodeCamera>& pNodeCam);

    virtual void selectionChanged(const Core::List<Ptr<Property> >& ptiesSelected);
private:
    float           _zoomFactor;
    bool            _isPressed;
    Core::Vector2f  _lastPosition;

    bool            _moveActivate[ECameraEditionOrthogonalMove_COUNT];
    float _moveSpeed;
    ControllerEditionOrthogonal _ctrl;
        float           _maxSpeedFactor;
};
//-----------------------------------------------------------------------------

} //namespace EPI

#endif //NAVIGATION_STYLE_H_
