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
#ifndef WORLD_SCRIPTABLE_H_
#define WORLD_SCRIPTABLE_H_


#include <QObject>
#include <Core/Standard.h>


namespace Universe
{
    class World;
}

namespace EPI
{
class Property;
class LuaOutput;
class DocumentWorld;
class DocumentRenderer;

//-----------------------------------------------------------------------------
class LM_API_EPI IWorldScriptable : public QObject, public Core::Object
{
    Q_OBJECT
public:
    IWorldScriptable() {}
    virtual ~IWorldScriptable() {}

    virtual bool                            isReadyForAnimation()   const = 0;
    virtual const Ptr<Universe::World>&     getWorldForAnimation()  const = 0;
    virtual const Ptr<LuaOutput>&           getLuaOutput()          const = 0;
    virtual void runScript  (const Core::String8   script, const String& name, const String& tagGamePlay = L"") = 0;
    virtual void startWorldScripts() = 0;
    virtual bool isRunning() const = 0;


public Q_SLOTS:
    virtual void stopScript () = 0;

Q_SIGNALS:
    void scriptStarted();
    void scriptStopped();
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif WORLD_SCRIPTABLE_H_

