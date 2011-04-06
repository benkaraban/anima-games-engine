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
#include "7zCoder.h"

#include <Core/Logger.h>
#include <Core/Exception.h>

#include <lzma465/CPP/Common/MyWindows.h>
#include <lzma465/CPP/Common/MyInitGuid.h>
#include <lzma465/CPP/7zip/Compress/LzmaDecoder.h>
#include <lzma465/CPP/7zip/Compress/LzmaEncoder.h>


class InStream7z: public ISequentialInStream, public CMyUnknownImp
{
public:
    InStream7z(Core::InputStream & input): in (input) {}
    virtual ~InStream7z(){}

    STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize)
    {
        HRESULT ret = S_FALSE;

        try
        {
            int32 readCount = 0;

            if (!in.eof())
                readCount = in.readUpTo(data, size);

            if(processedSize != NULL)
                *processedSize = readCount;

            ret = S_OK;
        }
        catch(Core::IOException e)
        {
            ERR << L"InStream7z::Read error InputStream readUpTo throw an execption  in lzma coder processing";
        }

        return ret;
    }

    STDMETHOD(ReadPart)(void *data, UInt32 size, UInt32 *processedSize)
    {
        return Read(data, size, processedSize);
    }

    virtual HRESULT STDMETHODCALLTYPE   QueryInterface(const GUID&, void**)     {return 0;}
    virtual ULONG STDMETHODCALLTYPE     AddRef()                                {return 0;}
    virtual ULONG STDMETHODCALLTYPE     Release()                               {return 0;}

private:
    Core::InputStream & in;
};

class OutStream7z: public ISequentialOutStream, public CMyUnknownImp
{
public:
    OutStream7z(Core::OutputStream & output) : out(output) {}
    virtual ~OutStream7z(){}

    STDMETHOD(Write)(const void *data, UInt32 size, UInt32 *processedSize)
    {
        HRESULT ret = S_FALSE;

        try
        {
            out.writeData(data, size);

            if(processedSize != NULL)
                *processedSize = size;

            ret = S_OK;
        }
        catch(Core::IOException e)
        {
            ERR << L"InStream7z::Write error OutputStream writeData throw an execption  in lzma coder processing";
        }

        return ret;
    }

    STDMETHOD(WritePart)(const void *data, UInt32 size, UInt32 *processedSize)
    {
        return Write(data, size, processedSize);
    }

    virtual HRESULT STDMETHODCALLTYPE   QueryInterface(const GUID&, void**) {return 0;}
    virtual ULONG STDMETHODCALLTYPE     AddRef()                            {return 0;}
    virtual ULONG STDMETHODCALLTYPE     Release()                           {return 0;}

private:
    Core::OutputStream & out; 
};

class CompressProgressInfo7z: public ICompressProgressInfo
{
public:
    CompressProgressInfo7z(Ptr<Core::IProgressObs> pObs, Core::InputStream & input) : _pObs(pObs), _totalSize(0)
    {
        Core::SeekableInputStream* pSeek = dynamic_cast<Core::SeekableInputStream*>(&input);
        if (pSeek!=NULL)
            _totalSize = pSeek->getLength() - pSeek->getPosition();
    }

    STDMETHOD(SetRatioInfo)(const UInt64 *inSize, const UInt64 *outSize)
    {
        if (_totalSize!=0)
        {
            float ratio = *inSize / float(_totalSize);
            _pObs->progress(ratio);
        }

        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE    QueryInterface(const GUID&, void**) {return 0;}
    virtual ULONG STDMETHODCALLTYPE      AddRef()                            {return 0;}
    virtual ULONG STDMETHODCALLTYPE      Release()                           {return 0;}

private:
    Ptr<Core::IProgressObs> _pObs;
    int64 _totalSize;
};

namespace Core
{

void LzmaEncoder(InputStream & input, OutputStream & output, Ptr<IProgressObs> pObs)
{
    CMyComPtr<ISequentialInStream>   inStream  = new InStream7z(input);
    CMyComPtr<ISequentialOutStream>  outStream = new OutStream7z(output);
    CMyComPtr<ICompressProgressInfo> info      = NULL;
    if (pObs!=null)
        info = new CompressProgressInfo7z(pObs, input);

    NCompress::NLzma::CEncoder *encoder= new NCompress::NLzma::CEncoder;

    UInt32 algorithm = 2;               // max compression
    UInt32 dictionary = 1 << 23;        // 8 Meg dictionary
    bool eos = true;

    PROPID propIDs[] =
    {
      NCoderPropID::kDictionarySize,
      NCoderPropID::kAlgorithm,
      NCoderPropID::kEndMarker
    };

    const int kNumProps = sizeof(propIDs) / sizeof(propIDs[0]);
    PROPVARIANT properties[kNumProps];
    properties[0].vt      = VT_UI4;
    properties[0].ulVal   = UInt32(dictionary); properties[1].vt = VT_UI4;
    properties[1].ulVal   = UInt32(algorithm);
    properties[2].vt      = VT_BOOL;
    properties[2].boolVal = eos ? VARIANT_TRUE : VARIANT_FALSE;

    if (encoder->SetCoderProperties(propIDs, properties, kNumProps) != S_OK)
        throw Exception (L"Unable to set properties");

    encoder->WriteCoderProperties(outStream);

    if( encoder->Code(inStream, outStream, 0, 0, info) != S_OK)
        throw Exception (L"Failed to compress data");
}

void LzmaDecoder(InputStream & input, OutputStream & output, Ptr<IProgressObs> pObs)
{
    CMyComPtr<ISequentialInStream> inStream   = new InStream7z(input);
    CMyComPtr<ISequentialOutStream> outStream = new OutStream7z(output);
    CMyComPtr<ICompressProgressInfo> info      = NULL;
    if (pObs!=null)
        info = new CompressProgressInfo7z(pObs, input);

    NCompress::NLzma::CDecoder *decoder= new NCompress::NLzma::CDecoder;

    const UInt32 kPropertiesSize = 5;
    Byte properties[kPropertiesSize];
    UInt32 processedSize;

    if (inStream->Read(properties, kPropertiesSize, &processedSize) != S_OK)
        throw Exception (L"Failed to read properties");

    if (processedSize != kPropertiesSize)
        throw Exception (L"Failed to read properties");

    if (decoder->SetDecoderProperties2(properties, kPropertiesSize) != S_OK)
        throw Exception (L"Unable to set decoder propertied");

    if (decoder->Code(inStream, outStream, 0, 0, info) != S_OK)
        throw Exception (L"Failed to decompress data\n");
}

}
