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
#include <Core/Math/Color.h>
#include <Workflow/COLLADA/COLLADATools.h>

namespace Workflow
{
void swapMat(Core::Matrix4f & m)
{
    Core::Matrix4f om1(-1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  0.0f, -1.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        0.0f,  0.0f,  0.0f,  1.0f);

    Core::Matrix4f om2(-1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  0.0f,  1.0f,  0.0f,
                        0.0f, -1.0f,  0.0f,  0.0f,
                        0.0f,  0.0f,  0.0f,  1.0f);

    m = om2 * (m * om1);
}

Core::Matrix4f cvtMat4(const COLLADABU::Math::Matrix4 & mat, bool swapYZ, float scale)
{
    Core::Matrix4f m(float(mat.getElement( 0)), float(mat.getElement( 1)), float(mat.getElement( 2)), float(mat.getElement( 3)),
                     float(mat.getElement( 4)), float(mat.getElement( 5)), float(mat.getElement( 6)), float(mat.getElement( 7)),
                     float(mat.getElement( 8)), float(mat.getElement( 9)), float(mat.getElement(10)), float(mat.getElement(11)),
                     float(mat.getElement(12)), float(mat.getElement(13)), float(mat.getElement(14)), float(mat.getElement(15)));

    if(swapYZ)
        swapMat(m);

    m.setTVector(scale * m.getTVector());

    return m;
}

Core::Matrix4f cvtMat4(const Core::Matrix4f & mat, bool swapYZ, float scale)
{
    Core::Matrix4f m(mat);

    if(swapYZ)
        swapMat(m);

    m.setTVector(scale * m.getTVector());
    
    return m;
}

Core::Vector4f cvtColor(const COLLADAFW::Color & color)
{
    return Core::Vector4f(
        Core::SRGB2Linear(float(color.getRed())), 
        Core::SRGB2Linear(float(color.getGreen())), 
        Core::SRGB2Linear(float(color.getBlue())), 
        Core::SRGB2Linear(float(color.getAlpha()))
        );
}

}
