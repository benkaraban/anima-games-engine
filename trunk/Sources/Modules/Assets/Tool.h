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
#ifndef ASSETS_TOOL_H_
#define ASSETS_TOOL_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/IProgressObs.h>
#include <Core/IO/Streams.h>
#include <Core/Math/Vector.h>
#include <Assets/VerticeSet.h>

namespace Assets
{
LM_API_ASS void removeUnusedVertices(Assets::VerticeSet & vset);
LM_API_ASS void removeDegeneratedTriangles(Assets::VerticeSet & vset);

/**
 * => calcul le rep�re TBN : tangente, bitangent, normal utilise pour l'eclairage par pixel
 * ATTENTION !! pour le moment le nombre de vertex du vset en sortie peut etre augmente, si le nombre de face 
 * reste identique l'ordre des vertex peut etre change. 
 *computeNormals == true : les normales de vset sont effacees, et recalculees
 *computeNormals == false : les normales de vset sont preservees et utilisees pour le calcul du repere TBN
 *minNormalsCreaseCosAngle - the minimum cosine of the angle between normals
 *							 so that they are allowed to be smoothed together
 *							 ranges between -1.0 and +1.0
 *							 this is ignored if computeNormals is set to false
 *							 
 *
 *minTangentsCreaseCosAngle - the minimum cosine of the angle between tangents
 *							  so that they are allowed to be smoothed together
 *							  ranges between -1.0 and +1.0
 *
 *minBinormalsCreaseCosAngle - the minimum cosine of the angle between binormals
 *							   so that they are allowed to be smoothed together
 *		 					   ranges between -1.0 and +1.0
 *
 *weightNormalsByArea - an ammount to blend the normalized face normal, and the 
 *						unnormalized face normal together.  Thus weighting the
 *						normal by the face area by a given ammount
 *						ranges between 0.0 and +1.0
 *						0.0 means use the normalized face normals (not weighted by area)
 *						1.0 means use the unnormalized face normal(weighted by area)
 *						this is ignored if computeNormals is set to false
 **/
LM_API_ASS void makeTBNBasis(Assets::VerticeSet & vset, const bool computeNormals, const Ptr<Core::IProgressObs> & pObs = null,
  const float minNormalsCreaseCosAngle=0.f, const float minTangentsCreaseCosAngle=0.f, const float minBinormalsCreaseCosAngle=0.f,
  const float weightNormalsByArea=1.f);
  
LM_API_ASS void makeTBN(Assets::VerticeSet & vset);
}  //namespace Assets
    
#endif /*ASSETS_TOOL_H_*/
