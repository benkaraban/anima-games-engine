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
#ifndef CORE_IOTOOLS_H_
#define CORE_IOTOOLS_H_

#include <Core/IO/Streams.h>
#include <Core/List.h>

namespace Core
{
LM_API_COR void readAllStream(Core::List<byte> & data, InputStream & input);
LM_API_COR void readAllStream(String8 & data, InputStream & input);
LM_API_COR void readAllStream(String & content, InputStream & input);

LM_API_COR String readTextStream(const Ptr<InputStream> & pStream);

template <class TElement>
inline void read(InputStream & input, List<TElement> & list)
{
    int32 size = input.readInt32();
    list.resize(size);
    for(typename List<TElement>::iterator iElement = list.begin(); iElement != list.end(); ++iElement)
        read(input, *iElement);
}

template <class TElement>
inline void readSafe(InputStream & input, List<TElement> & list, int32 maxSize)
{
    int32 size = input.readInt32();

    if(size > maxSize)
        throw Core::IOException(L"List readSafe error " + Core::toString(size) + L" items / " + Core::toString(maxSize));

    list.resize(size);
    for(typename List<TElement>::iterator iElement = list.begin(); iElement != list.end(); ++iElement)
        read(input, *iElement);
}

template <class TElement>
inline void write(OutputStream & output, const List<TElement> & list)
{
    int32 size = list.size();
    output.write(size);
    for(typename List<TElement>::const_iterator iElement = list.begin(); iElement != list.end(); ++iElement)
        write(output, *iElement);
}

}

#endif /*CORE_IOTOOLS_H_*/
