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
#ifndef IWIDGETMANAGEUNDOREDO_H_
#define IWIDGETMANAGEUNDOREDO_H_

#include <Core/Standard.h>

namespace EPI
{
//-----------------------------------------------------------------------------
class LM_API_EPI IWidgetManageUndoRedo : public QWidget, public Core::Object
{
    Q_OBJECT

public:
    virtual ~IWidgetManageUndoRedo() {}

protected Q_SLOTS:
    virtual void beginPropertyChange(QWidget* pWidget = NULL) = 0;
    virtual void finishPropertyChange(QWidget* pWidget = NULL) = 0;
    virtual void cancelPropertyChange() = 0;

    virtual void commitPropertyChange(QWidget* pWidget = NULL) = 0;

protected:
    IWidgetManageUndoRedo(QWidget * parent = 0) : QWidget(parent) {}
};
//-----------------------------------------------------------------------------
} // namespace EPI
#endif // IWIDGETMANAGEUNDOREDO_H_

