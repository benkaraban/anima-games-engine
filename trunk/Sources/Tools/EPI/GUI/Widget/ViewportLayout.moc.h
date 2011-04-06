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
#ifndef VIEWPORT_LAYOUT_H_
#define VIEWPORT_LAYOUT_H_


#include <QSplitter>

#include <Core/Standard.h>
#include <Core/List.h>


#include <EPI/GUI/Viewport/WorldViewport.moc.h>
#include <EPI/Document/GuiDescription.h>


namespace EPI
{
//-----------------------------------------------------------------------------
class ControllerMain;
class GuiDocument;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
*
*/
class LM_API_EPI ViewportLayout : public QSplitter 
{
    Q_OBJECT

public:
    ViewportLayout(const Ptr<ControllerMain>& pCtrl, QWidget * parent=0);
    virtual ~ViewportLayout();
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

public Q_SLOTS:
    void initialize();

protected:
    void myContextMenu(QMouseEvent* event);

private:
    bool navigationViewportsIsInAction() const;

private Q_SLOTS:
    void clearAllViewports();
    void selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc);
    void noGuiDocument();

Q_SIGNALS:
    void signalTextInfo(const String& str);

private:
    QSplitter *topSplitter;
    QSplitter *botSplitter;
    Core::List<Ptr<WorldViewportContainer> > _viewports;
    Ptr<GuiDocument>    _pCurrentGDoc;
    EViewportLayoutMode _mode;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif // VIEWPORT_LAYOUT_H_