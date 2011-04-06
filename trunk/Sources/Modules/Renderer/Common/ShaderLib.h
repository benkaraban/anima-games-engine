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
#ifndef RENDERER_SHADERLIB_H_
#define RENDERER_SHADERLIB_H_

#include <Core/VFS/VirtualFileSystem.h>
#include <Core/List.h>
#include <Gfx/IDevice.h>
#include <Gfx/IShader.h>

namespace Renderer
{
class LM_API_RDR ShaderLib
{
public:
    ShaderLib(const Ptr<Core::VirtualFileSystem> & pVFS,
              const String & sourceDir,
              const String & binariesDir,
              const Ptr<Gfx::IDevice> & pDevice);

    Ptr<Gfx::IVertexShader> getVShader(const String & name,
                                       Gfx::EVertexShaderVersion version,
                                       const String & entryPoint,
                                       const Core::List<Gfx::ShaderMacro> & macros);

    Ptr<Gfx::IPixelShader> getPShader(const String & name,
                                      Gfx::EPixelShaderVersion version,
                                      const String & entryPoint,
                                      const Core::List<Gfx::ShaderMacro> & macros);

protected:
    static String getShaderFullName(const String & name,
                                    const String & version,
                                    const String & entryPoint,
                                    const Core::List<Gfx::ShaderMacro> & macros);

    Core::Map<String, Ptr<Gfx::IVertexShader> > _vertexShaders;
    Core::Map<String, Ptr<Gfx::IPixelShader> >  _pixelShaders;

    Ptr<Core::VirtualFileSystem>  _pVFS;
    Ptr<Gfx::IDevice>             _pDevice;
    Ptr<Gfx::IShaderIncluder>     _pIncluder;
    String _sourceDir;
    String _binariesDir;

    Core::TimeValue _lastInclude;
};
}

#endif /* SHADERLIB_H_ */
