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
#include <Core/Math/Noise.h>

namespace Core
{
static int32 absmod(int32 x, int32 n)
{
    int32 m=x%n;
    return m < 0 ? m+n : m;
}

//-----------------------------------------------------------------------------
INoise1D::INoise1D()
{
    initFBMData();
}
//-----------------------------------------------------------------------------
INoise1D::~INoise1D()
{
}
//-----------------------------------------------------------------------------
void INoise1D::initFBMData(const float lacunarity)
{
    _lacunarity = lacunarity;

    float f = 1.f;
    for(int32 i=0; i<MAX_OCTAVE; i++)
    {
        //exponent[i] = powf(f, -H);
        _exponent[i] = 1.0f / f;
        f *= _lacunarity;
    }
}
//-----------------------------------------------------------------------------
float INoise1D::getFBM(const float f, float octaves) const
{
    float tf;

    double value = 0;
    int i;

    tf = f;

    for(i=0; i<(int)octaves; i++)
    {
        value += (getValue(tf)) * _exponent[i];
        tf *= _lacunarity;
    }

    octaves -= (int)octaves;
    if(octaves > 1e-6f)
        value += (double)(octaves * getValue(tf)) * _exponent[i];

    return clamp(-1.f, 1.f, (float)value);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
INoise2D::INoise2D()
{
    initFBMData();
}
//-----------------------------------------------------------------------------
INoise2D::~INoise2D()
{
}
//-----------------------------------------------------------------------------
void INoise2D::initFBMData(const float lacunarity)
{
    _lacunarity = lacunarity;

    float f = 1.f;
    for(int32 i=0; i<MAX_OCTAVE; i++)
    {
        //exponent[i] = powf(f, -H);
        _exponent[i] = 1.0f / f;
        f *= _lacunarity;
    }
}
//-----------------------------------------------------------------------------
float INoise2D::getFBM(const Vector2f& f, float octaves) const
{
    Vector2f tf;

    double value = 0;
    int i;

    tf.x = f.x;
    tf.y = f.y;

    for(i=0; i<(int)octaves; i++)
    {
        value += (double)(getValue(tf)) * _exponent[i];
        tf.x *= _lacunarity;
        tf.y *= _lacunarity;
    }

    octaves -= (int)octaves;
    if(octaves > 1e-6f)
        value += (double)(octaves * getValue(tf)) * _exponent[i];

    return clamp(-1.f, 1.f, (float)value);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
INoise3D::INoise3D()
{
    initFBMData();
}
//-----------------------------------------------------------------------------
INoise3D::~INoise3D()
{
}
//-----------------------------------------------------------------------------
void INoise3D::initFBMData(const float lacunarity)
{
    _lacunarity = lacunarity;

    float f = 1.f;
    for(int32 i=0; i<MAX_OCTAVE; i++)
    {
        //exponent[i] = powf(f, -H);
        _exponent[i] = 1.0f / f;
        f *= _lacunarity;
    }
}
//-----------------------------------------------------------------------------
float INoise3D::getFBM(const Vector3f& f, float octaves) const
{
    Vector3f tf;

    double value = 0;
    int i,j;

    for (i=0; i < 3; i++)
    {
        tf[i]=f[i];
    }

    for(i=0; i<(int)octaves; i++)
    {
        value += (double)(getValue(tf)) * _exponent[i];
        for (j=0; j < 3; j++) tf[j] *= _lacunarity;
    }

    octaves -= (int)octaves;
    if(octaves > 1e-6f)
        value += (double)(octaves * getValue(tf)) * _exponent[i];

    return clamp(-1.f, 1.f, (float)value);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
void waveletNoise_downSample(const int32 tileDimSize, float *dataIn, float *dataOut, const int32 stride)
{
    //on utilise lun ondelette BSpline pour obtenir l'echelle inferieur
    static float acoeffs[32]= {
    0.000334f, -0.001528f, 0.000410f, 0.003545f, -0.000938f, -0.008233f, 0.002172f, 0.019120f,
    -0.005040f,-0.044412f, 0.011655f, 0.103311f, -0.025936f, -0.243780f, 0.033979f, 0.655340f,
     0.655340f, 0.033979f,-0.243780f,-0.025936f,  0.103311f,  0.011655f,-0.044412f,-0.005040f,
    0.019120f,  0.002172f,-0.008233f,-0.000938f,  0.003546f,  0.000410f,-0.001528f, 0.000334f};
    static float *a = &acoeffs[16];

    for (int32 ii=0; ii<tileDimSize/2; ii++)
    {
        dataOut[ii*stride]=0;
        for (int32 icoef=2*ii-16; icoef<2*ii+16; icoef++)
        {
            dataOut[ii*stride] += a[icoef-2*ii]* dataIn[absmod(icoef,tileDimSize) * stride];
        }
    }
}
//-----------------------------------------------------------------------------
void waveletNoise_upSample(const int32 tileDimSize, float *dataIn, float *dataOut, const int32 stride)
{
    static float pcoeffs[4]= {0.25f, 0.75f, 0.75f, 0.25f};
    static float *p = &pcoeffs[2];

    for (int32 ii=0; ii<tileDimSize; ii++)
    {
        dataOut[ii*stride]=0;
        for (int32 iCoef=ii/2; iCoef<ii/2+1; iCoef++)
        {
            dataOut[ii*stride] += p[ii-2*iCoef]* dataIn[absmod(iCoef,tileDimSize/2) * stride];
        }
    }
}

//-----------------------------------------------------------------------------
void waveletNoise_initialiseTile(const int64 seed, const int32 tileDimSize, float*& tile, const ERandomDistribution type, const RandomDistributionfData* dData)
{
    RandDistributionf noiseGenerator (type, seed, -1.f, 1.f, dData);

    int32 i;
    delete tile;

    int32 nbTile = tileDimSize*tileDimSize*tileDimSize; //on prevoit au max 3 dimensions
    tile = new float [nbTile];
    float *temp1 = new float [nbTile];
    float *temp2 = new float [nbTile];

    for (int32 ii=0; ii<nbTile; ii++)
    {
        tile[ii] = noiseGenerator();
    }

    //on effectue la decomposition/recomposition dans les trois plans (YZ, XZ, XY)
    //YZ
    for (int32 iY=0; iY<tileDimSize; iY++)
    {
        for (int32 iZ=0; iZ<tileDimSize; iZ++)
        {
            i = iY * tileDimSize + iZ * tileDimSize * tileDimSize;
            waveletNoise_downSample(tileDimSize, &tile[i], &temp1[i], 1);
            waveletNoise_upSample(tileDimSize, &temp1[i], &temp2[i], 1);
        }
    }

    //XZ
    for (int32 iX=0; iX<tileDimSize; iX++)
    {
        for (int32 iZ=0; iZ<tileDimSize; iZ++)
        {
            i = iX + iZ * tileDimSize * tileDimSize;
            waveletNoise_downSample(tileDimSize, &temp2[i], &temp1[i], tileDimSize);
            waveletNoise_upSample(tileDimSize, &temp1[i], &temp2[i], tileDimSize);
        }
    }

    //XY
    for (int32 iX=0; iX<tileDimSize; iX++)
    {
        for (int32 iY=0; iY<tileDimSize; iY++)
        {
            i = iX + iY * tileDimSize;
            waveletNoise_downSample(tileDimSize, &temp2[i], &temp1[i], tileDimSize * tileDimSize);
            waveletNoise_upSample(tileDimSize, &temp1[i], &temp2[i], tileDimSize * tileDimSize);
        }
    }

    //on soustrait la decomposition/reconposition => on a les coef de la decomposition
    for (int32 ii=0; ii<nbTile; ii++)
    {
        tile[ii] -= temp2[ii];
    }

    //
    int32 offset = tileDimSize/2;
    if ( (offset & 1) == 0 ) offset++;

    i=0;
    for (int32 iX=0; iX < tileDimSize; iX++ )
    {
        for (int32 iY=0; iY < tileDimSize; iY++ )
        {
            for (int32 iZ=0; iZ < tileDimSize; iZ++ )
            {
                temp1[i++] = tile[ absmod(iX+offset,tileDimSize)
                    + absmod(iY+offset,tileDimSize)*tileDimSize
                    + absmod(iZ+offset,tileDimSize)*tileDimSize*tileDimSize
                    ];
            }
        }
    }

    //
    for (i=0; i<nbTile; i++)
    {
        tile[i] += temp1[i];
    }

    delete temp1;
    delete temp2;
}
//-----------------------------------------------------------------------------
NoiseWavelet1Df::NoiseWavelet1Df(const int32 tileDimSize, const ERandomDistribution type, const RandomDistributionfData* dData, const int64 seed):
    _tile(NULL),
    _tileDimSize(tileDimSize)
{
    waveletNoise_initialiseTile(seed, _tileDimSize, _tile, type, dData);
}
//-----------------------------------------------------------------------------
NoiseWavelet1Df::~NoiseWavelet1Df()
{
    delete _tile;
}
//-----------------------------------------------------------------------------
float NoiseWavelet1Df::getValue(const float x, const float inter) const
{
    float ret = 0.f;
    float weight;
    float tmp;

    float fct = 1.f;

    fct = (inter > 1/(float)_tileDimSize) ? _tileDimSize*inter : 1;

    Vector3f p;
    p[0] = x*fct;
    p[1] = 0;
    p[2] = 0;

    Vector3f mid;
    mid[0] = L_ceil(p[0]-0.5f);
    mid[1] = L_ceil(p[1]-0.5f);
    mid[2] = L_ceil(p[2]-0.5f);

    Vector3f t;
    t[0] = mid[0] - (p[0]-0.5f);
    t[1] = mid[1] - (p[1]-0.5f);
    t[2] = mid[2] - (p[2]-0.5f);

    mid[0] -= 1;
    mid[1] -= 1;
    mid[2] -= 1;

    //les coef BSpline de la recomposition
    Matrix3f w;
    w._00 = t.x * t.x * 0.5f;
    w._01 = 0.5f + t.x - t.x * t.x;
    w._02 = 0.5f - t.x + 0.5f * t.x * t.x;

    w._10 = t.y * t.y * 0.5f;
    w._11 = 0.5f + t.y - t.y * t.y;
    w._12 = 0.5f - t.y + 0.5f * t.y * t.y;

    w._20 = t.z * t.z * 0.5f;
    w._21 = 0.5f + t.z - t.z * t.z;
    w._22 = 0.5f - t.z + 0.5f * t.z * t.z;

    //on calcul la valeur du bruit
    for (int32 iZ=0; iZ<3; iZ++)
    {
        for(int32 iY=0; iY<3; iY++)
        {
            for (int32 iX=0; iX<3; iX++)
            {
                tmp = _tile[absmod(static_cast<int32>(mid.x + iX), _tileDimSize) +
                          absmod(static_cast<int32>(mid.y + iY), _tileDimSize) * _tileDimSize +
                          absmod(static_cast<int32>(mid.z + iZ), _tileDimSize) * _tileDimSize * _tileDimSize];
                weight = w._line[0][iX] * w._line[1][iY] * w._line[2][iZ];

                ret += tmp * weight;
            }
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
NoiseWavelet2Df::NoiseWavelet2Df(const int32 tileDimSize, const ERandomDistribution type, const RandomDistributionfData* dData, const int64 seed):
    _tile(NULL),
    _tileDimSize(tileDimSize)
{
    waveletNoise_initialiseTile(seed, _tileDimSize, _tile, type, dData);
}
//-----------------------------------------------------------------------------
NoiseWavelet2Df::~NoiseWavelet2Df()
{
    delete _tile;
}
//-----------------------------------------------------------------------------
float NoiseWavelet2Df::getValue(const Vector2f& f, const float inter) const
{
    float ret = 0.f;
    float weight;
    float tmp;

    float fct = 1.f;

    fct = (inter > 1/(float)_tileDimSize) ? _tileDimSize*inter : 1;

    Vector3f p;
    p[0] = f.x*fct;
    p[1] = f.y*fct;
    p[2] = 0;

    Vector3f mid;
    mid[0] = L_ceil(p[0]-0.5f);
    mid[1] = L_ceil(p[1]-0.5f);
    mid[2] = L_ceil(p[2]-0.5f);

    Vector3f t;
    t[0] = mid[0] - (p[0]-0.5f);
    t[1] = mid[1] - (p[1]-0.5f);
    t[2] = mid[2] - (p[2]-0.5f);

    mid[0] -= 1;
    mid[1] -= 1;
    mid[2] -= 1;

    //les coef BSpline de la recomposition
    Matrix3f w;
    w._00 = t.x * t.x * 0.5f;
    w._01 = 0.5f + t.x - t.x * t.x;
    w._02 = 0.5f - t.x + 0.5f * t.x * t.x;

    w._10 = t.y * t.y * 0.5f;
    w._11 = 0.5f + t.y - t.y * t.y;
    w._12 = 0.5f - t.y + 0.5f * t.y * t.y;

    w._20 = t.z * t.z * 0.5f;
    w._21 = 0.5f + t.z - t.z * t.z;
    w._22 = 0.5f - t.z + 0.5f * t.z * t.z;

    //on calcul la valeur du bruit
    for (int32 iZ=0; iZ<3; iZ++)
    {
        for(int32 iY=0; iY<3; iY++)
        {
            for (int32 iX=0; iX<3; iX++)
            {
                tmp = _tile[absmod(static_cast<int32>(mid.x + iX), _tileDimSize) +
                          absmod(static_cast<int32>(mid.y + iY), _tileDimSize) * _tileDimSize +
                          absmod(static_cast<int32>(mid.z + iZ), _tileDimSize) * _tileDimSize * _tileDimSize];
                weight = w._line[0][iX] * w._line[1][iY] * w._line[2][iZ];

                ret += tmp * weight;
            }
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
NoiseWavelet3Df::NoiseWavelet3Df(const int32 tileDimSize, const ERandomDistribution type, const RandomDistributionfData* dData, const int64 seed) :
_tile(NULL),
_tileDimSize(tileDimSize)
{
    waveletNoise_initialiseTile(seed, _tileDimSize, _tile, type, dData);
}
//-----------------------------------------------------------------------------
NoiseWavelet3Df::~NoiseWavelet3Df()
{
    delete _tile;
}
//-----------------------------------------------------------------------------
float NoiseWavelet3Df::getValue(const Vector3f& f, const float inter) const
{
    float ret = 0.f;
    float weight;
    float tmp;

    float fct = 1.f;
    Vector3f p;

    fct = (inter > 1/(float)_tileDimSize) ? _tileDimSize*inter : 1;

    p[0] = f[0]*fct;
    p[1] = f[1]*fct;
    p[2] = f[2]*fct;

    Vector3f mid;
    mid[0] = L_ceil(p[0]-0.5f);
    mid[1] = L_ceil(p[1]-0.5f);
    mid[2] = L_ceil(p[2]-0.5f);

    Vector3f t;
    t[0] = mid[0] - (p[0]-0.5f);
    t[1] = mid[1] - (p[1]-0.5f);
    t[2] = mid[2] - (p[2]-0.5f);

    mid[0] -= 1;
    mid[1] -= 1;
    mid[2] -= 1;

    //les coef BSpline de la recomposition
    Matrix3f w;
    w._00 = t.x * t.x * 0.5f;
    w._01 = 0.5f + t.x - t.x * t.x;
    w._02 = 0.5f - t.x + 0.5f * t.x * t.x;

    w._10 = t.y * t.y * 0.5f;
    w._11 = 0.5f + t.y - t.y * t.y;
    w._12 = 0.5f - t.y + 0.5f * t.y * t.y;

    w._20 = t.z * t.z * 0.5f;
    w._21 = 0.5f + t.z - t.z * t.z;
    w._22 = 0.5f - t.z + 0.5f * t.z * t.z;

    //on calcul la valeur du bruit
    for (int32 iZ=0; iZ<3; iZ++)
    {
        for(int32 iY=0; iY<3; iY++)
        {
            for (int32 iX=0; iX<3; iX++)
            {
                tmp = _tile[absmod(static_cast<int32>(mid.x + iX), _tileDimSize) +
                          absmod(static_cast<int32>(mid.y + iY), _tileDimSize) * _tileDimSize +
                          absmod(static_cast<int32>(mid.z + iZ), _tileDimSize) * _tileDimSize * _tileDimSize];
                weight = w._line[0][iX] * w._line[1][iY] * w._line[2][iZ];

                ret += tmp * weight;
            }
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
} //namespace Core
