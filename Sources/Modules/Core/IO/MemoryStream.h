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
#ifndef CORE_MEMORYSTREAM_H_
#define CORE_MEMORYSTREAM_H_

#include <Core/NonCopyable.h>
#include <Core/IO/Streams.h>

namespace Core
{
/**
 * Cette classe permet de traiter le contenu d'une zone mémoire comme s'il s'agissait
 * d'une stream.
 * 
 * La zone n'est accessible qu'en lecture est définie à la création de la stream,
 * sans pouvoir être modifiée. L'idée est d'utiliser cette stream pour interfacer
 * une zone mémoire déjà existante. Si vous voulez gérer une stream comme un tampon
 * dans lequel on peut lire et écrire il faut plutôt créer un Core::BufferStream.
 * \ingroup IO
 */
class LM_API_COR MemoryInputStream : public Object, public SeekableInputStream, public NonCopyable
{
public:
    /**
     * Crée une stream qui référence les données à l'adresse \a pData
     * de la taille de \a size octets. Si \a ownData est à true, la mémoire
     * est libérée avec l'opérateur delete []. Dans le cas contraire c'est à
     * l'appelant de gérer la mémoire.
     */
    MemoryInputStream(const void * pData, int32 size, bool ownData=false);
    ~MemoryInputStream();
    
    virtual void readData(void * buffer, int32 size);
    virtual int32 readUpTo(void * buffer, int32 size);
    virtual bool eof() const;
    virtual void close();
    
    virtual int64 getPosition() const;
    virtual int64 getLength() const;
    virtual void setPosition(int64 offset, ESeekOrigin origin);
    virtual void rewind();
protected:
    const byte * _pDataStart;
    const byte * _pDataEnd;
    const byte * _pCurData;
    bool         _ownData;
};
}

#endif /*CORE_MEMORYSTREAM_H_*/
