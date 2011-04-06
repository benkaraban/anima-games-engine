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
#ifndef DOCUMENT_TOOLS_H_
#define DOCUMENT_TOOLS_H_

#include <Core/Standard.h>

namespace Universe
{
    class World;
    class Node;
}

namespace EPI
{
class Property;
class DocumentRenderer;
class PtyNodeCamera;
class PtyNodeInstance;
class StateRecorder;

//-----------------------------------------------------------------------------
void universeNodesToProperties(const Ptr<Universe::World> pWorldInfoContent, const Ptr<Universe::World> pWorld, const Ptr<DocumentRenderer> & pDocumentRenderer, const Core::List<Ptr<Universe::Node> >& nodes, Property & ptyNode, Core::List<int32 >& generationPtyNode, const Ptr<StateRecorder>& pStateRecorder=null);
//-----------------------------------------------------------------------------
void saveGroup(const Ptr<Core::VirtualFileSystem>& pVFS, const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeGroup>& pNodeG, const Core::String & fileName);
//-----------------------------------------------------------------------------
LM_API_EPI void                         convertInstanceProperty (const Ptr<StateRecorder>& pStateRecorder, Ptr<PtyNodeInstance> & ptyToInstanciate, const Ptr<Universe::World> pWorldInfoContent, const Ptr<Universe::World> pWorld, const Ptr<DocumentRenderer> & pDocumentRenderer, Property & ptyNode, Core::List<int32 >& generationPtyNode);

LM_API_EPI void configWorldDeco(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldDeco);


LM_API_EPI void prepareWorldForRunning(Universe::World& world, const String& tagGamePlay = L"");
} // namespace EPI

#endif DOCUMENT_TOOLS_H_