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
#ifndef CORE_VIRTUAL_FILE_SYSTEM_H_
#define CORE_VIRTUAL_FILE_SYSTEM_H_

#include <Core/Object.h>
#include <Core/String.h>
#include <Core/List.h>
#include <Core/IO/Streams.h>
#include <Core/IO/FileStream.h>
#include <Core/System/Files.h>

namespace Core
{

LM_ENUM_3(EVFSType, VFS_MOUNT_POINT, VFS_DIRECT_MAPPING, VFS_ARCHIVE_MAPPING);

/**
 * Cette classe abstraite permet de définir le comportement général de chaque
 * élément constituant un Virtual File System.
 */
class LM_API_COR VirtualFileSystem : public Object
{
public:
    virtual ~VirtualFileSystem(){};
    /**
     * Cette méthode permet de connaitre le type du VFS.
     */
    virtual EVFSType getType() const = 0;
    /**
     * Cette methode permet de récupérer un InputStream sur les données d'un fichier dont
     * le 'nom complet' est passé en paramètre.
     * @param fileName[in] : le nom complet du fichier que l'on souhaite récupérer
     * @return : les données contenues dans le fichier demandé, NULL s'il n'existe pas
     */
    virtual Ptr<InputStream> readFile(const String & fileName) = 0;
    /**
     * Cette methode permet de récupérer un OutputStream sur un fichier dont
     * le 'nom complet' est passé en paramètre. Il faut en outre préciser si l'on souhaite
     * tronquer le fichier ou concaténer des données.
     * @param name[in] : le nom complet du fichier que l'on souhaite récupérer
     * @param fileAccess : indique si l'on souhaite tronquer le fichier ou concaténer les données à la suite.
     * @return : les données contenues dans le fichier demandé, NULL s'il n'existe pas
     */
    virtual Ptr<OutputStream> writeFile(const String & fileName,
                                        FileOutputStream::EFileAccess fileAccess=FileOutputStream::FILE_TRUNCATE) = 0;
    /**
     * Cette méthode permet de récupérer la liste des répertoires contenus dans un chemin donné
     * @param path[in] : chemin dans lequel on souhaite récupérer la liste des répertoires
     * @param content[out] : cette liste est remplie avec les noms des répertoires.
     */
    virtual void listDirs(const String & path, List<String> & content) = 0;
    /**
     * Cette méthode permet de récupérer la liste des fichiers contenus dans un chemin donné
     * @param path[in] : chemin dans lequel on souhaite récupérer la liste des fichiers
     * @param content[out] : cette liste est remplie avec les noms des fichiers.
     */
    virtual void listFiles(const String & path, List<String> & content) = 0;
    /**
     * Cette méthode permet de savoir si un fichier existe ou non
     * @param fileName[in] : nom complet du fichier dont on souhaite connaitre l'existence
     * @return : true si le fichier existe, false sinon.
     */
    virtual bool fileExists(const String & fileName) = 0;
    /**
     * Cette méthode permet de savoir si un répertoire existe ou non
     * @param fileName[in] : nom complet du répertoire dont on souhaite connaitre l'existence
     * @return : true si le répertoire existe, false sinon.
     */
    virtual bool dirExists(const String & dirName) = 0;
    /**
     * Cette méthode permet de créer une répertoire
     * @param fileName[in] : nom complet du répertoire que l'on souhaite créer
     */
    virtual void createDir(const String & dirName) = 0;
    /**
     * Cette méthode permet de renommer un fichier
     * @param from[in] : nom complet du fichier que l'on souhaite renommer
     * @param to[in] : nouveau nom du fichier
     * @return : true si le renommage s'est bien passé, false sinon
     */
    virtual bool moveFile(const String & from, const String & to) = 0;
    /**
     * Cette méthode permet de renommer un répertoire
     * @param from[in] : nom complet du répertoire que l'on souhaite renommer
     * @param to[in] : nouveau nom du répertoire
     * @return : true si le renommage s'est bien passé, false sinon
     */
    virtual bool moveDir(const String & from, const String & to) = 0;
    /**
     * Cette méthode permet de récupérer la date de dernière modification d'un fichier
     * @param fileName[in] : nom complet du fichier dont on souhaite connaitre la date de dernière modification
     * @return : le TimeValue correspondant à la date en question
     */
    virtual TimeValue getLastModificationDate(const String & fileName) = 0;

    virtual void read(const String & fileName, Serializable & serializable);
    virtual void write(const String & fileName, const Serializable & serializable);
};

}//namespace Core

#endif/*CORE_VIRTUAL_FILE_SYSTEM_H_*/
