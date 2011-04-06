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
#ifndef CORE_BUFFERSTREAM_H_
#define CORE_BUFFERSTREAM_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/NonCopyable.h>
#include <Core/DoubleQueue.h>
#include <Core/IO/Streams.h>

namespace Core
{
/**
 * Cette classe permet de gérer un tampon mémoire. Le tampon peut être
 * lu et écrit comme n'importe quel stream. Initialement le tampon ne
 * contient rien. Le tampon grandit quand on y écrit des données. La lecture
 * consomme les données écrites.
 * \ingroup IO
 */
class LM_API_COR BufferStream : public Object, public InputOutputStream
{
public:
    /**
     * Construit un tampon vide.
     */
    BufferStream();

    /**
     * Lecture bloquante de données. Tant que le buffer ne contient pas
     * assez de données, cette méthode attend qu'on y écrive quelque chose.
     */
    virtual void readData(void * buffer, int32 size);

    /**
     * Lecture non bloquante de données. Que le buffer contienne ou pas assez
     * de données, cette méthode n'est pas bloquante. Les données disponibles
     * sont retournées immédiatement.
     */
    virtual int32 readUpTo(void * buffer, int32 size);
    virtual void writeData(const void * buffer, int32 size);

    // Attention, les données des DoubleQueue ne sont pas forcément contiguës, il ne faut
    // donc pas retourner un pointeur brut vers les données
    virtual void getDataCopy(List<byte> & data) const;
    virtual int32 getDataSize() const;

    virtual bool eof() const;
    virtual void close();
    virtual void flush();

protected:
    void internalRead(void * buffer, int32 size);

    bool    _isOpened;
    DoubleQueue<byte>   _data;
};
}

#endif /*CORE_BUFFERSTREAM_H_*/
