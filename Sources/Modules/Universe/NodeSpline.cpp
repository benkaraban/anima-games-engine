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
#include <Core/XML/XMLMath.h>
#include <Universe/NodeSpline.h>

namespace Universe
{
//-----------------------------------------------------------------------------
NodeSpline::NodeSpline(const Ptr<Renderer::IAABox> & pBox,
                       const Ptr<Renderer::ISpline> & pSpline,
                       const Ptr<RessourcePool> & pRessourcesPool,
                       World * pWorld) :
    Node(pBox, pWorld),
    _pSpline(pSpline),
    _pRessourcesPool(pRessourcesPool)
{
}
//-----------------------------------------------------------------------------
void NodeSpline::setSpline(const Core::CRSpline & spline)
{
    _pSpline->setSpline(spline);
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
const Core::CRSpline & NodeSpline::getSpline() const
{
    return _pSpline->getSpline();
}
//-----------------------------------------------------------------------------
void NodeSpline::setResolution(int32 res)
{
    _pSpline->setResolution(res);
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
int32 NodeSpline::getResolution() const
{
    return _pSpline->getResolution();
}
//-----------------------------------------------------------------------------
void NodeSpline::setColor(const Core::Vector4f & color)
{
    _pSpline->setColor(color);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeSpline::getColor() const
{
    return _pSpline->getColor();
}
//-----------------------------------------------------------------------------
void NodeSpline::onWorldMatrixUpdate()
{
    _pSpline->setWorldMatrix(_localToWorldMatrix);
}
//-----------------------------------------------------------------------------
void NodeSpline::onVisibilityChange(bool flag)
{
    _pSpline->setVisible(flag);
}
//-----------------------------------------------------------------------------
void NodeSpline::onKill()
{
    _pSpline->kill();
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeSpline::internalGetBoundingBox() const
{
    return _pSpline->getBoundingBox();
}
//-----------------------------------------------------------------------------
void NodeSpline::update(double elapsed)
{
    Node::update(elapsed);
}
//-----------------------------------------------------------------------------
void NodeSpline::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    int32 resolution;
    Core::Vector4f color;

    Core::getXMLValue(*pNode, L"resolution", resolution);
    Core::getXMLValue(*pNode, L"color", color);

    setResolution(resolution);
    setColor(color);

    Ptr<Core::XMLNode> pCtrlPoints(pNode->firstChildNode(L"control-points"));

    Core::List<Ptr<Core::XMLNode> > xmlPoints;
    pCtrlPoints->getNodes(L"point", xmlPoints);

    Core::List<Core::Vector3f> points;
    points.resize(xmlPoints.size());

    for(int32 ii=0; ii < points.size(); ii++)
        Core::getXMLValue(*xmlPoints[ii], L"", points[ii]);

    _pSpline->setSpline(Core::CRSpline(points));
}
//-----------------------------------------------------------------------------
void NodeSpline::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"resolution", getResolution());
    pNode->setValue(L"color", Core::toXMLString(getColor()));

    Ptr<Core::XMLNode> pCtrlPoints(new Core::XMLNode(L"control-points"));
    const Core::List<Core::Vector3f> & points = _pSpline->getSpline().getPoints();

    for(int32 ii=0; ii < points.size(); ii++)
    {
        Ptr<Core::XMLNode> pPoint(new Core::XMLNode(L"point"));
        pPoint->setValue(Core::toXMLString(points[ii]));

        pCtrlPoints->addNode(pPoint);
    }

    pNode->addNode(pCtrlPoints);
}
//-----------------------------------------------------------------------------
}
