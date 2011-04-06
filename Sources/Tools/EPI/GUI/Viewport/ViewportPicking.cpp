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
#include <EPI/Gui/Viewport/ViewportPicking.h>

#include <EPI/Document/Properties/PtyDocEdition.moc.h>
#include <EPI/Document/PropertySelection.moc.h>

#include <EPI/Gui/GuiDocument.moc.h>
#include <EPI/Gui/GuiContext.moc.h>
#include <EPI/Document/DocumentBase.moc.h>
#include <EPI/Document/PropertyNode.moc.h>
#include <Universe/World.h>
namespace EPI
{
struct propertyIntersected
{
    propertyIntersected() : id(-1), distance(FLT_MAX), isSelected(false){}
    int32   id;
    float   distance;
    bool    isSelected; // ce flag sera mis a true si la pty est deja selectionne


    friend bool operator < (const propertyIntersected left, const propertyIntersected & right)  {return left.distance<right.distance;}
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void findPropertiesIntersected_travel(const GuiContext& guiContext, const Ptr<Property>& pPropertyNode, const Core::Rayf& ray, Core::Set<propertyIntersected>& ptiesIntersected)
{
    float distance = FLT_MAX;

    if (guiContext.propertyIsSelectable(*pPropertyNode) == true)
    {
        if (pPropertyNode->isIntersecting(ray, distance)==true)
        {
            propertyIntersected inter;
                inter.distance = distance;
                inter.id = pPropertyNode->getId();

            ptiesIntersected.insert(inter);
        }
    }

    for (int32 iChild=0; iChild<pPropertyNode->getNbChildren(); ++iChild)
    {
        findPropertiesIntersected_travel(guiContext, pPropertyNode->getChild(iChild), ray, ptiesIntersected);
    }
}
//-----------------------------------------------------------------------------
LM_API_EPI void findPropertiesIntersected(const GuiContext& guiContext, const Property& pPtyRoot, const Core::Rayf& ray, Core::Set<propertyIntersected>& ptiesIntersected)
{
    ptiesIntersected.clear();

    for (int32 iChild=0; iChild<pPtyRoot.getNbChildren(); ++iChild)
    {
        findPropertiesIntersected_travel(guiContext, pPtyRoot.getChild(iChild), ray, ptiesIntersected);
    }

    if (pPtyRoot.hasFlag(FLAG_PTY_EDITABLE_CONTENT))
    {
       const Core::List<Ptr<Property> >& ptiesContent =  static_cast<const PtyNodeEditableContent&> (pPtyRoot).getPtiesContentSelectable();
       for (int32 iPty=0; iPty<ptiesContent.size(); ++iPty)
       {
            findPropertiesIntersected_travel(guiContext, ptiesContent[iPty], ray, ptiesIntersected);
       }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewportPickingOnGuiDocument::ViewportPickingOnGuiDocument(const Ptr<GuiDocument>& pGDoc) :
    _pGDoc(pGDoc)
{
}
//-----------------------------------------------------------------------------
ViewportPickingOnGuiDocument::~ViewportPickingOnGuiDocument()
{
}
//-----------------------------------------------------------------------------
bool ViewportPickingOnGuiDocument::rayToProperty(const Core::Rayf& ray, Ptr<Property>& pPty)
{
    Ptr<Universe::Node> nodeIntersecting;
    float distance;
    bool result = false;

    //on test avec le WorldInfoContent
    result = _pGDoc->getDocument()->getWorldInfoContent()->rayIntersection(ray, nodeIntersecting, distance);

    if (result == true)
    {
        int32 idPty = _pGDoc->getDocument()->getPropertyIdFromUniverseNode(nodeIntersecting);
        pPty = _pGDoc->getDocument()->getProperty(idPty);
        if (pPty == null)
        {
            result = false;
        }
    }
    else
    {
        //result = _pGDoc->getGuiWorld()->rayIntersection(ray, nodeIntersecting, distance);
        Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (_pGDoc->getDocument()->getPropertyDocumentEdition());
        Ptr<PropertySelectionRoot> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelectionRoot> (pPtyDocEdit->getPtySelection());

        intersect_RayPropertyNodeRecursive(pPtySelection->getRootPtySelectable(), ray, distance, pPty);
        if (pPty!=null)
        {
            result = true;
        }
/*
        if (result == true)
        {
            int32 idPty = _pGDoc->getDocument()->getPropertyIdFromUniverseNode(nodeIntersecting);
            pPty = _pGDoc->getDocument()->getProperty(idPty);
            if (pPty == null)
            {
                result = false;
            }
        }*/

        if (result == false)
        {
            pPty = _pGDoc->getDocument()->getDefaultProperty();
            if (pPty!=null)
            {
                result = true;
            }
        }
    }

    return result;
}
//-----------------------------------------------------------------------------
bool ViewportPickingOnGuiDocument::selectNext(const Core::List<Ptr<Property> >& ptiesSelected, const Core::Rayf& ray, Ptr<Property>& pPty)
{
    pPty = null;
    Core::Set<propertyIntersected> ptiesIntersected;
    float maxDistance = -FLT_MAX;   //distance de la pty selectionne la plus eloigne

    Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (_pGDoc->getDocument()->getPropertyDocumentEdition());
    Ptr<PropertySelectionRoot> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelectionRoot> (pPtyDocEdit->getPtySelection());

    findPropertiesIntersected(*_pGDoc->getGuiContext(), pPtySelection->getRootPtySelectable(),ray, ptiesIntersected);

    if (ptiesIntersected.size() == 0)   return false;

    //on marque les pty deja selectionnees
    Core::Set<propertyIntersected>::iterator it = ptiesIntersected.begin();
    if (ptiesSelected.size()>0)
    {
        for (; it!=ptiesIntersected.end(); ++it)
        {
            for (int32 iSelected=0; iSelected<ptiesSelected.size(); ++iSelected)
            {
                if (ptiesSelected[iSelected]->getId() == it->id)
                {
                    it->isSelected = true;
                    if (it->distance > maxDistance)
                        maxDistance = it->distance;
                }
                else    //on garde la pty la plus proche
                {
                    if (pPty == null)
                        pPty = _pGDoc->getDocument()->getProperty(it->id);
                }
            }
        }
    }

    //on recherche la pty situe juste apres la pty selectionne la plus eloigne ( > maxDistance)
    
    for (it = ptiesIntersected.begin(); it!=ptiesIntersected.end(); ++it)
    {
        if (it->isSelected == false && it->distance > maxDistance)
        {
            pPty = _pGDoc->getDocument()->getProperty(it->id);
            break;
        }
    }

    return (pPty != null);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewportPickingOnPty::ViewportPickingOnPty(const Ptr<Property>& pPty):
    _pPty(pPty)
{

}
//-----------------------------------------------------------------------------
bool ViewportPickingOnPty::selectNext(const Core::List<Ptr<Property> >& pPtyCurrent, const Core::Rayf& ray, Ptr<Property>& pPty)
{
    //TODO pour le moment on gere ici le cas classique
    return rayToProperty(ray, pPty);
}
//-----------------------------------------------------------------------------
ViewportPickingOnPty::~ViewportPickingOnPty()
{
}
//-----------------------------------------------------------------------------
bool ViewportPickingOnPty::rayToProperty(const Core::Rayf& ray, Ptr<Property>& pPty)
{
    Ptr<Property> pPtyIntersecting;
    float distance;

    if (_pPty!=null)
    {
        intersect_RayPropertyNodeRecursive(*_pPty, ray, distance, pPty);
    }

    return pPty!=null;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI