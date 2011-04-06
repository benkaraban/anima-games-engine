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
#ifndef CORE_MATH_NOISE_H_
#define CORE_MATH_NOISE_H_

#include <Core/Math/GeometryComputations.h>
#include <Core/Math/Random.h>
#include <Core/Math/Vector.h>

namespace Core
{


//-----------------------------------------------------------------------------
/**
 * Les interfaces permettant de gerer un bruit cohérent pour des données 1D, 2D, et 3D
 */
static const int MAX_OCTAVE = 128;
class LM_API_COR INoise1D
{
protected:
    INoise1D();
public:
    /**
     * retroune la valeur du bruit suivant la position x
     * (note : la position retournera toujours la même valeur)
     * x : compris entre [0, 1]
     *     ou supérieur si l'on souhaite obtenir un bruit périodique (pour une période de 2 [0,2]
     * inter :valeur d'interpolation des donnée du bruit compris entre [0, 1]
     *        pour un bruit très lisse inter = 0, pour de nombreux détail inter =1
     */
    //inter est compris entre [0, 1]
    virtual float getValue(const float x, const float inter=0) const = 0;

    /**
     * fractalBrownian:
     * pour obtenir un bruit "détaillé" sur une grande étendue la fonction
     * getValue necessite d'initialisée une trop grande zone mémoire.
     * Pour éviter cela on génère un bruit avec la fonction getValue
     * que l'on va perturber par différent appel a getValue.
     * différente méthode existe ici il s'agit du mouvement Brownien.
     *
     * retroune la valeur du bruit suivant la position x
     * (note : la position retournera toujours la même valeur)
     * x : compris entre [0, 1]
     * octaves :nombre d'iteration (max = 128)
     */
    float getFBM(const float x, float octaves = 4.f) const;
    /**
     * initialise les donnees internes necessaire à la fonction getFBM
     * lacunarity : fréquence séparant deux octaves
     */
    void initFBMData(const float lacunarity = 2.f);

public:
    virtual ~INoise1D();

protected:
    float _lacunarity;
    float _exponent [MAX_OCTAVE];
};
//-----------------------------------------------------------------------------
class LM_API_COR INoise2D
{
protected:
    INoise2D();
public:
    virtual float getValue(const Vector2f& p, const float inter=0) const = 0;

    float getFBM(const Vector2f& p, float octaves = 4.f) const;
    void initFBMData(const float lacunarity = 2.f);

public:
    virtual ~INoise2D();

protected:
    float _lacunarity;
    float _exponent [MAX_OCTAVE];
};
//-----------------------------------------------------------------------------
class LM_API_COR INoise3D
{
protected:
    INoise3D();

public:
    virtual float getValue(const Vector3f& p, const float inter=0) const = 0;

    float getFBM(const Vector3f& p, float octaves = 4.f) const;
    void initFBMData(const float lacunarity = 2.f);

public:
    virtual ~INoise3D();

protected:
    float _lacunarity;
    float _exponent [MAX_OCTAVE];
};
//-----------------------------------------------------------------------------
/**
 *  Wavelet Noise
 *  Aussi simple qu'un bruit de Perlin, un peu plus lent, et meilleur qualité (evite l'aliasing des hautes frequences)
 *  publi : http://graphics.pixar.com/library/WaveletNoise/paper.pdf
 */
class LM_API_COR NoiseWavelet1Df : public INoise1D
{
public:
    NoiseWavelet1Df(const int32 tileDimSize = 32, const ERandomDistribution type = RD_NORMAL, const RandomDistributionfData* dData = NULL, const int64 seed = 789548);
    virtual ~NoiseWavelet1Df();
    virtual float getValue(const float x, const float inter=0) const;


private:
    float* _tile;
    int32  _tileDimSize;
};
//-----------------------------------------------------------------------------
class LM_API_COR NoiseWavelet2Df : public INoise2D
{
public:
    NoiseWavelet2Df(const int32 tileDimSize = 32, const ERandomDistribution type = RD_NORMAL, const RandomDistributionfData* dData = NULL, const int64 seed = 789548);
    virtual ~NoiseWavelet2Df();
    virtual float getValue(const Vector2f& p, const float inter=0) const;

private:
    float* _tile;
    int32  _tileDimSize;
};
//-----------------------------------------------------------------------------
class LM_API_COR NoiseWavelet3Df : public INoise3D
{
public:
    NoiseWavelet3Df(const int32 tileDimSize = 32, const ERandomDistribution type = RD_NORMAL, const RandomDistributionfData* dData = NULL, const int64 seed = 789548);
    virtual ~NoiseWavelet3Df();
    virtual float getValue(const Vector3f& p, const float inter=0) const;

private:
    float* _tile;
    int32  _tileDimSize;
};
//-----------------------------------------------------------------------------
} //namespace Core


#endif //CORE_MATH_NOISE_H_
