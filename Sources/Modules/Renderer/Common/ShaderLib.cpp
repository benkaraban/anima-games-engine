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
#include <Renderer/IRenderer.h>
#include <Renderer/Common/ShaderLib.h>
#include <Core/IO/Tools.h>
#include <Core/Logger.h>
#include <Core/System/System.h>

namespace Renderer
{
class Includer : public Gfx::IShaderIncluder, public Core::Object
{
public:
    Includer(const Ptr<Core::VirtualFileSystem> & pVFS, const String & sourceDir) :
        _pVFS(pVFS),
        _sourceDir(sourceDir)
    {
    }

    virtual bool getIncludeData(const String & fileName, Core::List<byte> & data)
    {
        bool result = true;

        try
        {
            String srcFileName(_sourceDir);
            srcFileName << fileName;

            Ptr<Core::InputStream> pInput(_pVFS->readFile(srcFileName));

            if(pInput == null)
            {
                throw RendererException(L"file not found.");
            }
            else
            {
                Core::readAllStream(data, *pInput);
            }
        }
        catch(Core::Exception & exception)
        {
            WAR << L"Error including shader file '" << fileName << L"' : " << exception.getMessage() << L"\n";
            result = false;
        }

        return result;
    }

protected:
    Ptr<Core::VirtualFileSystem>  _pVFS;
    String                        _sourceDir;
};

ShaderLib::ShaderLib(const Ptr<Core::VirtualFileSystem> & pVFS, const String & sourceDir, const String & binariesDir,
          const Ptr<Gfx::IDevice> & pDevice) :
              _pVFS(pVFS),
              _pDevice(pDevice),
              _pIncluder(new Includer(pVFS, sourceDir)),
              _sourceDir(sourceDir),
              _binariesDir(binariesDir),
              _lastInclude(0)
{
    Core::List<String> files;
    _pVFS->listFiles(_sourceDir, files);

    for(Core::List<String>::const_iterator iFile = files.begin(); iFile != files.end(); ++iFile)
    {
        if(iFile->endsWith(L".sh"))
        {
            Core::TimeValue fileTime(_pVFS->getLastModificationDate(_sourceDir + *iFile));
            if(fileTime > _lastInclude)
                _lastInclude = fileTime;
        }
    }
}

Ptr<Gfx::IVertexShader> ShaderLib::getVShader(const String & name, Gfx::EVertexShaderVersion version, const String & entryPoint,
                                              const Core::List<Gfx::ShaderMacro> & macros)
{
    Ptr<Gfx::IVertexShader> pResult;
    String fullName(getShaderFullName(name, Gfx::toString(version), entryPoint, macros));

    String srcFileName(_sourceDir);
    srcFileName << name;

    bool retry = false;

    do
    {
        try
        {
            Core::Map<String, Ptr<Gfx::IVertexShader> >::const_iterator iShader = _vertexShaders.find(fullName);

            if(iShader != _vertexShaders.end())
            {
                pResult = iShader->second;
            }
            else
            {
                Ptr<Gfx::IVertexShaderBinObject> pBinObject;
                String binFileName(_binariesDir);
                binFileName << fullName;

                bool binExists = _pVFS->fileExists(binFileName);
                bool srcExists = _pVFS->fileExists(srcFileName);

                Core::TimeValue binTime(0);
                Core::TimeValue srcTime(0);

                if(binExists) binTime = _pVFS->getLastModificationDate(binFileName);
                if(srcExists) srcTime = _pVFS->getLastModificationDate(srcFileName);

                if((binExists && !srcExists) ||
                   (binExists &&  srcExists && (binTime >= srcTime) && (binTime >= _lastInclude)))
                {
                    Ptr<Core::InputStream> pBinFile(_pVFS->readFile(binFileName));
                    pBinObject = _pDevice->readPrecompiledVShader(*pBinFile);
                }
                else if(srcExists)
                {
                    INF << L"Compiling shader '" << srcFileName << L" as " << binFileName << L"'...\n";

                    Ptr<Core::InputStream> pSrcFile(_pVFS->readFile(srcFileName));
                    String source;
                    Core::readAllStream(source, *pSrcFile);
                    pBinObject = _pDevice->compileVShader(source, version, entryPoint, macros, _pIncluder);

                    Ptr<Core::OutputStream> pDstFile(_pVFS->writeFile(binFileName));
                    _pDevice->writePrecompiledVShader(*pDstFile, pBinObject);
                }
                else
                {
                    String message(L"File '");
                    message << srcFileName << L"' not found.";
                    throw RendererException(message);
                }

                pResult = _pDevice->createVertexShader(pBinObject);
                _vertexShaders[fullName] = pResult;
                retry = false;
            }
        }
        catch(Core::Exception & exception)
        {
            ERR << L"Unable to initialise vertex shader '" << name << L"' : " << exception.getMessage() << L"\n";

    #ifdef _DEBUG
            String boxMessage;
            boxMessage << L"Error compiling vertex shader '" << name << L"' : \n\n" << exception.getMessage() << L"\n";
            retry = Core::System::retryCancelMessageBox(boxMessage);
    #endif
            if(!retry)
            {
                String exMessage;
                exMessage << L"Vertex shader '" << name << L"' initialisation failed.";
                throw RendererException(exMessage);
            }
        }
    } while(retry);

    return pResult;
}

Ptr<Gfx::IPixelShader> ShaderLib::getPShader(const String & name, Gfx::EPixelShaderVersion version, const String & entryPoint,
                                             const Core::List<Gfx::ShaderMacro> & macros)
{
    Ptr<Gfx::IPixelShader> pResult;
    String fullName(getShaderFullName(name, Gfx::toString(version), entryPoint, macros));

    String srcFileName(_sourceDir);
    srcFileName << name;

    bool retry = false;

    do
    {
        try
        {
            Core::Map<String, Ptr<Gfx::IPixelShader> >::const_iterator iShader = _pixelShaders.find(fullName);

            if(iShader != _pixelShaders.end())
            {
                pResult = iShader->second;
            }
            else
            {
                Ptr<Gfx::IPixelShaderBinObject> pBinObject;
                String binFileName(_binariesDir);
                binFileName << fullName;

                bool binExists = _pVFS->fileExists(binFileName);
                bool srcExists = _pVFS->fileExists(srcFileName);

                Core::TimeValue binTime(0);
                Core::TimeValue srcTime(0);

                if(binExists) binTime = _pVFS->getLastModificationDate(binFileName);
                if(srcExists) srcTime = _pVFS->getLastModificationDate(srcFileName);

                if((binExists && !srcExists) ||
                   (binExists &&  srcExists && (binTime >= srcTime) && (binTime >= _lastInclude)))
                {
                    Ptr<Core::InputStream> pBinFile(_pVFS->readFile(binFileName));
                    pBinObject = _pDevice->readPrecompiledPShader(*pBinFile);
                }
                else if (_pVFS->fileExists(srcFileName))
                {
                    INF << L"Compiling shader '" << srcFileName << L" as " << binFileName << L"'...\n";

                    String source;
                    Ptr<Core::InputStream> pSrcFile(_pVFS->readFile(srcFileName));
                    Core::readAllStream(source, *pSrcFile);

                    pBinObject = _pDevice->compilePShader(source, version, entryPoint, macros, _pIncluder);

                    Ptr<Core::OutputStream> pDstFile(_pVFS->writeFile(binFileName));
                    _pDevice->writePrecompiledPShader(*pDstFile, pBinObject);
                }
                else
                {
                    String message(L"File '");
                    message << srcFileName << L"' not found.";
                    throw RendererException(message);
                }

                pResult = _pDevice->createPixelShader(pBinObject);
                _pixelShaders[fullName] = pResult;
                retry = false;
            }
        }
        catch(Core::Exception & exception)
        {
            ERR << L"Unable to initialise pixel shader '" << name << L"' : " << exception.getMessage() << L"\n";

    #ifdef _DEBUG
            String boxMessage;
            boxMessage << L"Error compiling pixel shader '" << name << L"' : \n\n" << exception.getMessage() << L"\n";
            retry = Core::System::retryCancelMessageBox(boxMessage);
    #endif

            if(!retry)
            {
                String exMessage;
                exMessage << L"Pixel shader '" << name << L"' initialisation failed.";
                throw RendererException(exMessage);
            }
        }
    } while(retry);

    return pResult;
}

String ShaderLib::getShaderFullName(const String & name, const String & version, const String & entryPoint,
                                    const Core::List<Gfx::ShaderMacro> & macros)
{
    String result(name);
    result << L"_" << version << L"_" << entryPoint;

    for(Core::List<Gfx::ShaderMacro>::const_iterator iMacro = macros.begin(); iMacro != macros.end(); ++iMacro)
        result << L"_" << iMacro->name << L"=" << iMacro->value;

    result << L".obj";
    return result;
}
}
