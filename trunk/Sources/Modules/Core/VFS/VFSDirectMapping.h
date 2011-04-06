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
#ifndef CORE_VFS_DIRECT_MAPPING_H_
#define CORE_VFS_DIRECT_MAPPING_H_

#include <Core/VFS/VirtualFileSystem.h>

namespace Core
{
/**
 * Cette classe hérite de la classe VirtualFileSystem et représente un arborescence
 * de fichiers stockée sur un disque physique.
 * Une fois lié à un dossier physique du disque dur, ce dossier est considéré comme
 * la racine de ce VFS.
 * Par exemple : si on lie le VFSDirectMapping à "C:/Loaded" toute les requêtes
 * seront préfixés par ce répertoire.
 * @code
 * vfsDirectMapping.listDirs("/Dev/Sources", content);
 * @endcode
 * Ce code aura donc pour effet de remplir la liste "content" avec le nom des répertoires
 * contenu dans le dossier "C:/Loaded/Dev/Sources"
 */
class LM_API_COR VFSDirectMapping : public VirtualFileSystem
{
public:
    VFSDirectMapping();
    virtual ~VFSDirectMapping();

    virtual EVFSType getType() const {return VFS_DIRECT_MAPPING;}

    /**
     * Cette méthode permet de définir quel sera l'arborescence de fichiers sur laquelle
     * ce VFS sera lié.
     * @param[in] : le nom du répertoire sur le disque dur
     * @return : true si le dossier existe bien
     */
    virtual bool linkDirectory(const String & name);

    /**
     * Idem linkDirectory sauf que la méthode lève une exception si le répertoire n'existe pas.
     */
    virtual void linkDirectoryThrow(const String & name);

    virtual void listDirs(const String & path, List<String> & content);
    virtual void listFiles(const String & path, List<String> & content);
    virtual bool fileExists(const String & fileName);
    virtual bool dirExists(const String & dirName);
    virtual void createDir(const String & dirName);
    virtual bool moveFile(const String & from, const String & to);
    virtual bool moveDir(const String & from, const String & to);
    virtual TimeValue getLastModificationDate(const String & fileName);

    virtual String toString() const;

    virtual Ptr<InputStream> readFile(const String & fileName);
    virtual Ptr<OutputStream> writeFile(const String & fileName,
                                        FileOutputStream::EFileAccess fileAccess=FileOutputStream::FILE_TRUNCATE);

    /**
     * Permet de connaitre quel est le répertoire sur lequel pointe ce VFS.
     * Cette fonction est notamment utilisée pour la traduction de VFS en XML.
     */
    virtual const String & getMappedDirectory() const { return _directoryMapping ;}

private:
    virtual String createFullPath(const String & path);

private:
    String  _directoryMapping;

};


}//namespace Core

#endif /*CORE_VFS_DIRECT_MAPPING_H_*/
