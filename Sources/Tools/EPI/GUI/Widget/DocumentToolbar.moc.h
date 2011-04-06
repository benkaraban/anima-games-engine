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
#ifndef DOCUMENT_TOOLBAR_H_
#define DOCUMENT_TOOLBAR_H_

#include <QToolBar>

#include <Core/Standard.h>


namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ControllerMain;
class GuiDocument;
class ToolsTransform;
class GuiDocumentAnimate;
class GuiDocumentRunStopScript;
class GuiDocumentRestart;
class PickingFilterWidget;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI DocumentToolBar : public QWidget, public Core::Object
{
    Q_OBJECT

public:
    DocumentToolBar(const Ptr<ControllerMain>& pCtrl, QWidget* parent = 0);
    ~DocumentToolBar();

private:
    void connectGuiDocument(bool makeConnection);
    void connectDocumentDefault(bool makeConnection);
    void connectDocumentParticles(bool makeConnection);
    void setupUi();
    void setupUiDocumentDefault();
    void setupUiDocumentScript();
    void setupUiDocumentWorld();
    void setupUiDocumentGroup();
    void setupUiDocumentSpline();

private Q_SLOTS:
    void resetDocumentToolBar();
    void selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc);

private:
    QGridLayout*                  _layout;
    Ptr<GuiDocument>              _pCurrentGDoc;
    Ptr<ToolsTransform>           _pToolsTransform;
    Ptr<GuiDocumentRestart>       _pRestart;
    Ptr<GuiDocumentAnimate>       _pGDocAnimate;
    Ptr<GuiDocumentRunStopScript> _pGDocRunStopScript;
    Ptr<PickingFilterWidget>      _pPickingFilter;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace EPI

#endif DOCUMENT_TOOLBAR_H_