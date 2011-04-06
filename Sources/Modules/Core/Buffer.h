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
#ifndef CORE_BUFFER_H_
#define CORE_BUFFER_H_

#include <Core/Standard.h>
#include <Core/IO/Streams.h>
#include <Core/IO/Tools.h>
#include <Core/List.h>


namespace Core
{
/**
 * Cette classe doit permettre de gérer des buffers de taille fixe.
 * Typiquement on peut utiliser des Core::List<byte> pour ce genre de situations
 * mais la classe List est suceptible d'allouer beaucoup plus de mémoire que nécessaire.
 *
 * \ingroup Containers
 */
class Buffer : public Object, public Serializable
{
public:
    Buffer(int32 size)
    :   _data(new byte[size]),
        _size(size)
    {}

    Buffer(InputStream & inputStream)
    :   _data(NULL),
        _size(0)
    {
        read(inputStream);
    }

    virtual ~Buffer()
    {
        delete [] _data;
        _data = NULL;
        _size = 0;
    }

    inline byte * getData() const { return _data; }

    inline int32 size() const { return _size; }

    virtual void read(InputStream & inputStream)
    {
        Core::List<byte> data;
        readAllStream(data, inputStream);

        if(_data != NULL)
            delete _data;

        _size = data.size();
        _data = new byte[_size];

        for(int32 ii=0; ii < _size; ii++)
        {
            _data[ii] = data[ii];
        }

    }

    virtual void write(OutputStream & outputStream) const
    {
        outputStream.writeData(_data, _size);
    }

    virtual String toString() const
    {
        String result(L"Buffer of size ");
        result << Core::toString(this->size());
        return result;
    }

private:
    byte *  _data;
    int32   _size;
};

//-----------------------------------------------------------------------------
inline String toString(const Buffer & buffer)
{
    String result(L"[");
    int32 length = buffer.size();
    if(length > 0)
    {
        const byte * data = buffer.getData();
        for(int32 ii=0; ii < length - 1; ii++)
        {
            result << indirectToString(data[ii]) << L", ";
        }
        result << indirectToString(data[length - 1]);
    }
    result << L"]";
    return result;
}
//-----------------------------------------------------------------------------
}//namespace Core


#endif /*CORE_BUFFER_H_*/
