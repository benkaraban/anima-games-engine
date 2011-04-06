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
#ifndef DECOMPOSEMATRIX_H_
#define DECOMPOSEMATRIX_H_

namespace Core
{

/*
 * Décompose une matrice affine en translation, rotation, scale, scaleOrientation
 * Le code est repris de la publication : Matrix Animation and Polar Decomposition (Ken Shoemake et Tom Duff 1992)
 * disponible dans Graphics Gems IV
 * Note : la matrice est définie de manière à transofrmer un vecteur colonne dans un repère main droite.
 */

namespace MathDetail
{

typedef struct
{
    float x, y, z, w;
} Quat; // Quaternion

enum QuatPart
{   X, Y, Z, W};

typedef Quat HVect; // Homogeneous 3D vector

typedef float HMatrix[4][4]; // Right-handed, for column vectors

typedef struct
{
    HVect t; // Translation components
    Quat q; // Essential rotation
    Quat u; // Stretch rotation
    HVect k; // Stretch factors
    float f; // Sign of determinant
} AffineParts;

LM_API_COR float polar_decomp(HMatrix M, HMatrix Q, HMatrix S);
LM_API_COR HVect spect_decomp(HMatrix S, HMatrix U);
LM_API_COR Quat snuggle(Quat q, HVect *k);
LM_API_COR void decomp_affine(HMatrix A, AffineParts *parts);
LM_API_COR void invert_affine(AffineParts *parts, AffineParts *inverse);

}// MathDetail

}//Core

#endif /*DECOMPOSEMATRIX_H_*/
