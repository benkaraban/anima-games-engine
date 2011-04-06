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
#ifndef ISELECTION_MANAGER_H_
#define ISELECTION_MANAGER_H_

#include <QObject>
#include <Core/Standard.h>
#include <EPI/Document/PropertySelection.moc.h>

namespace EPI
{
class Property;

//-----------------------------------------------------------------------------
class LM_API_EPI ISelectionManager : public QObject, public Core::Object
{
    Q_OBJECT

public:
    virtual ~ISelectionManager() {;}

    virtual void select(const Ptr<Property>& pPty) = 0;
    virtual void addToSelection(const Ptr<Property>& pPty) = 0;
    virtual void removeFromSelection(const Ptr<Property>& pPty) = 0;
    virtual void deselectAll() = 0;
    virtual void selectAll() = 0;
    virtual void invertSelection() = 0;

    virtual void configureAddToSelection(bool flag) = 0;
    virtual const Ptr<PropertySelection>&   getPtySelection() const = 0;

public Q_SLOTS:
    void selectChanged() {emit selectionChanged(getPtySelection()->getSelectedPties());}

Q_SIGNALS:
    void selectionChanged(const Core::List<Ptr<Property> >& ptiesSelected);

protected:
    ISelectionManager() {;}
};
//-----------------------------------------------------------------------------
} // namespace EPI


#endif //ISELECTION_MANAGER_H_