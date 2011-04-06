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
#ifndef GUI_DOCUMENTS_SELECTOR_H_
#define GUI_DOCUMENTS_SELECTOR_H_

#include <QToolButton>

#include <Core/Standard.h>
#include <Core/Map.h>

#include <EPI/Gui/GuiDocumentManager.h>

namespace EPI
{
class ControllerMain;
class GuiDocument;

 /**
 * 
 */
class LM_API_EPI GuiDocumentsSelector : public QToolButton
{
    Q_OBJECT

public:
    GuiDocumentsSelector(const Ptr<ControllerMain>& pCtrl, const String& title=L"Documents", QWidget * parent = 0);
    virtual ~GuiDocumentsSelector();

    virtual void mousePressEvent    (QMouseEvent* event);
    virtual void mouseReleaseEvent  (QMouseEvent* event);

private Q_SLOTS:
    void launchMenu();
    void makeDisable();
    void makeEnable();
    void actionSelected(QAction* pAction);

Q_SIGNALS:
    void signalActiveGuiDocument(int32 id);
     
private:
    Ptr<GuiDocumentManager>     _pGDocMng;
    Core::Map<int32, QAction*>  _docIdToAction;
    bool                        _isPressed;
/*
    virtual QAction * 	exec(QList<QAction *> actions, const QPoint & pos, QAction * at, QWidget * parent);
    virtual QAction * 	exec(QList<QAction *> actions, const QPoint & pos, QAction * at = 0);
    virtual QAction * 	exec ();
    virtual QAction * 	exec ( const QPoint & p, QAction * action = 0 );
    virtual void aboutToShow ();
    virtual void popup ( const QPoint & p, QAction * atAction);

private Q_SLOTS:
    void addGuiDocument(const Ptr<GuiDocument>& pGDoc);
    void removeGuiDocument(const Ptr<GuiDocument>& pGDoc);
    
    
    void selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc);



private:
    */
};


} // namespace EPI

#endif // PROPERTIES_PANEL_H_
