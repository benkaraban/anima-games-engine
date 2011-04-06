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
#ifndef CORE_IPROGRESSOBS_H_
#define CORE_IPROGRESSOBS_H_

#include <Core/Standard.h>

namespace Core
{
//-----------------------------------------------------------------------------
class LM_API_COR IProgressObs
{
public:
    virtual ~IProgressObs() {};

    /**
     * p est compris entre 0.0 et 1.0
     */
    virtual void progress(float p) = 0;
};

//-----------------------------------------------------------------------------
class LM_API_COR PtrProgressObs : public IProgressObs
{
public:
    PtrProgressObs(IProgressObs * pObs) : _pObs(pObs) {}
    virtual void progress(float p) { _pObs->progress(p); }

protected:
    IProgressObs * _pObs;
};

//-----------------------------------------------------------------------------
class LM_API_COR SubProgressObs : public IProgressObs
{
public:
    SubProgressObs(const Ptr<IProgressObs> & pParent, float start, float end);
    virtual void progress(float p);

protected:
    Ptr<IProgressObs>   _pParent;
    float               _start;
    float               _end;
};

//-----------------------------------------------------------------------------
class LM_API_COR ProgressPrint : public IProgressObs
{
public:
    ProgressPrint();
    virtual void progress(float p);

protected:
    int32   _curStep;
};

}

#endif/*CORE_IPROGRESSOBS_H_*/