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

#ifndef CORE_MATH_INTERPOLATE_H_
#define CORE_MATH_INTERPOLATE_H_

#include <Core/Math/MathTools.h>

namespace Core
{
/**
 * Cet énuméré permet de choisir le type d'interpolation utilisée pour interpoler
 * une valeur entre deux autres.
 *
 * De manière concrête, voilà ce que ça donne pour un mouvement (translation/rotation ou autre) :
 *
 * INTER_LINEAR : Un mouvement de ce type s'effectue à une vitesse constante.
 *
 * INTER_FADE_IN : Le mouvement commence avec une vitesse nulle et accélère
 * tout au long du mouvement.
 *
 * INTER_FADE_OUT : Le mouvement commence avec sa vitesse maximale et ralentit
 * de manière à ce qu'à la fin du mouvement, l'accélération soit nulle.
 *
 * INTER_FADE_IN_OUT : Le mouvement commence avec une vitesse nulle, accélère
 * jusqu'à sa vitesse maximale, à mi distance, puis ralentit de manière
 * à obtenir une accélération nulle à la fin du mouvement.
 */
LM_ENUM_4(EInterpolateFunc,
          INTER_LINEAR,
          INTER_FADE_IN,
          INTER_FADE_OUT,
          INTER_FADE_IN_OUT);

LM_API_COR float interpolate(float alpha, EInterpolateFunc func);

inline float lerp(float from, float to, float amount)
{
    return from + (to - from) * amount;
}

}

#endif
