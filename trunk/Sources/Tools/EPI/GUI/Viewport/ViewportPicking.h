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
#ifndef VIEWPORT_PICKING_H_
#define VIEWPORT_PICKING_H_

#include <EPI/Gui/Viewport/IViewportPicking.h>


namespace EPI
{
class GuiDocument;

class ViewportPickingOnGuiDocument : public IViewportPicking
{
public:
    ViewportPickingOnGuiDocument(const Ptr<GuiDocument>& pGDoc);
    virtual ~ViewportPickingOnGuiDocument();

    virtual bool rayToProperty(const Core::Rayf& ray, Ptr<Property>& pPty);
    virtual bool selectNext(const Core::List<Ptr<Property> >& pPtyCurrent, const Core::Rayf& ray, Ptr<Property>& pPty);
private:
    Ptr<GuiDocument>   _pGDoc;
};

//picking pour l'edition d'un ptyNode
class ViewportPickingOnPty : public IViewportPicking
{
public:
    ViewportPickingOnPty(const Ptr<Property>& pPty);
    virtual ~ViewportPickingOnPty();

    virtual bool rayToProperty(const Core::Rayf& ray, Ptr<Property>& pPty);
    virtual bool selectNext(const Core::List<Ptr<Property> >& pPtyCurrent, const Core::Rayf& ray, Ptr<Property>& pPty);

private:
    Ptr<Property>   _pPty;
};

} // namespace EPI

#endif