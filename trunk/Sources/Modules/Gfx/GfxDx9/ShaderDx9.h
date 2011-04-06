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
#ifndef SHADERDX9_H_
#define SHADERDX9_H_

#include <Core/Standard.h>
#include <Gfx/IShader.h>
#include <Gfx/IDevice.h>
#include <Gfx/GfxDx9/dx9Header.h>



namespace Gfx
{
//-----------------------------------------------------------------------------
ShaderConstantDesc D3DTypeConvert(D3DXCONSTANT_DESC& d3dDesc, const String & prefix);

//-----------------------------------------------------------------------------
class ShaderConstantTableDx9 : public ShaderConstantTable
{
public:
    ShaderConstantTableDx9(LPDIRECT3DDEVICE9 pDevice, IDevice * pIDevice, GfxStats & stats);
    ShaderConstantTableDx9(LPDIRECT3DDEVICE9 pDevice, IDevice * pIDevice, LPD3DXCONSTANTTABLE pConstTable, EShaderType type, GfxStats & stats);

    virtual int32 getConstantCount() const { return _consts.size(); };
    virtual const ShaderConstantDesc & getConstantDesc(int32 index) const { return _consts[index]; };
    virtual const ShaderConstantDesc & getConstantDesc(const String & name) const;

    virtual int32 getConstantIndex(const String & name) const;
    virtual int32 getConstantIndexIfExists(const String & name) const;

    virtual int32 getSamplerId(int32 index) const;

    virtual void setBurstMode(bool bm);
    virtual bool isBurstMode() const;
    virtual void flushBurst();

    virtual void setConstant(int32 index, bool value);
    virtual void setConstant(int32 index, int32 value);
    virtual void setConstant(int32 index, float value);
    virtual void setConstant(int32 index, const Core::Vector2f & value);
    virtual void setConstant(int32 index, const Core::Vector3f & value);
    virtual void setConstant(int32 index, const Core::Vector4f & value);
    virtual void setConstant(int32 index, const Core::Matrix4f & value);
    virtual void setConstantMat3x4(int32 index, const Core::Matrix4f & value);
    virtual void setConstantMat3x4Array(int32 index, const Core::Matrix4f * pValues, int32 count);

    virtual void setSampler1D(int32 index, const IShaderResourceViewPtr & pRes);
    virtual void setSampler2D(int32 index, const IShaderResourceViewPtr & pRes);
    virtual void setSampler3D(int32 index, const IShaderResourceViewPtr & pRes);
    virtual void setSamplerCube(int32 index, const IShaderResourceViewPtr & pRes);

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    void setConstant(const ShaderConstantDesc & cd, const bool * pValues);
    void setConstant(const ShaderConstantDesc & cd, const int32 * pValues);
    void setConstant(const ShaderConstantDesc & cd, const float * pValues);
    void setSampler(const ShaderConstantDesc & cd, const IShaderResourceViewPtr & pRes);

    void verifyConstType(const ShaderConstantDesc & vc, EShaderConstantType expected) const;
    void verifyConstType(const ShaderConstantDesc & vc, EShaderConstantType expected, EShaderConstantType fallback) const;
    void errorConstType(const ShaderConstantDesc & vc, EShaderConstantType assigned) const;

    LPDIRECT3DDEVICE9               _pDevice;
    IDevice *                       _pIDevice;
    GfxStats &                      _stats;
    EShaderType                     _type;
    Core::List<ShaderConstantDesc>  _consts;
    Core::Map<String, int32>        _constDic;
    bool                            _isBurstMode;
    bool                            _isBurstStarted;
    int32                           _burstStart;
    int32                           _burstEnd;
    Core::List<Core::Vector4f>      _burstRegs;
};
typedef Ptr<ShaderConstantTableDx9> ShaderConstantTableDx9Ptr;

//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
class LM_API_GX9 HLSLPixelShaderBinObjectDx9 : public IPixelShaderBinObject
{
public:
    HLSLPixelShaderBinObjectDx9(LPDIRECT3DDEVICE9 pDevice, IDevice * pIDevice, GfxStats & stats);
    HLSLPixelShaderBinObjectDx9(const ShaderConstantTablePtr & pConstTable, LPD3DXBUFFER pShaderData);

    virtual ShaderConstantTablePtr getConstantTable() const { return _pConstTable; };
    virtual const DWORD * getCodeData() const;

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    ShaderConstantTablePtr _pConstTable;
    Core::List<byte>       _shaderData;
};
typedef Ptr<IPixelShaderBinObject> IPixelShaderBinObjectPtr;

//-----------------------------------------------------------------------------
class LM_API_GX9 HLSLVertexShaderBinObjectDx9 : public IVertexShaderBinObject
{
public:
    HLSLVertexShaderBinObjectDx9(LPDIRECT3DDEVICE9 pDevice, IDevice * pIDevice, GfxStats & stats);
    HLSLVertexShaderBinObjectDx9(const ShaderConstantTablePtr & pConstTable, LPD3DXBUFFER pShaderData);

    virtual ShaderConstantTablePtr getConstantTable() const { return _pConstTable; };
    virtual const DWORD * getCodeData() const;

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    ShaderConstantTablePtr _pConstTable;
    Core::List<byte>       _shaderData;
};
typedef Ptr<IVertexShaderBinObject> IVertexShaderBinObjectPtr;

//-----------------------------------------------------------------------------
class D3DXIncluder : public ID3DXInclude
{
public:
    virtual ~D3DXIncluder() {};
    D3DXIncluder(const Ptr<IShaderIncluder> & pIncluder);
    STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
    STDMETHOD(Close)(THIS_ LPCVOID pData);

protected:
    Ptr<IShaderIncluder>  _pIncluder;
};

//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
class HLSLPixelShaderDx9 : public IPixelShader
{
public:
    HLSLPixelShaderDx9(const ShaderConstantTablePtr & pConstTable, LPDIRECT3DPIXELSHADER9 pShader);
    virtual ~HLSLPixelShaderDx9();

    virtual ShaderConstantTablePtr getConstantTable() const { return _pConstTable; };
    virtual LPDIRECT3DPIXELSHADER9 getShader() const { return _pShader; };

protected:
    ShaderConstantTablePtr   _pConstTable;
    LPDIRECT3DPIXELSHADER9   _pShader;
};
typedef Ptr<HLSLPixelShaderDx9 > HLSLPixelShaderDx9Ptr;

//-----------------------------------------------------------------------------
class HLSLVertexShaderDx9 : public IVertexShader
{
public:
    HLSLVertexShaderDx9(const ShaderConstantTablePtr & pConstTable, LPDIRECT3DVERTEXSHADER9 pShader);
    virtual ~HLSLVertexShaderDx9();
    virtual ShaderConstantTablePtr getConstantTable() const { return _pConstTable; };
    virtual LPDIRECT3DVERTEXSHADER9 getShader() const { return _pShader; };

protected:
    ShaderConstantTablePtr   _pConstTable;
    LPDIRECT3DVERTEXSHADER9  _pShader;
};
typedef Ptr<HLSLVertexShaderDx9 > HLSLVertexShaderDx9Ptr;
}  //namespace Gfx

#endif /* SHADERDX9_H_ */
