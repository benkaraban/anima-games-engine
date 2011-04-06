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
#ifndef PROPERTIES_PANEL_H_
#define PROPERTIES_PANEL_H_

#include <QWidget>


class QVBoxLayout;

namespace Core
{
template <class TElement> class List;
}

namespace EPI
{
class ControllerMain;
class GuiDocument;
class PropertyWidget;
class Property;
class IDocumentWidget;
//-----------------------------------------------------------------------------
 /**
 * Decrit le panel recevant les propriete widget
 */
class LM_API_EPI PropertiesPanel : public QWidget
{
    Q_OBJECT

public:
    PropertiesPanel(const Ptr<ControllerMain>& pCtrl, QWidget * parent = 0);
    virtual ~PropertiesPanel();

    virtual void setupUi();
    void cleanPanel();

protected Q_SLOTS:
    void clear();
    void selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc);
    void changeSelection(const Core::List<Ptr<Property> >& pties);
    //void selectActiveProperty(const Ptr<Property>& pP);
    void documentEditingChange(bool isEditing);

private:
    Ptr<GuiDocument> _pGDoc;
    Ptr<PropertyWidget> _pPW;
    Ptr<IDocumentWidget> _pDW;
    QVBoxLayout *   _layout;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif // PROPERTIES_PANEL_H_
