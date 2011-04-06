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
#ifndef CHRONORAGE_LEVEL_H_
#define CHRONORAGE_LEVEL_H_

#include <Core/Standard.h>
#include <Core/Math/GeometryComputations.h>

namespace ChronoRage
{

class LM_API_CHR Level : public Core::Object
{
public:
    Level(float width, float height, float backgroundDepth, float playgroundDepth);
    virtual ~Level();

    inline float xMin() const { return _xMin; }
    inline float xMax() const { return _xMax; }
    inline float yMin() const { return _yMin; }
    inline float yMax() const { return _yMax; }

    inline float backgroundDepth() const { return _backgroundDepth; }
    inline float playgroundDepth() const { return _playgroundDepth; }

    Core::Planef getPlane();

    bool isOutOfBoundaries(const Core::Vector3f & position, float tolerance = 0.0f) const;

private:
    float _xMin;
    float _xMax;
    float _yMin;
    float _yMax;

    float _backgroundDepth;
    float _playgroundDepth;
};


}//namespace ChronoRage

#endif/*CHRONORAGE_LEVEL_H_*/