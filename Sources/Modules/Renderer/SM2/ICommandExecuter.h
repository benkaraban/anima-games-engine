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
#ifndef RENDERER_ICOMMAND_EXECUTER_H_
#define RENDERER_ICOMMAND_EXECUTER_H_

#include <Renderer/SM2/RenderContext.h>

namespace Renderer
{
LM_ENUM_4(ECommandMode,
            CMD_SOLID,
            CMD_SKYSP,
            CMD_WATER,
            CMD_TRANS
            );

class ICommandExecuter;

struct Command
{
public:
    ERenderPass         pass;
    ECommandMode        mode;
    float               camDist;
    ICommandExecuter *  pExecuter;
    void *              pExecData;
    void *              pExecData2;
    int32               flags;

    bool operator < (const Command & c) const
    {
        if(pass != c.pass)
            return pass < c.pass;
        else if(mode != c.mode)
            return mode < c.mode;
        else if(mode == CMD_SOLID)
            return int32(pExecData2) < int32(c.pExecData2);
        else // mode == CMD_TRANS
            return c.camDist < camDist;
    }
};


class ICommandExecuter
{
public:
    virtual ~ICommandExecuter() {};
    virtual void startContext(const RenderContext & context, ERenderPass pass) = 0;
    virtual void endContext() = 0;
    
    virtual void enqueueCommands(Core::List<Command> & commands) = 0;
    virtual void exec(Command * pStart, Command * pEnd) = 0;
};

void initCommands(const Core::List<Ptr<ICommandExecuter> > & executers, Core::List<Command> & commands, const RenderContext & context, ERenderPass pass);
void execCommands(const Core::List<Ptr<ICommandExecuter> > & executers, Core::List<Command> & commands);

}

#endif
