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
#ifndef CORE_FILESTREAM_H_
#define CORE_FILESTREAM_H_

#include <stdio.h>
#include <Core/NonCopyable.h>
#include <Core/IO/Streams.h>

namespace Core
{
/**
 * Cette classe permet de lire le contenu de fichiers.
 * 
 * Les fichiers sont toujours lus comme des fichiers binaires, on ne fait
 * pas de distinction comme sous Windows. Si l'on veut explicitement gérer des
 * fichiers textes il faut passer par un Core::TextInputStream.
 * 
 * \ingroup IO
 */
class LM_API_COR FileInputStream : public Object, public SeekableInputStream, public NonCopyable
{
public:
    /**
     * Construit la stream sans qu'elle ne soit attachée à un fichier. Il faudra
     * appeler la méthode open() pour la lier à un fichier.
     */
    FileInputStream();
    
    /**
     * Construit la stream en l'attachant à un fichier. L'ouverture se
     * fait comme pour la méthode open().
     */
    FileInputStream(const String & fileName);
    virtual ~FileInputStream();
    
    /**
     * Ouvre un fichier.
     * Après l'appel à cette méthode, la stream est liée au fichier et toutes
     * les lectures se font dans ce fichier.
     * Si le fichier n'a pas pu être ouvert, <b>aucune exception n'est levée</b>,
     * il faut utiliser la méthode isOpened() pour vérifier que le fichier est bien 
     * ouvert. En revanche, lire dans un fichier qui n'est pas ouvert lève une 
     * Core::IOException.
     * 
     * Ce mécanisme se justifie par le fait que la non ouverture d'un fichier
     * est un évènement suffisamment courant pour devoir être géré par 
     * l'algorithme de l'appelant plutôt que par un traitement d'exception,
     * libre à l'appelant de lever une exception si l'ouverture du fichier
     * est si cruciale pour le bon déroulement des opérations.
     */
    virtual void open(const String & fileName);
    
    /**
     * Cette méthode retourne true si le fichier est ouvert. C'est une erreur
     * de lire dans une stream dont le fichier n'est pas ouvert.
     */
    virtual bool isOpened() const;
    
    virtual void readData(void * buffer, int32 size);
    virtual int32 readUpTo(void * buffer, int32 size);
    virtual bool eof() const;
    virtual void close();
    
    virtual int64 getPosition() const;
    virtual int64 getLength() const;
    virtual void setPosition(int64 offset, ESeekOrigin origin);
    virtual void rewind();
    
private:
    /**
     * Le descripteur de fichier C.
     */
    FILE * _file;
};

/**
 * Cette classe permet d'écrire dans des fichiers.
 * 
 * Les fichiers sont toujours lus comme des fichiers binaires, on ne fait
 * pas de distinction comme sous Windows. Si l'on veut explicitement gérer des
 * fichiers textes il faut passer par un Core::TextOutputStream.
 * 
 * \ingroup IO
 */
class LM_API_COR FileOutputStream : public Object, public OutputStream
{
public:
    
    enum EFileAccess {FILE_TRUNCATE, FILE_APPEND};
    
    /**
     * Construit la stream sans qu'elle ne soit attachée à un fichier. Il faudra
     * appeler la méthode open() pour la lier à un fichier.
     */
    FileOutputStream();

    /**
     * Construit la stream en l'attachant à un fichier. L'ouverture se
     * fait comme pour la méthode open().
     */
    FileOutputStream(const String & fileName, EFileAccess fileAccess=FILE_TRUNCATE);
    ~FileOutputStream();
    
    /**
     * Ouvre un fichier.
     * Après l'appel à cette méthode, la stream est liée au fichier et toutes
     * les lectures se font dans ce fichier.
     * Si le fichier n'a pas pu être ouvert, <b>aucune exception n'est levée</b>,
     * il faut utiliser la méthode isOpened() pour vérifier que le fichier est bien 
     * ouvert. En revanche, lire dans un fichier qui n'est pas ouvert lève une 
     * Core::IOException.
     * 
     * Ce mécanisme se justifie par le fait que la non ouverture d'un fichier
     * est un évènement suffisamment courant pour devoir être géré par 
     * l'algorithme de l'appelant plutôt que par un traitement d'exception,
     * libre à l'appelant de lever une exception si l'ouverture du fichier
     * est si cruciale pour le bon déroulement des opérations.
     * 
     * Si \a fileAccess vaut FILE_TRUNCATE et que le fichier existait déjà,
     * son contenu est écrasé. Si \a fileAccess vaut FILE_APPEND alors tout
     * ce qui est envoyé dans la stream est ajouté à la fin du fichier. Dans
     * les deux cas le fichier est créé s'il n'existait pas.
     */
    virtual void open(const String & fileName, EFileAccess fileAccess=FILE_TRUNCATE);
    virtual bool isOpened() const;
    virtual void flush();
    
    virtual void writeData(const void * buffer, int32 size);
    virtual bool eof() const;
    virtual void close();
private:
    FILE * _file;
};
}

#endif /*CORE_FILESTREAM_H_*/
