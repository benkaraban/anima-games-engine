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
#ifndef GUI_WORLD_OVERLAY_H_
#define GUI_WORLD_OVERLAY_H_

#include <Core/Standard.h>
#include <Universe/World.h>


namespace EPI
{
class PropertySelection;


LM_ENUM_1(ENodeEditorType,
          NODE_SELECTION);

class LM_API_EPI NodeEditor
{
public:
    virtual ~NodeEditor() {;}
    virtual ENodeEditorType getNodeEditorType() const = 0;

protected:
    NodeEditor() {;}
};

class LM_API_EPI NodeSelection : public Universe::Node, public NodeEditor
{
public:
    NodeSelection(const Ptr<Renderer::IAABox> & pBox, Universe::World * pWorld);
    virtual ~NodeSelection();
    virtual ENodeEditorType getNodeEditorType() const { return NODE_SELECTION; }
};
//-----------------------------------------------------------------------------
/**
* 
*/
class LM_API_EPI WorldOverlay : public Universe::World
{
public:
    WorldOverlay(const Ptr<Renderer::IRenderer> & pRenderer,
          const Ptr<Core::VirtualFileSystem> & pVFS,
          const Ptr<Universe::RessourcePool> & pMasterPool = Ptr<Universe::RessourcePool>(null));
    virtual ~WorldOverlay();

    
    void updateSelection(const Ptr<PropertySelection>& pPtySelection);

    Ptr<Universe::NodeCamera> createCameraEdition();
    void setSelectedBoundingBoxVisible(bool flag);
    void configureOverlayForRendering(const Ptr<Universe::NodeCamera>& camera) const;

protected:
    bool    _showSelectedBoundingBoxVisible;
    Ptr<NodeSelection> createSelection(const Core::AABoxf& box);
    Core::List<Ptr<Universe::Node> >   _nodesSelected;
    Core::List<Ptr<Universe::Node> >    _memoNodeSelected;   //temporaire

    Core::Ptr<Universe::Node>          _globalAxis;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif GUI_WORLD_OVERLAY_H_