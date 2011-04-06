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
#ifndef STANDARDSTREAM_H_
#define STANDARDSTREAM_H_


#include <stdio.h>
#include <iostream>
#include <Core/NonCopyable.h>
#include <Core/IO/Streams.h>

#define LM_COUT_PTR Core::stdoutStream::getPtr()
#define LM_COUT (*LM_COUT_PTR)
const Core::String STDCOUT_STREAM_NAME (L"STDCOUT_NAME");

namespace Core
{
/**
*Le fichier contient la définition des flux standard d'entrée et de sortie des données du programme
*cout, cin, cerr, clog
*/

/**
*le stream associé à la sortie standard : stdout
*/
class LM_API_COR stdoutStream : public Object, public OutputStream
{
protected:
    stdoutStream();
    static Core::Ptr<stdoutStream> sInstance;

public:
    static Core::Ptr<stdoutStream> getPtr();
    virtual ~stdoutStream();
    virtual void writeData(const void * buffer, int32 size);
    virtual void flush();
    virtual void close();
    template <class TElement>
    stdoutStream& operator << (const TElement & element);
};


//-----------------------------------------------------------------------------
template <class TElement>
stdoutStream& stdoutStream::operator << (const TElement & element)
{
    std::cout<<Core::toString(element);
    return *this;
}
//-----------------------------------------------------------------------------


} // namespace Core

#endif /* STANDARDSTREAM_H_ */
