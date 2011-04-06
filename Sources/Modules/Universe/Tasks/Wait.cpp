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
#include <Universe/Tasks/Wait.h>

namespace Universe
{
//-----------------------------------------------------------------------------
TaskWait::TaskWait(double duration) :
    _remaining(duration)
{}
//-----------------------------------------------------------------------------
void TaskWait::update(double elapsed)
{
    _remaining -= elapsed;
}
//-----------------------------------------------------------------------------
bool TaskWait::internalIsFinished() const
{
    return _remaining <= 0.0;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
TaskWaitTag::TaskWaitTag(const Ptr<Node> & pNode, const String & tag, bool waitHasTag) :
    _pNode(pNode),
    _logical(WAIT_TAGS_AND),
    _waitHasTag(waitHasTag),
    _hasTag(!_waitHasTag)
{
    _tags.push_back(tag);
}
//-----------------------------------------------------------------------------
TaskWaitTag::TaskWaitTag(const Ptr<Node> & pNode, const Core::List<String> & tags, EWaitTagLogical logical) :
    _pNode(pNode),
    _tags(tags),
    _logical(logical),
    _waitHasTag(true),
    _hasTag(false)
{
}
//-----------------------------------------------------------------------------
void TaskWaitTag::update(double elapsed)
{
    if(_pNode != null && _pNode->isAlive())
    {
        switch(_logical)
        {
        case WAIT_TAGS_AND:
            {
                _hasTag = true;

                for(int32 ii=0; ii < _tags.size(); ii++)
                    _hasTag = _hasTag && _pNode->hasTag(_tags[ii]);

                break;
            }
        case WAIT_TAGS_OR:
            {
                _hasTag = false;

                for(int32 ii=0; ii < _tags.size(); ii++)
                    _hasTag = _hasTag || _pNode->hasTag(_tags[ii]);

                break;
            }
        } // switch
    }
}
//------------------------------------------------------------------------------
bool TaskWaitTag::internalIsFinished() const
{
    return !_pNode->isAlive() || (_hasTag == _waitHasTag);
}
//------------------------------------------------------------------------------
}
