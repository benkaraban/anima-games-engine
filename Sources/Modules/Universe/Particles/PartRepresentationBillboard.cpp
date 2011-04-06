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
#include <Universe/Particles/PartRepresentationBillboard.h>
#include <Universe/World.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartRepresentationBillboard::PartRepresentationBillboard(World & world) :
    _pRessourcesPool(world.getRessourcesPool())
{
    Ptr<Renderer::ITextureMap> pTexture(_pRessourcesPool->getDefaultTexture());
    _pBill = Ptr<Renderer::IBillboardGroup>(world.getScene()->createBillboardGroup(1000, pTexture, Renderer::BILLBOARD_ADD));
}
//-----------------------------------------------------------------------------
PartRepresentationBillboard::PartRepresentationBillboard(
        const Ptr<RessourcePool> & pRessourcesPool,
        const Ptr<Renderer::IBillboardGroup> & pBill) :
    _pRessourcesPool(pRessourcesPool),
    _pBill(pBill)
{
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::update(const Core::List<Particle> & particles)
{
    Core::List<Renderer::Billboard> & bills = _pBill->beginUpdate();

    bills.resize(particles.size());

    for(int32 ii=0; ii < particles.size(); ii++)
    {
        bills[ii].position = particles[ii].position;
        bills[ii].color = particles[ii].color;
        bills[ii].width = particles[ii].width;
        bills[ii].height = particles[ii].height;
        bills[ii].time = particles[ii].timeKey;
    }

    if(_pBill->getAlign() == Renderer::BILLBOARD_ALIGN_HORIZONTAL_ON_DIR || _pBill->getAlign() == Renderer::BILLBOARD_ALIGN_VERTICAL_ON_DIR)
    {
        Core::Matrix4f world(_pBill->getWorldMatrix());

        for(int32 ii=0; ii < particles.size(); ii++)
        {
            Core::Vector3f dir(particles[ii].position - particles[ii].oldPosition);

            if(dir.isZero())
                Assets::packUInt8Vec3(world.apply3x3(particles[ii].speed).getNormalizedFast(), bills[ii].dir);
            else
                Assets::packUInt8Vec3(world.apply3x3(dir).getNormalizedFast(), bills[ii].dir);
        }
    }
    else
    {
        for(int32 ii=0; ii < particles.size(); ii++)
            bills[ii].angle = particles[ii].angle;
    }

    _pBill->endUpdate();
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setColorFactor(float f)
{
    _pBill->setColorFactor(f);
}
//-----------------------------------------------------------------------------
float PartRepresentationBillboard::getColorFactor() const
{
    return _pBill->getColorFactor();
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setGlowFactor(float f)
{
    _pBill->setGlowFactor(f);
}
//-----------------------------------------------------------------------------
float PartRepresentationBillboard::getGlowFactor() const
{
    return _pBill->getGlowFactor();
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _pBill->setWorldMatrix(matrix);
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::kill()
{
    _pBill->kill();
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setMaxSize(int32 maxSize)
{
    _pBill->setMaxSize(maxSize);
}
//-----------------------------------------------------------------------------
int32 PartRepresentationBillboard::getMaxSize() const
{
    return _pBill->getMaxSize();
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setPartScale(float s)
{
    _pBill->setPartScale(s);
}
//-----------------------------------------------------------------------------
float PartRepresentationBillboard::getPartScale() const
{
    return _pBill->getPartScale();
}
//-----------------------------------------------------------------------------
const String & PartRepresentationBillboard::getTexture() const
{
    return _textureName;
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setTexture(const String & name)
{
    Ptr<Renderer::ITextureMap> pTexture(_pRessourcesPool->getTexture(name));

    if(pTexture == null)
    {
        pTexture = _pRessourcesPool->getDefaultTexture();
        _textureName = String::EMPTY;
    }
    else
    {
        _textureName = name;
    }

    _pBill->setTexture(pTexture);
}
//-----------------------------------------------------------------------------
Renderer::EBillboardMode PartRepresentationBillboard::getMode() const
{
    return _pBill->getMode();
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setMode(Renderer::EBillboardMode mode)
{
    _pBill->setMode(mode);
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setAlign(Renderer::EBillboardAlign align)
{
    _pBill->setAlign(align);
}
//-----------------------------------------------------------------------------
Renderer::EBillboardAlign PartRepresentationBillboard::getAlign() const
{
    return _pBill->getAlign();
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setAlignVector(const Core::Vector3f & vec)
{
    _pBill->setAlignVector(vec);
}
//-----------------------------------------------------------------------------
const Core::Vector3f & PartRepresentationBillboard::getAlignVector() const
{
    return _pBill->getAlignVector();
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::setVisible(bool flag)
{
    _pBill->setVisible(flag);
}
//-----------------------------------------------------------------------------
Core::AABoxf PartRepresentationBillboard::getBoundingBox() const
{
    return _pBill->getBoundingBox();
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::importXML(const Ptr<Core::XMLNode> & pNode)
{
    String texture;
    String mode;
    String align;
    String alignVec;

    pNode->getValue(L"texture", texture);
    pNode->getValue(L"mode", mode);
    
    setTexture(texture);
    if(mode == Renderer::toString(Renderer::BILLBOARD_ADD))
        setMode(Renderer::BILLBOARD_ADD);
    else if(mode == Renderer::toString(Renderer::BILLBOARD_LERP))
        setMode(Renderer::BILLBOARD_LERP);
    else if(mode == Renderer::toString(Renderer::BILLBOARD_ADD_LERP))
        setMode(Renderer::BILLBOARD_ADD_LERP);
    else if(mode == Renderer::toString(Renderer::BILLBOARD_REFRAC))
        setMode(Renderer::BILLBOARD_REFRAC);
    else
        throw Core::Exception(L"Unknown NodeEmitter Representation Mode :" + mode);

    if(pNode->getValue(L"align", align))
    {
        if(align == Renderer::toString(Renderer::BILLBOARD_ALIGN_FACE_CAM))
            setAlign(Renderer::BILLBOARD_ALIGN_FACE_CAM);
        else if(align == Renderer::toString(Renderer::BILLBOARD_ALIGN_HORIZONTAL_ON_VEC))
            setAlign(Renderer::BILLBOARD_ALIGN_HORIZONTAL_ON_VEC);
        else if(align == Renderer::toString(Renderer::BILLBOARD_ALIGN_VERTICAL_ON_VEC))
            setAlign(Renderer::BILLBOARD_ALIGN_VERTICAL_ON_VEC);
        else if(align == Renderer::toString(Renderer::BILLBOARD_ALIGN_HORIZONTAL_ON_DIR))
            setAlign(Renderer::BILLBOARD_ALIGN_HORIZONTAL_ON_DIR);
        else if(align == Renderer::toString(Renderer::BILLBOARD_ALIGN_VERTICAL_ON_DIR))
            setAlign(Renderer::BILLBOARD_ALIGN_VERTICAL_ON_DIR);
        else
            throw Core::Exception(L"Unknown NodeEmitter Representation Align :" + align);
    }
    else
    {
        setAlign(Renderer::BILLBOARD_ALIGN_FACE_CAM);
    }

    if(pNode->getValue(L"align-vector", alignVec))
    {
        Core::Vector3f alignVec3;
        Core::fromXMLString(alignVec, alignVec3);
        setAlignVector(alignVec3);
    }
    else
    {
        setAlignVector(Core::Vector3f(0.0f, 1.0f, 0.0f));
    }

    int32 uSpriteLayout; 
    int32 vSpriteLayout; 
    pNode->getValue(L"u-sprite-layout", uSpriteLayout);
    pNode->getValue(L"v-sprite-layout", vSpriteLayout);
    setSpriteLayout(uSpriteLayout, vSpriteLayout);

    bool isSpriteSmooth = false;
    pNode->getValue(L"smooth-sprite", isSpriteSmooth);
    setSpriteSmooth(isSpriteSmooth);
}
//-----------------------------------------------------------------------------
void PartRepresentationBillboard::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    pNode->setValue(L"texture", getTexture());
    pNode->setValue(L"mode", Renderer::toString(getMode()));
    pNode->setValue(L"u-sprite-layout", getUSpriteLayout());
    pNode->setValue(L"v-sprite-layout", getVSpriteLayout());
    pNode->setValue(L"smooth-sprite", isSpriteSmooth());
    pNode->setValue(L"align", Renderer::toString(getAlign()));
    pNode->setValue(L"align-vector", Core::toXMLString(getAlignVector()));
}
//-----------------------------------------------------------------------------
}
