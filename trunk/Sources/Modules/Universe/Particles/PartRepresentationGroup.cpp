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
#include <Universe/Particles/PartRepresentationGroup.h>
#include <Universe/World.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartRepresentationGroup::PartRepresentationGroup(World * pWorld, const String & group) :
    _pWorld(pWorld),
    _groupName(group),
    _alignOnDir(false),
    _restartGroups(false),
    _affectSize(false),
    _affectRoll(false),
    _glowAsColor(false)
{
    _pNode = pWorld->createGroup();
    _pNode->setChildInstance();
    _pNode->setExportable(false);
}
//-----------------------------------------------------------------------------
PartRepresentationGroup::~PartRepresentationGroup()
{
    kill();
}
//-----------------------------------------------------------------------------
void PartRepresentationGroup::kill()
{
    _pNode->kill();
}
//-----------------------------------------------------------------------------
void PartRepresentationGroup::update(const Core::List<Particle> & particles)
{
    if(_groupName.empty())
        return;

    Core::Set<int32> isGroupAlive;

    for(int32 ii=0; ii < particles.size(); ii++)
    {
        const Particle & part = particles[ii];

        Core::Map<int32, Ptr<Node> >::iterator iGroup = _groups.find(part.id);
        Ptr<Node> pNode;

        if(iGroup == _groups.end())
        {
            pNode = _pWorld->createInstance(_groupName);
            pNode->setVisible(_pNode->isVisible(), true);

            _pNode->addChild(pNode);
            _groups[part.id] = pNode;

            if(_restartGroups)
                pNode->restart(true);

            pNode->setVisible(_pNode->isVisible());
        }
        else
        {
            pNode = iGroup->second;
        }

        pNode->beginMatrixUpdate();
        {
            pNode->setLocalPosition(part.position);

            if(_alignOnDir)
            {
                Core::Vector3f moveDir(part.position - part.oldPosition);

                if(!moveDir.isZero())
                    pNode->setLocalDirection(moveDir.getNormalizedSafe());
                else if(!part.speed.isZero())
                    pNode->setLocalDirection(part.speed.getNormalizedSafe());
            }

            if(_affectSize)
            {
                pNode->setLocalScale(Core::Vector3f(part.width));
            }

            if(_affectRoll)
            {
                pNode->localRoll(part.angle - part.oldAngle);
            }
        }
        pNode->endMatrixUpdate();

        pNode->setNodeColor(part.color, true);

        if(_glowAsColor)
            pNode->setNodeGlow(part.color, true);

        isGroupAlive.insert(part.id);
    }

    // Remove dead groups

    Core::Map<int32, Ptr<Node> >::iterator iGroup = _groups.begin();

    while(iGroup != _groups.end())
    {
        if(isGroupAlive.count(iGroup->first) > 0)
            ++iGroup;
        else
        {
            Core::Map<int32, Ptr<Node> >::iterator iDelete = iGroup++;
            iDelete->second->kill();
            _pNode->removeChild(iDelete->second);
            _groups.erase(iDelete);
        }
    }
}
//-----------------------------------------------------------------------------
void PartRepresentationGroup::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _pNode->setLocalMatrix(matrix);
}
//-----------------------------------------------------------------------------
void PartRepresentationGroup::setVisible(bool flag)
{
    _pNode->setVisible(flag, true);
}
//-----------------------------------------------------------------------------
Core::AABoxf PartRepresentationGroup::getBoundingBox() const
{
    return _pNode->getBoundingBox();
}
//-----------------------------------------------------------------------------
void PartRepresentationGroup::importXML(const Ptr<Core::XMLNode> & pNode)
{
    _glowAsColor = false;

    pNode->getValue(L"group-name", _groupName);
    pNode->getValue(L"align-on-dir", _alignOnDir);
    pNode->getValue(L"restart-groups", _restartGroups);
    pNode->getValue(L"affect-size", _affectSize);
    pNode->getValue(L"affect-roll", _affectRoll);
    pNode->getValue(L"glow-as-color", _glowAsColor);
}
//-----------------------------------------------------------------------------
void PartRepresentationGroup::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    pNode->setValue(L"group-name", _groupName);
    pNode->setValue(L"align-on-dir", _alignOnDir);
    pNode->setValue(L"restart-groups", _restartGroups);
    pNode->setValue(L"affect-size", _affectSize);
    pNode->setValue(L"affect-roll", _affectRoll);
    pNode->setValue(L"glow-as-color", _glowAsColor);
}
//-----------------------------------------------------------------------------
}
