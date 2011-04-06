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
#ifndef CORE_VFS_MOUNT_POINT_H_
#define CORE_VFS_MOUNT_POINT_H_

#include <Core/VFS/VirtualFileSystem.h>
#include <Core/Map.h>
namespace Core
{
/**
 * Cette classe représente un point de montage du VFS.
 * Elle permet d'organiser les VFS en une hierarchie.
 * Pour cela elle associe à chaque VFS "monté" une clé permettant
 * d'accéder à ces derniers rapidement.
 *
 * Si l'on considère un VFSDirectMapping lié au répertoire "C:/Loaded"
 * @code
 * vfsMountPoint.mount("/home", vfsDirectMapping);
 * vfsMountPoint.listDir("/home/Dev/Sources", content);
 * @endcode
 * Le code précedent aura pour effet de récupérer la liste des répertoires
 * contenu dans le dossier "C:/Loaded/Dev/Sources"
 */
class LM_API_COR VFSMountPoint : public VirtualFileSystem
{
public:
    VFSMountPoint();
    virtual ~VFSMountPoint();

    virtual EVFSType getType() const {return VFS_MOUNT_POINT;}

    /**
     * Cette méthode permet de monté un VFS.
     * Le code suivant :
     * @code
     * vfsMountPoint.mount("/data", vfsDirectMapping);
     * @endcode
     * Associera le VFS vfsDirectMapping à la clé "/data"
     * @param key[in] : la clé à laquelle on souhaite associé notre vfs
     * @param pVFS[in] : le VFS que l'on souhaite monter
     * @return true si le VFS a bien été monté
     */
    virtual bool mount(const String & key, const Ptr<VirtualFileSystem> & pVFS);

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
     * Cette méthode permet de récupérer les clés de tous les VFS montés.
     * Elle sert notamment pour la traduction du VFS en XML.
     */
    virtual void getMountedPointKeys(List<String> & mountedPointKeys);
    /**
     * Cette méthode permet de récupéré un VFS par sa clé.
     * Elle sert notamment pour la traduction du VFS en XML.
     */
    virtual Ptr<VirtualFileSystem> getMountedPoint(const String & vfsKey);

private:
    virtual bool isPathValid(const String & path);
    virtual bool isVFSMounted(const String & path);
    /**
     * Méthode permettant d'extraire la clé du vfs à partir du chemin complet
     * Si l'on considère le code suivant :
     * @code
     * path = "/Gfx/Textures"
     * key = extractVFSKey(path)
     * @endcode
     * key = "/Gfx"
     * path = "/Textures"
     * @param[in,out] path : le chemin complet. Attention il sera tronqué de la clé
     * @return la clé du VFS recherché
     */
    virtual String extractVFSKey(String & path);
    virtual void internalGetMountedPointKeys(List<String> & mountedPointKeys, bool includeRoot);

private:
    Map<String, Ptr<VirtualFileSystem> > _mountedVFS;
    Ptr<VirtualFileSystem>               _pRootVFS;
};

}//namespace Core

#endif/*CORE_VFS_MOUNT_POINT_H_*/
