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

#include <Core/Math/MathIO.h>
#include <Core/Math/MathTools.h>
#include <Assets/Animation.h>
#include <Assets/ModelNode.h>

namespace Assets
{
//-----------------------------------------------------------------------------
static const int32 ANIMATION_MAGIC = LM_FOURCC('A', 'N', 'I', '0');
//-----------------------------------------------------------------------------
static const int32 ANIMATION_DURATION_FLAG         = 0x00000001;
//-----------------------------------------------------------------------------
Animation::Animation() :
    _frameCount(0),
    _flags(ANIMATION_DURATION_FLAG),
    _duration(1.0f)
{}
//-----------------------------------------------------------------------------
Animation::Animation(int32 nodeCount) :
    _frameCount(0),
    _flags(ANIMATION_DURATION_FLAG),
    _duration(1.0f)
{
    _keyFrames.resize(nodeCount);
}
//-----------------------------------------------------------------------------
Animation::Animation(const Animation & anim) : 
    _frameCount(anim._frameCount),
    _flags(anim._flags),
    _duration(anim._duration),
    _keyFrames(anim._keyFrames)
{
}
//-----------------------------------------------------------------------------
Animation::Animation(const Animation & anim, int32 startKey, int32 endKey) :
    _frameCount(endKey - startKey + 1),
    _flags(anim._flags),
    _duration(0.0f)
{
    if(startKey < 0 || startKey >= anim._frameCount)
        throw Core::Exception(L"Invalid startKey animation slice");

    if(endKey < 0 || endKey >= anim._frameCount)
        throw Core::Exception(L"Invalid endKey animation slice");

    if(anim._frameCount > 0)
        _duration = anim._duration * (float(_frameCount) / float(anim._frameCount));

    _keyFrames.resize(anim._keyFrames.size());

    for(int32 ii=0; ii < _keyFrames.size(); ii++)
    {
        const Core::List<KeyFrame> & src = anim._keyFrames[ii];
        Core::List<KeyFrame> & dst = _keyFrames[ii];

        if(!src.empty())
        {
            dst.insert(dst.end(), src.begin() + startKey, src.begin() + endKey + 1);

            int32 startFrame = dst[0].frameNumber;

            for(int32 iKey=0; iKey < dst.size(); iKey++)
                dst[iKey].frameNumber -= startFrame;
        }
    }
}
//-----------------------------------------------------------------------------
Animation::Animation(const Animation & anim, int32 nodeCount, const Core::List<int32> & cvtNodeId) :
    _frameCount(anim._frameCount),
    _flags(anim._flags),
    _duration(anim._duration)
{
    _keyFrames.resize(nodeCount);

    for(int32 iNode=0; iNode < anim._keyFrames.size(); iNode++)
    {
        int32 newNodeId = cvtNodeId[iNode];

        if(newNodeId != UNDEFINED_NODE_ID && newNodeId < _keyFrames.size())
        {
            for(int32 iFrame=0; iFrame < anim._keyFrames[iNode].size(); iFrame++)
                addKeyFrame(newNodeId, anim._keyFrames[iNode][iFrame]);
        }
    }
}
//-----------------------------------------------------------------------------
void Animation::read(Core::InputStream & input)
{
    if(input.readInt32() != ANIMATION_MAGIC)
        throw Core::IOException(L"Bad animation magic");

    input.read(_flags);

    if((_flags & ANIMATION_DURATION_FLAG) != 0)
        input.read(_duration);
    else
        _duration = 1.0;

    _keyFrames.clear();
    input.read(_frameCount);

    int32 nodeCount = input.readInt32();
    _keyFrames.resize(nodeCount);

    for(Core::List<Core::List<KeyFrame> >::iterator iNode = _keyFrames.begin(); iNode != _keyFrames.end(); ++iNode)
    {
        int32 keyFrameCount = input.readInt32();
        iNode->resize(keyFrameCount);

        for(Core::List<KeyFrame>::iterator iKeyFrame = (*iNode).begin(); iKeyFrame != (*iNode).end(); ++iKeyFrame)
            Assets::read(input, *iKeyFrame);
    }

}
//-----------------------------------------------------------------------------
void Animation::write(Core::OutputStream & output) const
{
    Core::write(output, ANIMATION_MAGIC);
    Core::write(output, _flags);

    if((_flags & ANIMATION_DURATION_FLAG) != 0)
        Core::write(output, _duration);

    Core::write(output, _frameCount);
    Core::write(output, _keyFrames.size());
    for(Core::List<Core::List<KeyFrame> >::const_iterator iNode = _keyFrames.begin(); iNode != _keyFrames.end(); ++iNode)
    {
        Core::write(output, iNode->size());
        for(Core::List<KeyFrame>::const_iterator iKeyFrame = (*iNode).begin(); iKeyFrame != (*iNode).end(); ++iKeyFrame)
            Assets::write(output, *iKeyFrame);
    }
}
//-----------------------------------------------------------------------------
void Animation::getNodeMatrix(Core::Matrix4f & matrix, int32 nodeId, float alpha) const
{
    const Core::List<KeyFrame> & keyFrames = _keyFrames[nodeId];

    if(_frameCount <= 0 || keyFrames.size() <= 0)
    {
        matrix.setIdentity();
    }
    else
    {
        float frameNumber = alpha * (_frameCount - 1);
        int32 iFrame = int32(Core::L_floor(frameNumber));

        int32 iNextKeyFrame = 0;

        while(iNextKeyFrame < keyFrames.size() && keyFrames[iNextKeyFrame].frameNumber <= iFrame)
            iNextKeyFrame ++;

        if(iNextKeyFrame >= keyFrames.size())
        {
            matrix.setTransform(keyFrames[keyFrames.size() - 1].key);
        }
        else if(iNextKeyFrame == 0 || keyFrames.size() == 1)
        {
            matrix.setTransform(keyFrames[0].key);
        }
        else
        {
            iNextKeyFrame = std::min(iNextKeyFrame, keyFrames.size() - 1);
            int32 iPrevKeyFrame = iNextKeyFrame - 1;

            int32 iPrevFrame = keyFrames[iPrevKeyFrame].frameNumber;
            int32 iNextFrame = keyFrames[iNextKeyFrame].frameNumber;

            float frameAlpha = 0.0;

            if(iPrevFrame != iNextFrame)
            {
                frameAlpha = (frameNumber - iPrevFrame) / (iNextFrame - iPrevFrame);
                frameAlpha = std::min(1.0f, std::max(0.0f, frameAlpha));
            }

            const Core::Transformf & transform1 = keyFrames[iPrevKeyFrame].key;
            const Core::Transformf & transform2 = keyFrames[iNextKeyFrame].key;

            Core::Transformf transform(transform1, transform2, frameAlpha);
            matrix.setTransform(transform);
        }
    }
}
//-----------------------------------------------------------------------------
Animation & Animation::operator = (const Animation & anim)
{
    if(this != &anim)
    {
        _frameCount = anim._frameCount;
        _flags = anim._flags;
        _duration = anim._duration;
        _keyFrames = anim._keyFrames;
    }
    return *this;
}
//-----------------------------------------------------------------------------
bool Animation::hasKeyFrame(int32 nodeId) const 
{ 
    return (nodeId < _keyFrames.size()) && (_keyFrames[nodeId].size() >= 1);
}
//-----------------------------------------------------------------------------
bool Animation::operator == (const Animation & anim) const
{
    return _frameCount == anim._frameCount && _keyFrames == anim._keyFrames &&
        _flags == anim._flags && _duration == anim._duration;
}
//-----------------------------------------------------------------------------
bool Animation::operator != (const Animation & anim) const
{
    return !(*this == anim);
}
//-----------------------------------------------------------------------------
void Animation::addKeyFrame(int32 nodeId, const KeyFrame & frame)
{
    Core::List<KeyFrame> & keyFrames = _keyFrames[nodeId];

    Core::List<KeyFrame>::const_iterator iKeyFrame = keyFrames.begin();
    while(iKeyFrame != keyFrames.end() && iKeyFrame->frameNumber <= frame.frameNumber)
        ++iKeyFrame;
    keyFrames.insert(iKeyFrame, frame);
    _frameCount = std::max(_frameCount, frame.frameNumber + 1);
}
//-----------------------------------------------------------------------------
}
