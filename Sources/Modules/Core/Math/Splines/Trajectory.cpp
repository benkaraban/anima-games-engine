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
#include <Core/Math/Splines/Trajectory.h>
#include <Core/Math/Splines/Bezier.h>
#include <Core/XML/XMLMath.h>
#include <Core/Logger.h>

namespace Core
{
//-----------------------------------------------------------------------------
CheckPoint::CheckPoint() :
    position(ZERO), orient(IDENTITY), time(0.0)
{}
//-----------------------------------------------------------------------------
CheckPoint::CheckPoint(const Vector3f & pos, const Quaternionf & ori, float t) :
    position(pos), orient(ori), time(t)
{}
//-----------------------------------------------------------------------------
void CheckPoint::importXML(const Ptr<Core::XMLNode> & pNode)
{
    pNode->getValue(L"t", time);
    getXMLValue(*pNode, L"pos", position);
    getXMLValue(*pNode, L"ori", orient);
}
//-----------------------------------------------------------------------------
void CheckPoint::exportXML(const Ptr<Core::XMLNode> & pNode) const
{
    pNode->setValue(L"t", time);
    pNode->setValue(L"pos", Core::toXMLString(position));
    pNode->setValue(L"ori", Core::toXMLString(orient));
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Trajectory::Trajectory()
{
}
//-----------------------------------------------------------------------------
Trajectory::Trajectory(const CRSpline & spline)
{
    List<CheckPoint> cp;
    const List<Vector3f> & pts = spline.getPoints();

    for(int32 ii=0; ii < pts.size(); ii++)
        cp.push_back(CheckPoint(pts[ii], Quaternionf(IDENTITY), float(ii)));

    update(cp, 0.0);
}
//-----------------------------------------------------------------------------
Trajectory::Trajectory(const List<CheckPoint> & checkPoints, double timeStep)
{
    update(checkPoints, timeStep);
}
//-----------------------------------------------------------------------------
void Trajectory::update(const List<CheckPoint> & checkPoints, double timeStep)
{
    LM_ASSERT(checkPoints.size() >= 2);
    _checkPoints = checkPoints;

    {
        List<Vector3f> points;
        List<Quaternionf> quats;

        points.push_back(checkPoints[0].position);
        quats.push_back(checkPoints[0].orient.getNormalizedFast());
        
        for(int32 ii=0; ii < checkPoints.size(); ii++)
        {
            points.push_back(checkPoints[ii].position);
            quats.push_back(checkPoints[ii].orient.getNormalizedFast());
        }
    
        points.push_back(checkPoints[checkPoints.size() - 1].position);
        quats.push_back(checkPoints[checkPoints.size() - 1].orient.getNormalizedFast());
        
        _splinePos = CRSpline(points);
        _splineOri = CRQuatSpline(quats);

        _duration = checkPoints[checkPoints.size() - 1].time;
    }

    if(timeStep != 0.0)
    {
        int32 stepCount = checkPoints.size() - 1;
        double minDuration = DBL_MAX;
        List<double> durations;
        List<Vector3f> points;
        List<Quaternionf> quats;
        
        durations.resize(stepCount);
        
        for(int32 ii=0; ii < stepCount; ii++)
        {
            durations[ii] = checkPoints[ii + 1].time - checkPoints[ii].time;
            minDuration = std::min(minDuration, durations[ii]);
        }
        
        points.push_back(checkPoints[0].position);
        quats.push_back(checkPoints[0].orient.getNormalizedFast());

        for(int32 i0=0; i0 < stepCount; i0++)
        {
            double t0 = double(i0) / double(stepCount);
            double t1 = double(i0 + 1) / double(stepCount);
            double dt = t1 - t0;

            int32 subSteps = std::max(1, int32(durations[i0] / timeStep));

            for(int32 iSub=0; iSub < subSteps; iSub++)
            {
                double subt = double(iSub) / double(subSteps);
                float t = float(t0 + subt * dt);

                points.push_back(_splinePos.eval(t));
                quats.push_back(_splineOri.eval(t).getNormalizedFast());
            }
        }
            
        points.push_back(checkPoints[stepCount].position);
        points.push_back(checkPoints[stepCount].position);
        quats.push_back(checkPoints[stepCount].orient.getNormalizedFast());
        quats.push_back(checkPoints[stepCount].orient.getNormalizedFast());

        _splinePos = CRSpline(points);
        _splineOri = CRQuatSpline(quats);
    }
}
//-----------------------------------------------------------------------------
Vector3f Trajectory::getPosition(float t) const
{
    float u = 1.0;
    
    if(_duration != 0.0)
        u = t / _duration;
    
    return _splinePos.eval(u);
}
//-----------------------------------------------------------------------------
Quaternionf Trajectory::getOrientation(float t) const
{
    float u = 1.0;
    
    if(_duration != 0.0)
        u = t / _duration;
    
    return _splineOri.eval(u);
}
//-----------------------------------------------------------------------------
void Trajectory::getORPO(Vector3f & pos, Quaternionf & ori, float t) const
{
    float u = 1.0;
    
    if(_duration != 0.0)
        u = t / _duration;
    
    pos = _splinePos.eval(u);
    ori = _splineOri.eval(u);
}
//-----------------------------------------------------------------------------
void Trajectory::importXML(const Ptr<Core::XMLNode> & pNode)
{
    float duration;
    List<CheckPoint> cp;
    List<Ptr<XMLNode> > cpXML;

    pNode->getValue(L"duration", duration);
    pNode->getNodes(L"checkpoints", L"cp", cpXML);

    cp.resize(cpXML.size());

    for(int32 ii=0; ii < cp.size(); ii++)
        cp[ii].importXML(cpXML[ii]);

    update(cp, duration);
}
//-----------------------------------------------------------------------------
void Trajectory::exportXML(const Ptr<Core::XMLNode> & pNode) const
{
    pNode->setValue(L"duration", _duration);

    for(int32 ii=0; ii < _checkPoints.size(); ii++)
    {
        Ptr<XMLNode> pNodeCP(new XMLNode(L"cp"));
        _checkPoints[ii].exportXML(pNodeCP);
        pNode->addNode(L"checkpoints", pNodeCP);
    }
}
//-----------------------------------------------------------------------------
}
