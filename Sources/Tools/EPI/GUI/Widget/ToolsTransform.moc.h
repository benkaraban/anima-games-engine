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
#ifndef TOOLS_TRANSFORM_H_
#define TOOLS_TRANSFORM_H_

#include <QtToolbox/PushButtonMenu.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class IViewportTool;
class GuiDocument;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI ToolsTransform : public QtToolbox::PushButtonMenu
{
    Q_OBJECT

public:
    ToolsTransform(Ptr<GuiDocument>& pGDoc, QWidget* parent = 0);
    ~ToolsTransform();

private:
    void connectSignals(bool flag);

private Q_SLOTS:
    void internalToolChanged(int32 index, bool isChecked);
    void guiDocChangeVTool(const Ptr<IViewportTool>& pVTool);

Q_SIGNALS:
    void signalToolChanged(const Ptr<IViewportTool>& pVTool);

private:
    Ptr<GuiDocument>& _pGDoc;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace EPI

#endif TOOLS_TRANSFORM_H_