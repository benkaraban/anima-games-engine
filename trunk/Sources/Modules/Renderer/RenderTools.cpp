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
#include <Core/Math/Interpolate.h>
#include <Renderer/RenderTools.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
void generateTrail(Core::List<Renderer::FreeFormVertex> & vert, 
                   Core::List<uint16> & ind, 
                   const Core::List<Core::Vector3f> & positions, 
                   const TrailConfig & trail,
                   bool append)
{
    int32 segCount = std::max(0, positions.size() - 1);
    int32 internalCount = std::max(1, segCount - 1);

    if(!append)
    {
        vert.clear();
        ind.clear();
    }

    int32 oldVertCount = vert.size();
    int32 iv = oldVertCount;

    Core::Vector3f lastV(Core::Vector3f::ZERO);
    vert.resize(oldVertCount + 2 * (segCount + 1));

    Core::List<Core::Vector3f> uVecs(segCount);
    Core::List<Core::Vector3f> vVecs(segCount);

    Core::List<float> angles(segCount);

    Core::Vector3f lastVVec(0.0f, 0.0f, 1.0f);

    if(trail.mode == TRAIL_TANGENT_LOCAL || trail.mode == TRAIL_TANGENT_VIEW)
    {
        for(int32 ii=0; ii < vVecs.size(); ii++)
        {
            const Core::Vector3f & p1 = positions[ii + 0];
            const Core::Vector3f & p2 = positions[ii + 1];
            uVecs[ii] = (p2 - p1).getNormalizedSafe();
        }

        for(int32 ii=0; ii < vVecs.size() - 1; ii++)
        {
            const Core::Vector3f & u1 = uVecs[ii + 0];

            for(int32 jj=ii + 1; jj < vVecs.size(); jj++)
            {
                const Core::Vector3f & u2 = uVecs[jj];
                vVecs[ii] = Core::cross(u1, u2);
                
                if(!vVecs[ii].isZero())
                {
                    vVecs[ii].normalizeSafe();
                    break;
                }
            }

            if(Core::dot(vVecs[ii], lastVVec) < 0.0f)
                vVecs[ii] = -vVecs[ii];

            float angle = f_PI_DIV_2 - Core::L_acosSafe((Core::dot(vVecs[ii], trail.viewVec)));

            angle = abs(angle);

            Core::Vector3f axis;
            if(trail.mode == TRAIL_TANGENT_VIEW)
                axis = Core::cross(trail.viewVec, vVecs[ii]).getNormalizedSafe();
            else //if(trail.mode == TRAIL_TANGENT_LOCAL)
                axis = uVecs[ii];

            angles[ii] = angle;
            LM_ASSERT(angle >= -f_PI);
            LM_ASSERT(angle <=  f_PI);

            Core::Matrix4f rot(Core::ROTATION, angle, axis);
            vVecs[ii] = rot.apply3x3(vVecs[ii]);
        }

        if(vVecs.size() >= 3)
        {
            vVecs[vVecs.size() - 2] = vVecs[vVecs.size() - 3];
            vVecs[vVecs.size() - 1] = vVecs[vVecs.size() - 3];
        }
    }

    for(int32 ii=0; ii < positions.size(); ii++)
    {
        int32 i0 = ii;
        int32 i1 = ii + 1;

        float s1 = 0.0f;
        float s2 = 0.0f;

        if(i0 == 0)
            s1 = 0.0f;
        else if(i0 >= (positions.size() - 1) && trail.tailClamp)
            s1 = 1.0f;
        else
            s1 = float(i0 - 1) / float(internalCount) + trail.duStart;

        if(i1 == 0)
            s2 = 0.0f;
        else if(i1 >= (positions.size() - 1) && trail.tailClamp)
            s2 = 1.0f;
        else
            s2 = float(i1 - 1) / float(internalCount) + trail.duStart;

        int32 idx1 = std::min(positions.size() - 1, i1);
        int32 idx0 = std::max(idx1 - 1, 0);

        const Core::Vector3f & p1 = positions[idx0];
        const Core::Vector3f & p2 = positions[idx1];
        
        int32 iVec = std::min(ii, uVecs.size() - 1);

        Core::Vector3f u;
        Core::Vector3f v;

        switch(trail.mode)
        {
        case TRAIL_STANDARD_UNORM:
            u = (p2 - p1).getNormalizedSafe();
            v = Core::cross(u, trail.viewVec);
            break;
        case TRAIL_STANDARD_NORM:
            u = (p2 - p1).getNormalizedSafe();
            v = Core::cross(u, trail.viewVec).getNormalizedSafe();
            break;
        case TRAIL_TANGENT_LOCAL:
        case TRAIL_TANGENT_VIEW:
            u = uVecs[iVec];
            v = vVecs[iVec];
            break;
        }

        float width1 = 0.5f * Core::lerp(trail.headWidth, trail.tailWidth, s1);
        float width2 = 0.5f * Core::lerp(trail.headWidth, trail.tailWidth, s2);

        Core::Vector4f color1(trail.headColor, trail.tailColor, s1);
        Core::Vector4f color2(trail.headColor, trail.tailColor, s2);

        Core::Vector4f glow1(trail.headGlow, trail.tailGlow, s1);
        Core::Vector4f glow2(trail.headGlow, trail.tailGlow, s2);

        if(trail.tailFade)
        {
            if(i0 >= (positions.size() - 1))
                color1.a = glow1.a = 0.0f;
            if(i1 >= (positions.size() - 1))
                color2.a = glow2.a = 0.0f;
        }

        Core::Vector3f p(p1);

        if(ii == positions.size() - 1)
            p = p2;

        vert[iv + 0].position     = p + width1 * v;
        vert[iv + 0].texcoords[0] = Core::lerp(trail.headU, trail.tailU, s1);

        vert[iv + 0].texcoords[1] = 0.0f;
        Assets::packColor(color1, vert[iv + 0].color);
        Assets::packColor(glow1,  vert[iv + 0].glow);

        vert[iv + 1].position     = p - width1 * v;
        vert[iv + 1].texcoords[0] = vert[iv + 0].texcoords[0];
        vert[iv + 1].texcoords[1] = 1.0f;
        vert[iv + 1].colors32     = vert[iv + 0].colors32;
        vert[iv + 1].glow32       = vert[iv + 0].glow32;

        LM_ASSERT(iv < Core::UINT16_MAX);

        if(ii < positions.size() - 1)
        {
            ind.push_back(iv + 0);
            ind.push_back(iv + 2);
            ind.push_back(iv + 1);

            ind.push_back(iv + 1);
            ind.push_back(iv + 2);
            ind.push_back(iv + 3);
        }

        lastV = v;
        iv += 2;
    }

    LM_ASSERT(iv == vert.size());
}
//-----------------------------------------------------------------------------
}
