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
#include "ShaderDx9.h"
#include <Core/Logger.h>

#include <iostream>
#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/IO/Tools.h>
#include <Core/Math/MathTools.h>
#include <Gfx/GfxDx9/TextureDx9.h>

namespace Gfx
{
void makeConstantDesc(
        LPD3DXCONSTANTTABLE pConstTable,
        Core::List<ShaderConstantDesc> & consts,
        D3DXHANDLE pParent,
        const String & prefix,
        int32 index)
{
    D3DXHANDLE pD3DConstant = pConstTable->GetConstant(pParent, index);
    D3DXCONSTANT_DESC D3DConstantDesc;

    unsigned int numParams = 1;
    HRESULT hresult = pConstTable->GetConstantDesc(pD3DConstant, &D3DConstantDesc, &numParams);
    LM_HANDLE_D3D_ERROR(hresult, L"GetConstantDesc");

    String8 str8paramName(D3DConstantDesc.Name);
    // pour supprimer le caractere '$' qui apparait au debut du nom
    if (str8paramName.size() >= 1 && str8paramName.at(0) == '$')
        str8paramName.erase(str8paramName.begin());


    // pour supprimer les caractere [0] qui sont présent à la fin //TODO


    String paramName(str8paramName);

    if (D3DConstantDesc.Class == D3DXPC_STRUCT)
    {
        String prefixStruct = prefix + paramName + L".";

        for (int32 ii = 0; ii < int32(D3DConstantDesc.StructMembers); ++ii)
        {
            makeConstantDesc(pConstTable, consts, pD3DConstant, prefixStruct, ii);
        }
    }
    else
    {
        ShaderConstantDesc desc(D3DTypeConvert(D3DConstantDesc, prefix));
        if(desc.type != SHADER_PARAM_UNKNOWN)
            consts.push_back(desc);
    }
}
//-----------------------------------------------------------------------------
ShaderConstantTableDx9::ShaderConstantTableDx9(LPDIRECT3DDEVICE9 pDevice, IDevice * pIDevice, GfxStats & stats) :
    _pDevice(pDevice),
    _pIDevice(pIDevice),
    _stats(stats),
    _isBurstMode(false),
    _isBurstStarted(false)
{
}
//-----------------------------------------------------------------------------
ShaderConstantTableDx9::ShaderConstantTableDx9(LPDIRECT3DDEVICE9 pDevice, IDevice * pIDevice, LPD3DXCONSTANTTABLE pConstTable, EShaderType type, GfxStats & stats) :
    _pDevice(pDevice),
    _pIDevice(pIDevice),
    _stats(stats),
    _type(type),
    _isBurstMode(false),
    _isBurstStarted(false)
{
    D3DXCONSTANTTABLE_DESC constantTableDesc;
    HRESULT hresult = pConstTable->GetDesc(&constantTableDesc);
    LM_HANDLE_D3D_ERROR(hresult, L"GetConstantDesc");

    for(int32 ii = 0; ii < int32(constantTableDesc.Constants); ++ii)
        makeConstantDesc(pConstTable, _consts, NULL, String::EMPTY, ii);

    for(int32 ii=0; ii < _consts.size(); ii++)
        _constDic[_consts[ii].name] = ii;
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setBurstMode(bool bm)
{
    _isBurstMode = bm;
    if(_isBurstMode)
    {
        _isBurstStarted = false;
        _burstRegs.resize(256);
        for(int32 ii=0; ii < _burstRegs.size(); ii++)
        {
            _burstRegs[ii].r = f_INIT_TO_NAN;
            _burstRegs[ii].g = f_INIT_TO_NAN;
            _burstRegs[ii].b = f_INIT_TO_NAN;
            _burstRegs[ii].a = f_INIT_TO_NAN;
        }
    }
}
//-----------------------------------------------------------------------------
bool ShaderConstantTableDx9::isBurstMode() const
{
    return _isBurstMode;
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::flushBurst()
{
    if(_isBurstMode && _isBurstStarted)
    {
        int32 count = _burstEnd - _burstStart + 1;

        if(count >= 1)
        {
            switch(_type)
            {
            case VERTEX_SHADER:
            {
                _stats.setVSConstCount ++;
                HRESULT hresult = _pDevice->SetVertexShaderConstantF(_burstStart, (float*)&_burstRegs[_burstStart], count);
                LM_HANDLE_D3D_ERROR(hresult, L"SetVertexShaderConstantF");
                break;
            }
            case PIXEL_SHADER:
            {
                _stats.setPSConstCount ++;
                HRESULT hresult = _pDevice->SetPixelShaderConstantF(_burstStart, (float*)&_burstRegs[_burstStart], count);
                LM_HANDLE_D3D_ERROR(hresult, L"SetPixelShaderConstantF");
                break;
            }
            }
        }

        _isBurstStarted = false;
    }
}
//-----------------------------------------------------------------------------
const ShaderConstantDesc & ShaderConstantTableDx9::getConstantDesc(const String & name) const
{
    return _consts[getConstantIndex(name)];
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::read(Core::InputStream & inputStream)
{
    _consts.clear();
    _constDic.clear();

    _type = (EShaderType)inputStream.readInt32();
    Core::read(inputStream, _consts);

    for(int32 ii=0; ii < _consts.size(); ii++)
        _constDic[_consts[ii].name] = ii;
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::write(Core::OutputStream & outputStream) const
{
    outputStream.write((int32)_type);
    Core::write(outputStream, _consts);
}
//-----------------------------------------------------------------------------
int32 ShaderConstantTableDx9::getConstantIndex(const String & name) const
{
    Core::Map<String, int32>::const_iterator iConst = _constDic.find(name);

    if(iConst == _constDic.end())
    {
        String message(L"Unknown shader constant ");
        message << name;
        throw GfxException(message);
    }

    return iConst->second;
}
//-----------------------------------------------------------------------------
int32 ShaderConstantTableDx9::getConstantIndexIfExists(const String & name) const
{
    int32 result = UNDEFINED_SHADER_CONST;
    Core::Map<String, int32>::const_iterator iConst = _constDic.find(name);

    if(iConst != _constDic.end())
        result = iConst->second;

    return result;
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(int32 index, bool value)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_BOOL);
    LM_ASSERT(cd.registerCount == 1);
    setConstant(cd, &value);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(int32 index, int32 value)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_INT1);
    int32 values[4] = { value, 0, 0, 0 };
    LM_ASSERT(cd.registerCount == 1);
    setConstant(cd, values);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(int32 index, float value)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_FLOAT1);
    Core::Vector4f vec4(value, 0.0, 0.0, 1.0);
    LM_ASSERT(cd.registerCount == 1);
    setConstant(cd, &vec4._x);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(int32 index, const Core::Vector2f & value)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_FLOAT2);
    Core::Vector4f vec4(value.x, value.y, 0.0, 1.0);
    LM_ASSERT(cd.registerCount == 1);
    setConstant(cd, &vec4.x);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(int32 index, const Core::Vector3f & value)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_FLOAT3, SHADER_PARAM_FLOAT4);
    Core::Vector4f vec4(value._x, value._y, value._z, 1.0);
    LM_ASSERT(cd.registerCount == 1);
    setConstant(cd, &vec4._x);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(int32 index, const Core::Vector4f & value)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_FLOAT4);
    LM_ASSERT(cd.registerCount == 1);
    setConstant(cd, &value._x);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(int32 index, const Core::Matrix4f & value)
{
    const ShaderConstantDesc & cd = _consts[index];

    verifyConstType(cd, SHADER_PARAM_MATRIX_4X4);

    if(cd.isRowMajor)
    {
        setConstant(cd, value._val);
    }
    else
    {
        Core::Matrix4f transposed(value);
        transposed.transposeSelf();
        setConstant(cd, transposed._val);
    }
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstantMat3x4(int32 index, const Core::Matrix4f & value)
{
    const ShaderConstantDesc & cd = _consts[index];

    verifyConstType(cd, SHADER_PARAM_MATRIX_3X4);

    if(cd.isRowMajor)
    {
        LM_ASSERT(cd.registerCount == 3);
        setConstant(cd, value._val);
    }
    else
    {
        LM_ASSERT(cd.registerCount == 4);
        Core::Matrix4f transposed(value);
        transposed.transposeSelf();
        setConstant(cd, transposed._val);
    }
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstantMat3x4Array(int32 index, const Core::Matrix4f * pValues, int32 count)
{
    const ShaderConstantDesc & cd = _consts[index];

    verifyConstType(cd, SHADER_PARAM_MATRIX_3X4);

    if(count > cd.nbElements)
        throw GfxException(L"Shader constant array out of bound.");

    if(cd.isRowMajor)
    {
        Core::List<Core::Vector4f> regs;
        regs.reserve(3 * count);

        for(int32 ii=0; ii < count; ii++)
        {
            regs.push_back(Core::Vector4f(pValues[ii]._line[0]));
            regs.push_back(Core::Vector4f(pValues[ii]._line[1]));
            regs.push_back(Core::Vector4f(pValues[ii]._line[2]));
        }

        LM_ASSERT(cd.registerCount == (3 * cd.nbElements));
        LM_ASSERT(int32(regs.size()) <= cd.registerCount);

        _stats.setVSConstCount ++;
        HRESULT hresult = _pDevice->SetVertexShaderConstantF(cd.registerStart, &regs[0]._x, regs.size());
        LM_HANDLE_D3D_ERROR(hresult, L"SetVertexShaderConstantF");
    }
    else
    {
        Core::List<Core::Matrix4f> values;
        values.resize(count);

        for(int32 ii=0; ii < count; ii++)
            values[ii] = pValues[ii].getTransposed();

        LM_ASSERT(cd.registerCount == (4 * cd.nbElements));
        LM_ASSERT((4 * int32(values.size())) <= cd.registerCount);

        _stats.setVSConstCount ++;
        HRESULT hresult = _pDevice->SetVertexShaderConstantF(cd.registerStart, values[0]._val, 4 * values.size());
        LM_HANDLE_D3D_ERROR(hresult, L"SetVertexShaderConstantF");
    }
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(const ShaderConstantDesc & cd, const bool * pValues)
{
    Core::List<BOOL> values;
    values.resize(cd.registerCount);

    for(int32 ii=0; ii < cd.registerCount; ii++)
        values[ii] = pValues[ii] ? TRUE : FALSE;

    switch(_type)
    {
    case VERTEX_SHADER:
    {
		//TODO GCC->VC2008
        //HRESULT hresult = _pDevice->SetVertexShaderConstantB(cd.registerStart, values.begin(), cd.registerCount);
        _stats.setVSConstCount ++;
		HRESULT hresult = _pDevice->SetVertexShaderConstantB(cd.registerStart, &values[0], cd.registerCount);
        LM_HANDLE_D3D_ERROR(hresult, L"SetVertexShaderConstantB");
        break;
    }
    case PIXEL_SHADER:
    {
		//TODO GCC->VC2008
        //HRESULT hresult = _pDevice->SetPixelShaderConstantB(cd.registerStart, values.begin(), cd.registerCount);
        _stats.setPSConstCount ++;
		HRESULT hresult = _pDevice->SetPixelShaderConstantB(cd.registerStart, &values[0], cd.registerCount);
        LM_HANDLE_D3D_ERROR(hresult, L"SetPixelShaderConstantB");
        break;
    }
    }
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(const ShaderConstantDesc & cd, const int32 * pValues)
{
    switch(_type)
    {
    case VERTEX_SHADER:
    {
        _stats.setVSConstCount ++;
        HRESULT hresult = _pDevice->SetVertexShaderConstantI(cd.registerStart, pValues, cd.registerCount);
        LM_HANDLE_D3D_ERROR(hresult, L"SetVertexShaderConstantI");
        break;
    }
    case PIXEL_SHADER:
    {
        _stats.setPSConstCount ++;
        HRESULT hresult = _pDevice->SetPixelShaderConstantI(cd.registerStart, pValues, cd.registerCount);
        LM_HANDLE_D3D_ERROR(hresult, L"SetPixelShaderConstantI");
        break;
    }
    }
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setConstant(const ShaderConstantDesc & cd, const float * pValues)
{
    if(_isBurstMode)
    {
        int32 byteCount = 4 * 4 * cd.registerCount;

        if(!_isBurstStarted)
        {
            _burstStart = cd.registerStart;
            _burstEnd   = cd.registerStart + cd.registerCount - 1;

            _isBurstStarted = true;
        }
        else
        {
            _burstStart = std::min(_burstStart, cd.registerStart);
            _burstEnd   = std::max(_burstEnd,   cd.registerStart + cd.registerCount - 1);
        }

        memcpy(&_burstRegs[cd.registerStart], pValues, byteCount);
    }
    else
    {
        switch(_type)
        {
        case VERTEX_SHADER:
        {
            _stats.setVSConstCount ++;
            HRESULT hresult = _pDevice->SetVertexShaderConstantF(cd.registerStart, pValues, cd.registerCount);
            LM_HANDLE_D3D_ERROR(hresult, L"SetVertexShaderConstantF");
            break;
        }
        case PIXEL_SHADER:
        {
            _stats.setPSConstCount ++;
            HRESULT hresult = _pDevice->SetPixelShaderConstantF(cd.registerStart, pValues, cd.registerCount);
            LM_HANDLE_D3D_ERROR(hresult, L"SetPixelShaderConstantF");
            break;
        }
        }
    }
}
//-----------------------------------------------------------------------------
int32 ShaderConstantTableDx9::getSamplerId(int32 index) const
{
    const ShaderConstantDesc & cd = _consts[index];
    int32 slot = 0;

    switch(_type)
    {
    case VERTEX_SHADER: slot = cd.registerStart + (D3DDMAPSAMPLER+1); break;
    case PIXEL_SHADER:  slot = cd.registerStart; break;
    }

    return slot;
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setSampler(const ShaderConstantDesc & cd, const IShaderResourceViewPtr & pRes)
{
    int32 slot = 0;

    switch(_type)
    {
    case VERTEX_SHADER: slot = cd.registerStart + (D3DDMAPSAMPLER+1); break;
    case PIXEL_SHADER:  slot = cd.registerStart; break;
    }

    LPDIRECT3DBASETEXTURE9 pD3DTexture = NULL;

    if(pRes != null)
        pD3DTexture = (LM_DEBUG_PTR_CAST<ShaderResourceViewDx9>(pRes))->_pD3DBaseTexture;

    _pIDevice->tagTextureUnitAsUsed(slot);
    HRESULT hresult = _pDevice->SetTexture(slot, pD3DTexture);
    LM_HANDLE_D3D_ERROR(hresult, L"SetTextureSampler");
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setSampler1D(int32 index, const IShaderResourceViewPtr & pRes)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_SAMPLER1D);
    setSampler(cd, pRes);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setSampler2D(int32 index, const IShaderResourceViewPtr & pRes)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_SAMPLER2D);
    setSampler(cd, pRes);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setSampler3D(int32 index, const IShaderResourceViewPtr & pRes)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_SAMPLER3D);
    setSampler(cd, pRes);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::setSamplerCube(int32 index, const IShaderResourceViewPtr & pRes)
{
    const ShaderConstantDesc & cd = _consts[index];
    verifyConstType(cd, SHADER_PARAM_SAMPLERCUBE);
    setSampler(cd, pRes);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::verifyConstType(const ShaderConstantDesc & vc, EShaderConstantType type) const
{
    if(vc.type != type)
        errorConstType(vc, type);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::verifyConstType(const ShaderConstantDesc & vc, EShaderConstantType type, EShaderConstantType fallback) const
{
    if(vc.type != type && vc.type != fallback)
        errorConstType(vc, type);
}
//-----------------------------------------------------------------------------
void ShaderConstantTableDx9::errorConstType(const ShaderConstantDesc & vc, EShaderConstantType assigned) const
{
    String message;
    message << Gfx::toString(_type) << L" constant mismatch : '" << vc.name << L"' is declared as "
            << Gfx::toString(vc.type) << L" but assigned a "
            << Gfx::toString(assigned);
    throw GfxException(message);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
D3DXIncluder::D3DXIncluder(const Ptr<IShaderIncluder> & pIncluder) :
    _pIncluder(pIncluder)
{
}
//-----------------------------------------------------------------------------
HRESULT D3DXIncluder::Open(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
    HRESULT result = S_OK;

    try
    {
        String fileName(pFileName);
        Core::List<byte> content;

        if(_pIncluder != null && _pIncluder->getIncludeData(fileName, content))
        {
            byte * pData = new byte[content.size()];
			//TODO GCC-> VC 2008
            //memcpy(pData, content.begin(), content.size());
			memcpy(pData, &content[0], content.size());
            *ppData = pData;
            *pBytes = content.size();
        }
        else
        {
            result = D3DXERR_INVALIDDATA;
        }
    }
    catch(Core::Exception &)
    {
        result = D3DXERR_INVALIDDATA;
    }

    return result;
}
//-----------------------------------------------------------------------------
HRESULT D3DXIncluder::Close(LPCVOID pData)
{
    const byte * pDataByte = reinterpret_cast<const byte*>(pData);
    delete [] pDataByte;
    return S_OK;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
HLSLPixelShaderBinObjectDx9::HLSLPixelShaderBinObjectDx9(LPDIRECT3DDEVICE9 pDevice, IDevice * pIDevice, GfxStats & stats) :
    _pConstTable(new ShaderConstantTableDx9(pDevice, pIDevice, stats))
{
}
//-----------------------------------------------------------------------------
HLSLPixelShaderBinObjectDx9::HLSLPixelShaderBinObjectDx9(const ShaderConstantTablePtr & pConstTable, LPD3DXBUFFER pShaderData) :
    _pConstTable(pConstTable)
{
    int32 size = pShaderData->GetBufferSize();
    _shaderData.resize(size);
	//TODO GCC->VC2008
    //memcpy(_shaderData.begin(), pShaderData->GetBufferPointer(), size);
	memcpy(&_shaderData[0], pShaderData->GetBufferPointer(), size);
}
//-----------------------------------------------------------------------------
void HLSLPixelShaderBinObjectDx9::read(Core::InputStream & inputStream)
{
    _pConstTable->read(inputStream);
    Core::read(inputStream, _shaderData);
}
//-----------------------------------------------------------------------------
void HLSLPixelShaderBinObjectDx9::write(Core::OutputStream & outputStream) const
{
    _pConstTable->write(outputStream);
    Core::write(outputStream, _shaderData);
}
//-----------------------------------------------------------------------------
const DWORD * HLSLPixelShaderBinObjectDx9::getCodeData() const
{
	//TODO GCC->VC2008
    //return (const DWORD*)_shaderData.begin();
	return (const DWORD*)&_shaderData[0];
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
HLSLVertexShaderBinObjectDx9::HLSLVertexShaderBinObjectDx9(LPDIRECT3DDEVICE9 pDevice, IDevice * pIDevice, GfxStats & stats) :
    _pConstTable(new ShaderConstantTableDx9(pDevice, pIDevice, stats))
{
}
//-----------------------------------------------------------------------------
HLSLVertexShaderBinObjectDx9::HLSLVertexShaderBinObjectDx9(const ShaderConstantTablePtr & pConstTable, LPD3DXBUFFER pShaderData) :
    _pConstTable(pConstTable)
{
    int32 size = pShaderData->GetBufferSize();
    _shaderData.resize(size);
	//TO GCC->VC2008
    //memcpy(_shaderData.begin(), pShaderData->GetBufferPointer(), size);
	memcpy(&_shaderData[0], pShaderData->GetBufferPointer(), size);
}
//-----------------------------------------------------------------------------
void HLSLVertexShaderBinObjectDx9::read(Core::InputStream & inputStream)
{
    _pConstTable->read(inputStream);
    Core::read(inputStream, _shaderData);
}
//-----------------------------------------------------------------------------
void HLSLVertexShaderBinObjectDx9::write(Core::OutputStream & outputStream) const
{
    _pConstTable->write(outputStream);
    Core::write(outputStream, _shaderData);
}
//-----------------------------------------------------------------------------
const DWORD * HLSLVertexShaderBinObjectDx9::getCodeData() const
{
	//TO GCC->VC2008
    //return (const DWORD*)_shaderData.begin();
	return (const DWORD*)&_shaderData[0];
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
HLSLVertexShaderDx9::HLSLVertexShaderDx9(const ShaderConstantTablePtr & pConstTable, LPDIRECT3DVERTEXSHADER9 pShader) :
    _pConstTable(pConstTable),
    _pShader(pShader)
{
}
//-----------------------------------------------------------------------------
HLSLVertexShaderDx9::~HLSLVertexShaderDx9()
{
    if(_pShader != NULL)
    {
        LM_D3D_RELEASE(_pShader);
    }
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
HLSLPixelShaderDx9::HLSLPixelShaderDx9(const ShaderConstantTablePtr & pConstTable, LPDIRECT3DPIXELSHADER9 pShader) :
    _pConstTable(pConstTable),
    _pShader(pShader)
{
}
//-----------------------------------------------------------------------------
HLSLPixelShaderDx9::~HLSLPixelShaderDx9()
{
    if(_pShader != NULL)
    {
        LM_D3D_RELEASE(_pShader);
    }
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
ShaderConstantDesc D3DTypeConvert(D3DXCONSTANT_DESC& d3dDesc, const String & prefix)
{
	ShaderConstantDesc desc;
	desc.name = prefix + String(String8(d3dDesc.Name));
    desc.nbElements = d3dDesc.Elements;
    desc.registerStart = d3dDesc.RegisterIndex;
    desc.registerCount = d3dDesc.RegisterCount;
    desc.isRowMajor = (d3dDesc.Class == D3DXPC_MATRIX_ROWS);
    desc.bytesPerElement = 0;

    switch(d3dDesc.Type)
    {
    case D3DXPT_SAMPLER1D:
        desc.type = SHADER_PARAM_SAMPLER1D;
        break;
    case D3DXPT_SAMPLER:
        WAR << L"Shader sampler " << desc.name << L" type is not specified (2D assumed).\n";
    case D3DXPT_SAMPLER2D:
        desc.type = SHADER_PARAM_SAMPLER2D;
        break;
    case D3DXPT_SAMPLER3D:
        desc.type = SHADER_PARAM_SAMPLER3D;
        break;
    case D3DXPT_SAMPLERCUBE:
        desc.type = SHADER_PARAM_SAMPLERCUBE;
        break;
    case D3DXPT_BOOL:
        desc.type = SHADER_PARAM_BOOL;
        desc.bytesPerElement = 4;
        break;
    case D3DXPT_INT:
        switch(d3dDesc.Columns)
        {
        case 1:
            desc.type = SHADER_PARAM_INT1;
            desc.bytesPerElement = 4;
            break;
        case 2:
            desc.type = SHADER_PARAM_INT2;
            desc.bytesPerElement = 4;
            break;
        case 3:
            desc.type = SHADER_PARAM_INT3;
            desc.bytesPerElement = 4;
            break;
        case 4:
            desc.type = SHADER_PARAM_INT4;
            desc.bytesPerElement = 4;
            break;
        }
        break;
    case D3DXPT_FLOAT:
        switch(d3dDesc.Rows)
        {
        case 1:
            switch(d3dDesc.Columns)
            {
            case 1:
                desc.type = SHADER_PARAM_FLOAT1;
                desc.bytesPerElement = 4;
                break;
            case 2:
                desc.type = SHADER_PARAM_FLOAT2;
                desc.bytesPerElement = 4;
                break;
            case 3:
                desc.type = SHADER_PARAM_FLOAT3;
                desc.bytesPerElement = 4;
                break;
            case 4:
                desc.type = SHADER_PARAM_FLOAT4;
                desc.bytesPerElement = 4;
                break;
            }
            break;
        case 2:
            switch(d3dDesc.Columns)
            {
            case 2:
                desc.type = SHADER_PARAM_MATRIX_2X2;
                desc.bytesPerElement = 8;
                break;
            case 3:
                desc.type = SHADER_PARAM_MATRIX_2X3;
                desc.bytesPerElement = 8;
                break;
            case 4:
                desc.type = SHADER_PARAM_MATRIX_2X4;
                desc.bytesPerElement = 8;
                break;
            }
            break;
        case 3:
            switch(d3dDesc.Columns)
            {
            case 2:
                desc.type = SHADER_PARAM_MATRIX_3X2;
                desc.bytesPerElement = 12;
                break;
            case 3:
                desc.type = SHADER_PARAM_MATRIX_3X3;
                desc.bytesPerElement = 12;
                break;
            case 4:
                desc.type = SHADER_PARAM_MATRIX_3X4;
                desc.bytesPerElement = 12;
                break;
            }
            break;
        case 4:
            switch(d3dDesc.Columns)
            {
            case 2:
                desc.type = SHADER_PARAM_MATRIX_4X2;
                desc.bytesPerElement = 16;
                break;
            case 3:
                desc.type = SHADER_PARAM_MATRIX_4X3;
                desc.bytesPerElement = 16;
                break;
            case 4:
                desc.type = SHADER_PARAM_MATRIX_4X4;
                desc.bytesPerElement = 16;
                break;
            }
            break;
        }
        break;
    default:
        desc.type = SHADER_PARAM_UNKNOWN;
        break;
    }

    return desc;
};
//-----------------------------------------------------------------------------
}  //namespace Gfx
