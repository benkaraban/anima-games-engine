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
#ifndef CORE_TRAJECTORY_H_
#define CORE_TRAJECTORY_H_

#include <Core/List.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Splines/CatmullRom.h>
#include <Core/XML/XMLNode.h>

namespace Core
{

struct LM_API_COR CheckPoint
{
    CheckPoint();
    CheckPoint(const Vector3f & pos, const Quaternionf & ori, float t);

    Vector3f     position;
    Quaternionf  orient;
    float        time;

    void importXML(const Ptr<Core::XMLNode> & pNode);
    void exportXML(const Ptr<Core::XMLNode> & pNode) const;
};

/**
 * Catmull Rom Splines
 */
class LM_API_COR Trajectory : public Core::Object
{
public:
    Trajectory();
    Trajectory(const CRSpline & spline);
    Trajectory(const List<CheckPoint> & checkPoints, double timeStep = 0.0);

    void update(const List<CheckPoint> & checkPoints, double timeStep = 0.0);

    float       getDuration() const { return _duration; };

    Vector3f    getPosition(float t) const;
    Quaternionf getOrientation(float t) const;
    void        getORPO(Vector3f & pos, Quaternionf & ori, float t) const;

    const List<CheckPoint> &    getCheckPoints() const  { return _checkPoints; };
    List<CheckPoint> &          getCheckPoints()        { return _checkPoints; };
    const CRSpline &            getSplinePos() const    { return _splinePos; };
    const CRQuatSpline &        getSplineOri() const    { return _splineOri; };

    void importXML(const Ptr<Core::XMLNode> & pNode);
    void exportXML(const Ptr<Core::XMLNode> & pNode) const;

protected:
    List<CheckPoint>   _checkPoints;
    CRSpline           _splinePos;
    CRQuatSpline       _splineOri;
    float              _duration;
};

}

#endif /* TRAJECTORY_H_ */
