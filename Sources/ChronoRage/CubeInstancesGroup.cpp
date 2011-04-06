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
#include <CubeInstancesGroup.h>

namespace ChronoRage
{
CubeInstancesGroup::CubeInstancesGroup(const Ptr<Renderer::IFreeForm> & pFreeForm, float size) : 
    _pFreeForm(pFreeForm),
    _size(size)
{
    _pFreeForm->setPrecomputedAABox(Core::AABoxf(Core::Vector3f(-1000.0f, -1000.0f, -1000.0f), Core::Vector3f(1000.0f, 1000.0f, 1000.0f)));
}

inline uint32 quickPackColor(const Core::Vector4f & color)
{
    uint32 dwR = color.r >= 1.0f ? 0xff : color.r <= 0.0f ? 0x00 : (uint32) (color.r * 255.0f + 0.5f);
    uint32 dwG = color.g >= 1.0f ? 0xff : color.g <= 0.0f ? 0x00 : (uint32) (color.g * 255.0f + 0.5f);
    uint32 dwB = color.b >= 1.0f ? 0xff : color.b <= 0.0f ? 0x00 : (uint32) (color.b * 255.0f + 0.5f);
    uint32 dwA = color.a >= 1.0f ? 0xff : color.a <= 0.0f ? 0x00 : (uint32) (color.a * 255.0f + 0.5f);

    return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;
}

void CubeInstancesGroup::endUpdate()
{
    Renderer::FreeFormData & data = _pFreeForm->beginUpdate();

    Core::List<Renderer::FreeFormVertex>   & vertices = data.vertices;
    Core::List<uint16>                     & indices = data.indices;

    vertices.resize(_instances.size() * CUBE_VERTEX_COUNT);

    int32 oldIndSize = indices.size();
    int32 newIndSize = _instances.size() * CUBE_INDEX_COUNT;

    indices.resize(newIndSize);

    if(oldIndSize < newIndSize)
    {
        int32 jj = 0;

        for(int32 ii=0; ii < newIndSize; ii+= CUBE_INDEX_COUNT)
        {
            indices[ii +  0] = jj + 0; indices[ii +  1] = jj + 3; indices[ii +  2] = jj + 1;
            indices[ii +  3] = jj + 1; indices[ii +  4] = jj + 3; indices[ii +  5] = jj + 2;

            indices[ii +  6] = jj + 4; indices[ii +  7] = jj + 7; indices[ii +  8] = jj + 5;
            indices[ii +  9] = jj + 5; indices[ii + 10] = jj + 7; indices[ii + 11] = jj + 6;

            indices[ii + 12] = jj + 8; indices[ii + 13] = jj + 11; indices[ii + 14] = jj + 9;
            indices[ii + 15] = jj + 9; indices[ii + 16] = jj + 11; indices[ii + 17] = jj + 10;

            indices[ii + 18] = jj + 12; indices[ii + 19] = jj + 13; indices[ii + 20] = jj + 15;
            indices[ii + 21] = jj + 13; indices[ii + 22] = jj + 14; indices[ii + 23] = jj + 15;

            indices[ii + 24] = jj + 16; indices[ii + 25] = jj + 17; indices[ii + 26] = jj + 19;
            indices[ii + 27] = jj + 17; indices[ii + 28] = jj + 18; indices[ii + 29] = jj + 19;

            indices[ii + 30] = jj + 20; indices[ii + 31] = jj + 23; indices[ii + 32] = jj + 21;
            indices[ii + 33] = jj + 21; indices[ii + 34] = jj + 23; indices[ii + 35] = jj + 22;

            jj += CUBE_VERTEX_COUNT;
       }
    }

    int32 ii = 0;

    float * pData1 = NULL;
    float * pData2 = NULL;

    if(vertices.size() > 0)
    {
        pData1 = (float*)&vertices[0];
        pData2 = pData1 + 4;
    }

    __m128 rhs = _mm_set1_ps(0.5f * _size);

    for(int32 iInst=0; iInst < _instances.size(); iInst++)
    {
        CubeInstance & inst = _instances[iInst];
        float * pMat = &(inst.worldMat._00);
        pMat[ 3] = 0.0f;
        pMat[ 7] = 0.0f;
        pMat[11] = 0.0f;
        pMat[15] = 0.0f;

        union { uint32 color32; float colorFloat; };
        union { uint32 glow32; float glowFloat; };

        color32 = quickPackColor(inst.color);
        glow32 = quickPackColor(inst.glow);

        __m128 r0       = _mm_set_ss(0.0f);

        __m128 rright   = _mm_load_ps(&pMat[ 0]);
        __m128 rup      = _mm_load_ps(&pMat[ 4]);
        __m128 rback    = _mm_load_ps(&pMat[ 8]);
        __m128 rcenter  = _mm_load_ps(&pMat[12]);

        __m128 rdown    = _mm_sub_ps(r0, rup);
        __m128 rleft    = _mm_sub_ps(r0, rright);
        __m128 rfront   = _mm_sub_ps(r0, rback);

        __m128 rcol = _mm_load_ss(&colorFloat);
        rcol = _mm_shuffle_ps(rcol, rcol, _MM_SHUFFLE(0, 1, 2, 3));

        __m128 rglow = _mm_load_ss(&glowFloat);
        rglow = _mm_shuffle_ps(rglow, rglow, _MM_SHUFFLE(0, 1, 2, 3));

        __m128 rf1 = _mm_or_ps(rcol, _mm_add_ps(rcenter, _mm_mul_ps(rhs, _mm_add_ps(rleft,  _mm_add_ps(rup,     rfront)))));
        __m128 rf2 = _mm_or_ps(rcol, _mm_add_ps(rcenter, _mm_mul_ps(rhs, _mm_add_ps(rright, _mm_add_ps(rup,     rfront)))));
        __m128 rf3 = _mm_or_ps(rcol, _mm_add_ps(rcenter, _mm_mul_ps(rhs, _mm_add_ps(rright, _mm_add_ps(rdown,   rfront)))));
        __m128 rf4 = _mm_or_ps(rcol, _mm_add_ps(rcenter, _mm_mul_ps(rhs, _mm_add_ps(rleft,  _mm_add_ps(rdown,   rfront)))));

        __m128 rb1 = _mm_or_ps(rcol, _mm_add_ps(rcenter, _mm_mul_ps(rhs, _mm_add_ps(rleft,  _mm_add_ps(rup,     rback)))));
        __m128 rb2 = _mm_or_ps(rcol, _mm_add_ps(rcenter, _mm_mul_ps(rhs, _mm_add_ps(rright, _mm_add_ps(rup,     rback)))));
        __m128 rb3 = _mm_or_ps(rcol, _mm_add_ps(rcenter, _mm_mul_ps(rhs, _mm_add_ps(rright, _mm_add_ps(rdown,   rback)))));
        __m128 rb4 = _mm_or_ps(rcol, _mm_add_ps(rcenter, _mm_mul_ps(rhs, _mm_add_ps(rleft,  _mm_add_ps(rdown,   rback)))));

        rup = _mm_add_ps(rglow, rup);
        rdown = _mm_add_ps(rglow, rdown);
        rleft = _mm_add_ps(rglow, rleft);
        rright = _mm_add_ps(rglow, rright);
        rfront = _mm_add_ps(rglow, rfront);
        rback = _mm_add_ps(rglow, rback);

        //     B1     B2
        //     +------+
        //    /      /|
        //F1 +------+ | 
        //   |    F2| +B3
        //   |      |/
        //   +------+
        //  F4     F3
        // STORE POS + COLOR; STORE NORMAL + GLOW
        {
            _mm_stream_ps(pData1, rf1); pData1 += 8; _mm_stream_ps(pData2, rup); pData2 += 8;
            _mm_stream_ps(pData1, rf2); pData1 += 8; _mm_stream_ps(pData2, rup); pData2 += 8;
            _mm_stream_ps(pData1, rb2); pData1 += 8; _mm_stream_ps(pData2, rup); pData2 += 8;
            _mm_stream_ps(pData1, rb1); pData1 += 8; _mm_stream_ps(pData2, rup); pData2 += 8;
        }
        {
            _mm_stream_ps(pData1, rf1); pData1 += 8; _mm_stream_ps(pData2, rfront); pData2 += 8;
            _mm_stream_ps(pData1, rf4); pData1 += 8; _mm_stream_ps(pData2, rfront); pData2 += 8;
            _mm_stream_ps(pData1, rf3); pData1 += 8; _mm_stream_ps(pData2, rfront); pData2 += 8;
            _mm_stream_ps(pData1, rf2); pData1 += 8; _mm_stream_ps(pData2, rfront); pData2 += 8;
        }
        {
            _mm_stream_ps(pData1, rf2); pData1 += 8; _mm_stream_ps(pData2, rright); pData2 += 8;
            _mm_stream_ps(pData1, rf3); pData1 += 8; _mm_stream_ps(pData2, rright); pData2 += 8;
            _mm_stream_ps(pData1, rb3); pData1 += 8; _mm_stream_ps(pData2, rright); pData2 += 8;
            _mm_stream_ps(pData1, rb2); pData1 += 8; _mm_stream_ps(pData2, rright); pData2 += 8;
        }
        {
            _mm_stream_ps(pData1, rb2); pData1 += 8; _mm_stream_ps(pData2, rback); pData2 += 8;
            _mm_stream_ps(pData1, rb1); pData1 += 8; _mm_stream_ps(pData2, rback); pData2 += 8;
            _mm_stream_ps(pData1, rb4); pData1 += 8; _mm_stream_ps(pData2, rback); pData2 += 8;
            _mm_stream_ps(pData1, rb3); pData1 += 8; _mm_stream_ps(pData2, rback); pData2 += 8;
        }
        {
            _mm_stream_ps(pData1, rf1); pData1 += 8; _mm_stream_ps(pData2, rleft); pData2 += 8;
            _mm_stream_ps(pData1, rf4); pData1 += 8; _mm_stream_ps(pData2, rleft); pData2 += 8;
            _mm_stream_ps(pData1, rb4); pData1 += 8; _mm_stream_ps(pData2, rleft); pData2 += 8;
            _mm_stream_ps(pData1, rb1); pData1 += 8; _mm_stream_ps(pData2, rleft); pData2 += 8;
        }
        {
            _mm_stream_ps(pData1, rf3); pData1 += 8; _mm_stream_ps(pData2, rdown); pData2 += 8;
            _mm_stream_ps(pData1, rf4); pData1 += 8; _mm_stream_ps(pData2, rdown); pData2 += 8;
            _mm_stream_ps(pData1, rb4); pData1 += 8; _mm_stream_ps(pData2, rdown); pData2 += 8;
            _mm_stream_ps(pData1, rb3); pData1 += 8; _mm_stream_ps(pData2, rdown); pData2 += 8;
        }
    }

    _pFreeForm->endUpdate();
}

}
