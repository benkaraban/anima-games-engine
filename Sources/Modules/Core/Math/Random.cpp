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
#include "Random.h"
#include <Core/Math/MathTools.h>

#include <ctime>
#include <boost/random.hpp>
#include <boost/utility/base_from_member.hpp>

namespace Core
{

LM_ENUM_1 (EUniform_t, UNIFORM)
LM_ENUM_1 (ENormal_t, NORMAL)


template <typename SuperType> class RandomBase_engine : public SuperType
{
public:
    typedef SuperType super_type;

public:
    RandomBase_engine(uint32 seed) :
        super_type(seed)
    {
    }
};

template <typename SuperType> class RandomBase_distribution : public SuperType
{
public:
    typedef SuperType super_type;
    typedef typename super_type::input_type value_type;

public:
    RandomBase_distribution(const value_type minValue, const value_type maxValue) :
        super_type(minValue, maxValue)
    {
    }
};

template <typename Engine, typename Distribution> class Random :
    public boost::base_from_member<RandomBase_engine<Engine> >,
    public boost::base_from_member<RandomBase_distribution<Distribution> >,
    public boost::variate_generator<Engine&, Distribution>
{
public:
    typedef Engine engine_type;
    typedef Distribution distribution_type;
    typedef typename boost::base_from_member<RandomBase_engine<engine_type> > pbase_engine_type;
    typedef typename boost::base_from_member<RandomBase_distribution<distribution_type> > pbase_distribution_type;
    typedef typename boost::variate_generator<engine_type&, distribution_type> super_type;
    typedef typename Distribution::input_type value_type;

public:
    Random(EUniform_t type, uint32 seed, const value_type minValue, const value_type maxValue) :
        pbase_engine_type(seed), pbase_distribution_type(minValue, maxValue),
                super_type(pbase_engine_type::member, pbase_distribution_type::member)
    {
    }

    Random(ENormal_t type, uint32 seed, const value_type mean, const value_type sigma) :
            pbase_engine_type(seed), pbase_distribution_type(mean, sigma),
                    super_type(pbase_engine_type::member, pbase_distribution_type::member)
    {
    }

    ~Random()
    {
    }
};

typedef Random<boost::minstd_rand, boost::uniform_real<double> > RandomDouble;
typedef Random<boost::minstd_rand, boost::uniform_real<float> > RandomFloat;
typedef Random<boost::minstd_rand, boost::uniform_int<int32> > RandomInt32;

static RandomDouble drandom(UNIFORM, static_cast<uint32>(std::time(0)), 0, 1.f);
static RandomFloat frandom(UNIFORM, static_cast<uint32>(std::time(0)), 0, 1.f);
static RandomInt32 irandom(UNIFORM, static_cast<uint32>(std::time(0)), 0, INT32_MAX);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double drand()
{
    return drandom();
}
//-----------------------------------------------------------------------------
double drand(const double min, const double max)
{
    return drandom() * (max-min) + min;
}
//-----------------------------------------------------------------------------
float frand()
{
    return frandom();
}
//-----------------------------------------------------------------------------
float frand(const float min, const float max)
{
    return frandom() * (max-min) + min;
}
//-----------------------------------------------------------------------------
int32 irand()
{
    return irandom();
}
//-----------------------------------------------------------------------------
int32 irand(const int32 min, const int32 max)
{
    int32 range = max - min;

    if(range != 0)
        return min + irandom() % (range + 1);
    else
        return min;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
RandDistributionf::RandDistributionf(const ERandomDistribution type, const int64 seed, const float min, const float max, const RandomDistributionfData* dData):
    _type(type),
    _min(min),
    _max(max)
{
    float mean, sigma;

    //on initialise les parametres
    if (dData==NULL) //parametres par defaut
    {
        switch (_type)
        {
        case RD_UNIFORM:
            //pas de parametre
        break;
        case RD_NORMAL:
            //pas de parametre
        break;
        case RD_GAUSSIAN:
            _dData.gaussian.mean = 0.5f * (max - min);
            _dData.gaussian.sigma = (max - _dData.gaussian.mean) / 3.0f;
        break;
        }
    }
    else
    {
        _dData = *dData;
    }

    //on cree le random
    switch (_type)
    {
        case RD_UNIFORM:
        {
            _pRand = static_cast<void*> (new Random<boost::minstd_rand, boost::uniform_real<float> > (UNIFORM, (uint32)seed, min, max));
        }
        break;
        case RD_NORMAL:
        {
            mean = (_min+_max)/2.f;
            sigma = (_max-mean)/3.f;
            _pRand = static_cast<void*> (new Random<boost::minstd_rand, boost::normal_distribution<float> > (NORMAL, (uint32)seed, mean,  sigma));
        }
        break;
        case RD_GAUSSIAN:
        {
            mean = _dData.gaussian.mean;
            sigma = _dData.gaussian.sigma;
            _pRand = static_cast<void*> (new Random<boost::minstd_rand, boost::normal_distribution<float> > (NORMAL, (uint32)seed, mean,  sigma));
        }
        break;
    }
}
//-----------------------------------------------------------------------------
RandDistributionf::~RandDistributionf()
{
    switch (_type)
    {
        case RD_UNIFORM:
        {
            typedef Random<boost::minstd_rand, boost::uniform_real<float> > Random_t;
            delete static_cast<Random_t*>(_pRand);
        }
        break;
        case RD_NORMAL:
        {
            typedef Random<boost::minstd_rand, boost::normal_distribution<float> > Random_t;
            delete static_cast<Random_t*>(_pRand);
        }
        break;
        case RD_GAUSSIAN:
        {
            typedef Random<boost::minstd_rand, boost::normal_distribution<float> > Random_t;
            delete static_cast<Random_t*>(_pRand);
        }
        break;
    }
}
//-----------------------------------------------------------------------------
float RandDistributionf::operator() ()
{
    float ret;

    switch (_type)
    {
        case RD_UNIFORM:
        {
            typedef Random<boost::minstd_rand, boost::uniform_real<float> > Random_t;
            ret = (*static_cast<Random_t*>(_pRand))();
        }
        break;
        case RD_NORMAL:
        {
            typedef Random<boost::minstd_rand, boost::normal_distribution<float> > Random_t;
            ret = clamp(_min, _max, (*static_cast<Random_t*>(_pRand))());
        }
        break;
        case RD_GAUSSIAN:
        {
            typedef Random<boost::minstd_rand, boost::normal_distribution<float> > Random_t;
            ret = clamp(_min, _max, (*static_cast<Random_t*>(_pRand))());
        }
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
} //namespace Core


