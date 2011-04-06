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
#ifndef CORE_RANDOM_H_
#define CORE_RANDOM_H_

#include <Core/Types.h>
#include <Core/Enum.h>

namespace Core
{
//-----------------------------------------------------------------------------
LM_API_COR double drand();
LM_API_COR double drand(const double min, const double max);
//-----------------------------------------------------------------------------
LM_API_COR float frand();
LM_API_COR float frand(const float min, const float max);
//-----------------------------------------------------------------------------
LM_API_COR int32 irand();
LM_API_COR int32 irand(const int32 min, const int32 max );
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

LM_ENUM_3 (ERandomDistribution,
           RD_UNIFORM,
           RD_NORMAL,   //=> gaussienne
           RD_GAUSSIAN)


//RandomDistributionfData : stock les parametres liés à la distribution du générateur
union LM_API_COR RandomDistributionfData
{
    struct UniformData {} uniform;  //pas de parametre
    struct NormalData { } normal;   //pas de parametre
    struct GaussianData 
    { 
        float mean;
        float sigma;
    } gaussian;
};

class LM_API_COR RandDistributionf
{
public:
    /**
     * Si dData == NULL le constructeur utilise les valeur par default
     * distribution uniform : pas de parametre
     * distribution normal : pas de parametre
     */
    RandDistributionf(const ERandomDistribution type, const int64 seed, const float min, const float max, const RandomDistributionfData* dData = NULL);
    ~RandDistributionf();

    float operator() ();

protected:
    void* _pRand;
    RandomDistributionfData _dData;
    ERandomDistribution _type;
    float _min;
    float _max;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} //namespace Core

#endif /*RANDOMGENERATOR_H_*/
