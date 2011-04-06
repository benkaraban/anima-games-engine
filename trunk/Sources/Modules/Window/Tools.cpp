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
#include <Window/Tools.h>

namespace Window
{
//-----------------------------------------------------------------------------
void updateQuakeCamera(Core::CameraQuake & camera, const InputManager & input, double moveSpeed, double rotSpeed, double elapsed)
{
    moveSpeed *= elapsed;

    if(input.isLMBDown() && !input.isKeyDown(VK_LSHIFT))
    {
        camera.turnLeftRight(input.getMouseXDelta() * float(rotSpeed));
        camera.turnUpDown(input.getMouseYDelta() * float(rotSpeed));
    }

    if(input.isKeyDown('Z')) camera.forward(float(moveSpeed));
    if(input.isKeyDown('S')) camera.backward(float(moveSpeed));
    if(input.isKeyDown('Q')) camera.strafeLeft(float(moveSpeed));
    if(input.isKeyDown('D')) camera.strafeRight(float(moveSpeed));
    if(input.isKeyDown('A')) camera.moveDown(float(moveSpeed));
    if(input.isKeyDown('E')) camera.moveUp(float(moveSpeed));

    const Core::List<Ptr<IJoystick> > & joys = input.getJoysticks();

    if(joys.size() > 0)
    {
        IJoystick & joy = *joys[0];

        camera.backward(float(moveSpeed * joy.yAxisL()));
        camera.strafeRight(float(moveSpeed * joy.xAxisL()));

        camera.turnLeftRight(float(elapsed * rotSpeed * 200.0f * joy.xAxisR()));
        camera.turnUpDown(float(elapsed * rotSpeed * 200.0f * joy.yAxisR()));
    }
}
//-----------------------------------------------------------------------------
void updateOrbitalCamera(Core::CameraOrbital & camera, const InputManager & input, double moveSpeed, double rotSpeed, double depthSpeed, double elapsed)
{
    int32 dx = input.getMouseXDelta();
    int32 dy = input.getMouseYDelta();

    if(input.isLMBDown() && !input.isKeyDown(VK_LSHIFT))
    {
        camera.turnLeftRight(float(rotSpeed) * float(dx));
        camera.turnUpDown(float(rotSpeed) * float(dy));
    }
    else if(input.isMMBDown())
    {
        camera.relativeMove(float(moveSpeed * elapsed * dx), float(-moveSpeed * elapsed * dy));
    }

    if(!input.isKeyDown(VK_LSHIFT))
    {
        camera.zoomInOut(float(input.getMouseWheelDelta() * depthSpeed));
    }

    const Core::List<Ptr<IJoystick> > & joys = input.getJoysticks();

    if(joys.size() > 0)
    {
        IJoystick & joy = *joys[0];

        camera.turnUpDown(float(elapsed * 200.0f * rotSpeed) * joy.yAxisL());
        camera.turnLeftRight(float(elapsed * 200.0f * rotSpeed) * joy.xAxisL());

        camera.zoomInOut(float(elapsed * 100.0f * depthSpeed * joy.yAxisR()));
    }
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
}
