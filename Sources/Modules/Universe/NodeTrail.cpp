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
#include <Core/Math/Interpolate.h>
#include <Universe/NodeTrail.h>

namespace Universe
{
static const double POS_THRESHOLD = 0.001;
//-----------------------------------------------------------------------------
NodeTrail::NodeTrail(const Ptr<Renderer::IAABox> & pBox,
                     const Ptr<Renderer::IFreeForm> & pFreeForm,
                     const Ptr<RessourcePool> & pRessourcesPool,
                     World * pWorld) :
    Node(pBox, pWorld),
    _pFreeForm(pFreeForm),
    _pRessourcesPool(pRessourcesPool),
    _length(20.0f),
    _segCount(30),
    _mainColor(1.0f),
    _mainGlow(1.0f),
    _lastTime(0.0),
    _relativeSize(0.0f)
{
    _trail.mode = Renderer::TRAIL_STANDARD_UNORM;

    _trail.headColor = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    _trail.tailColor = Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f);

    _trail.headGlow  = Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
    _trail.headGlow  = Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

    _trail.headWidth = 0.5f;
    _trail.tailWidth = 0.5f;

    _trail.headU = 0.0f;
    _trail.tailU = 1.0f;
}
//-----------------------------------------------------------------------------
void NodeTrail::restart(bool recursive)
{
    Node::restart(recursive);
    _lastTime = 0.0;
    _positions.clear();
}
//-----------------------------------------------------------------------------
void NodeTrail::setTexture(const String & textureName)
{
    Ptr<Renderer::ITextureMap> pTexture;

    if(textureName == String::EMPTY)
        pTexture = _pRessourcesPool->getDefaultTexture();
    else
        pTexture = _pRessourcesPool->getTexture(textureName);

    _textureName = textureName;
    _pFreeForm->setTexture(pTexture);
}
//-----------------------------------------------------------------------------
void NodeTrail::setMode(Renderer::EFreeFormMode mode)
{
    _pFreeForm->setMode(mode);
}
//-----------------------------------------------------------------------------
Renderer::EFreeFormMode NodeTrail::getMode() const
{
    return _pFreeForm->getMode();
}
//-----------------------------------------------------------------------------
void NodeTrail::onVisibilityChange(bool flag)
{
    _pFreeForm->setVisible(flag);
}
//-----------------------------------------------------------------------------
void NodeTrail::onKill()
{
    _pFreeForm->kill();
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeTrail::internalGetBoundingBox() const
{
    return _pFreeForm->getBoundingBox();
}
//-----------------------------------------------------------------------------
void NodeTrail::internalPrepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodeTrail::update(double elapsed)
{
    Node::update(elapsed);

    if(_positions.empty())
    {
        _relativeSize = 0.0f;
        _positions.push_back(getWorldPosition());
    }
    else
    {
        Core::Vector3f newPos(getWorldPosition());
        Core::Vector3f oldPos(_positions.back());
        Core::Vector3f disp(newPos - oldPos);
        float dispLength = disp.getLengthSafe();
        Core::Vector3f unitDisp((1.0f / dispLength) * disp);

        float segmentLength = _length / float(_segCount - 1);
        int32 ctrlPoints = int32(Core::L_floor(dispLength / segmentLength));

        for(int32 ii=1; ii <= ctrlPoints; ii++)
        {
            while(_positions.size() >= _segCount)
                _positions.pop_front();

            Core::Vector3f ctrlPoint(oldPos + float(ii) * segmentLength * unitDisp);
            _positions.push_back(ctrlPoint);
        }

        _renderPos.clear();
        _renderPos.insert(_renderPos.end(), _positions.begin(), _positions.end());

        if(newPos != _positions.back())
        {
            Core::Vector3f remDisp(newPos - _positions.back());
            float remLength = remDisp.getLengthSafe();

            _renderPos.push_back(newPos);
            std::reverse(_renderPos.begin(), _renderPos.end());

            float duPerSegment = 1.0f / std::max(1, _renderPos.size() - 2);
            _trail.duStart = duPerSegment * (0.0f + (remLength / segmentLength));

            LM_ASSERT(_trail.duStart >= 0.0f);
            LM_ASSERT(_trail.duStart <= 1.0f);

            Core::Vector3f p1(_renderPos[_renderPos.size() - 2]);
            Core::Vector3f p2(_renderPos[_renderPos.size() - 1]);
            Core::Vector3f p1p2n((p2 - p1));

            if(segmentLength > dispLength && _positions.size() >= _segCount)
            {
                p1p2n.normalizeSafe();
                _renderPos[_renderPos.size() - 1] = p1 + (segmentLength - dispLength) * p1p2n;
                _trail.tailClamp = true;
            }
            else
            {
                _trail.tailClamp = false;
            }
            
            if(_positions.size() < _segCount)
            {
                _relativeSize = std::min(1.0f, (float(_renderPos.size() - 2) * segmentLength) / _length);

                if(_relativeSize == 0.0f)
                {
                    _relativeSize = dispLength / _length;
                }

                _trail.tailClamp = false;
            }
            else
            {
                _relativeSize = std::min(1.0f, (float(_renderPos.size() - 2) * segmentLength) / _length);
                _trail.tailClamp = true;
            }
        }
   }

    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void NodeTrail::prepareForRendering(const Ptr<NodeCamera> & pCamera)
{
    Renderer::TrailConfig       trail(_trail);
    trail.viewVec = pCamera->getWorldDirection();
    trail.headColor = Core::mul(_trail.headColor, _mainColor);
    trail.tailColor = Core::mul(_trail.tailColor, _mainColor);
    trail.headGlow  = Core::mul(_trail.headGlow,  _mainGlow);
    trail.tailGlow  = Core::mul(_trail.tailGlow,  _mainGlow);
    trail.tailU     = Core::lerp(_trail.headU, _trail.tailU, _relativeSize);
    trail.tailWidth = Core::lerp(_trail.headWidth, _trail.tailWidth, _relativeSize);
    trail.tailFade  = !trail.tailClamp;

    Renderer::FreeFormData & data = _pFreeForm->beginUpdate();
    {
        if(_renderPos.size() >= 2)
        {
            Renderer::generateTrail(data.vertices, data.indices, _renderPos, trail);
        }
        else
        {
            data.vertices.clear();
            data.indices.clear();
        }
    }
    _pFreeForm->endUpdate();
}
//-----------------------------------------------------------------------------
void NodeTrail::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    String textureName;
    String modeName;
    String trailModeName;
    Renderer::EFreeFormMode mode = (Renderer::EFreeFormMode) 0;

    pNode->getValue(L"texture", textureName);
    pNode->getValue(L"length", _length);
    pNode->getValue(L"seg-count", _segCount);

    pNode->getValue(L"head-width", _trail.headWidth);
    pNode->getValue(L"tail-width", _trail.tailWidth);

    pNode->getValue(L"head-u", _trail.headU);
    pNode->getValue(L"tail-u", _trail.tailU);

    Core::getXMLValue(*pNode, L"head-color", _trail.headColor);
    Core::getXMLValue(*pNode, L"tail-color", _trail.tailColor);

    Core::getXMLValue(*pNode, L"head-glow", _trail.headGlow);
    Core::getXMLValue(*pNode, L"tail-glow", _trail.tailGlow);

    pNode->getValue(L"mode", modeName); // Le mode est stock� comme un int
    _pFreeForm->setMode((Renderer::EFreeFormMode)Core::toInteger(modeName));

    _trail.mode = Renderer::TRAIL_STANDARD_UNORM;
    if(pNode->getValue(L"trail-mode", trailModeName))
    {
        int32 ii = 0;
        while(ii < Renderer::ETrailMode_COUNT)
        {
            if(trailModeName == Renderer::toString((Renderer::ETrailMode) ii))
                _trail.mode = (Renderer::ETrailMode) ii;
            ii++;
        }
    }

    setTexture(textureName);
    restart();
}
//-----------------------------------------------------------------------------
void NodeTrail::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"texture", _textureName);
    pNode->setValue(L"length", _length);
    pNode->setValue(L"seg-count", _segCount);

    pNode->setValue(L"head-width", _trail.headWidth);
    pNode->setValue(L"tail-width", _trail.tailWidth);

    pNode->setValue(L"head-u", _trail.headU);
    pNode->setValue(L"tail-u", _trail.tailU);

    pNode->setValue(L"head-color", Core::toXMLString(_trail.headColor));
    pNode->setValue(L"tail-color", Core::toXMLString(_trail.tailColor));

    pNode->setValue(L"head-glow", Core::toXMLString(_trail.headGlow));
    pNode->setValue(L"tail-glow", Core::toXMLString(_trail.tailGlow));

    pNode->setValue(L"mode", Core::toString(_pFreeForm->getMode()));
    pNode->setValue(L"trail-mode", Renderer::toString(_trail.mode));
}
//-----------------------------------------------------------------------------
void NodeTrail::setNodeColor(const Core::Vector4f & color, bool recursive)
{
    Node::setNodeColor(color, recursive);
    setMainColor(color);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeTrail::getNodeColor() const
{
    return getMainColor();
}
//-----------------------------------------------------------------------------
void NodeTrail::setNodeGlow(const Core::Vector4f & color, bool recursive)
{
    Node::setNodeGlow(color, recursive);
    setMainGlow(color);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeTrail::getNodeGlow() const
{
    return getMainGlow();
}
//-----------------------------------------------------------------------------
bool NodeTrail::internalIsIntersecting(const Core::Rayf& ray, float& distance)
{
    return isIntersectingBoundingBox(ray, distance);
}
//-----------------------------------------------------------------------------
}
