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
#ifndef CORE_ALLOCATOR_H_
#define CORE_ALLOCATOR_H_

#include <Core/Types.h>

namespace Core
{
/**
 * \defgroup Memory Package Memory
 * Ce package regroupe les classes qui permettent de gérer les allocations mémoire.
 * 
 * \ingroup Core
 */

/**
 * Allocateur de mémoire. Cet allocateur est optimisé pour être plus rapide qu'un
 * malloc standard, quitte à être moins économe en mémoire.
 * 
 * \ingroup Memory
 */
class LM_API_COR Allocator
{
public:  
    /**
     * Cette constante définit l'alignement de blocs alloués en mémoire. L'adresse
     * d'un bloc alloué est toujours un multiple de MEMORY_ALIGNMENT.
     * <br><b>Attention :</b> MEMORY_ALIGNMENT doit être au moins égal à 8 car
     * l'allocateur insère des données particulières en entête des blocs qu'il 
     * alloue, et ces données font 8 octets.   
     */
    static const int32 MEMORY_ALIGNMENT = 16;
    
    /**
     * Alloue un bloc de size octets. L'adresse du bloc est un multiple de MEMORY_ALIGNMENT.
     */
    void * allocate(int32 size);
    
    /**
     * Libère un bloc mémoire. La libération du bloc ne se traduit pas forcément en une 
     * libération de mémoire au niveau de l'OS. L'allocateur recycle les blocs libérés.
     */
    void free(void * pointer);
    
    void dumpMemoryUsage();
    
    /**
     * Retourne l'unique instance de l'allocateur.
     */
    static Allocator & getInstance();
    
private:
    /**
     * La taille maximale des allocations par pool. Lorsqu'un bloc de taille
     * supérieure doit être alloué, l'allocateur utilise malloc pour allouer
     * le bloc, en ajustant la taille pour garantir son alignement.
     */
    static const int32 MAXIMUM_POOL_ALLOC_SIZE = 1024;
    
    /**
     * Taille d'un super block de mémoire. Pour limiter le nombre d'appel
     * à malloc, losque l'allocateur doit allouer un nouveau bloc et qu'il
     * na plus de bloc adapté dans son pool, il alloue un super bloc de cette
     * taille, super bloc qui est ensuite découpé en blocs de la taille désirée.
     */
    static const int32 POOL_ALLOC_SUPERBLOC_SIZE = 64<<10; // 64 Ko

    Allocator();
    ~Allocator();
    
    void createSuperBloc(int32 blocSize);

    void initLock();
    void destroyLock();    
    void lock();
    void unlock();
    
    struct BlocHeader
    {
        union
        {
            struct
            {
                /**
                 * La taille du bloc, en octets.
                 */
                int32        _blocSize;
                int32        _sentinel;
                union
                {
                    /**
                     * L'adresse de début de bloc.
                     * En raison de l'alignement, l'adresse de l'entête ne correspond
                     * pas forcément à celle de début du bloc. Si le bloc est alloué
                     * dans un pool ce n'est pas génant mais dans le cas d'un bloc 
                     * alloué avec malloc il nous faut l'adresse de début de bloc
                     * pour pouvoir le libérer avec free. Cet attribut permet de stocker
                     * cette adresse. Il n'est valide que lorsqu'il s'agit d'un bloc
                     * alloué avec malloc.
                     */
                    void *       _effectiveBlocStart;
                    
                    /**
                     * L'adresse du header prochain bloc libre.
                     * Les blocs libres sont contenus dans une liste chaînée.
                     * Cet attribut n'est utilisée que lorsque le bloc est libre. Dans
                     * le cas contraire c'est l'attribut _effectiveBlocStart qui sera 
                     * utilisé.
                     */
                    BlocHeader * _nextFreeBloc;
                };
            };
            
            /**
             * Cet attribut permet de s'assurer de l'alignement des données
             * effectives du bloc. On considére que l'entête du bloc est
             * elle même alignée. Ormis l'alignement, cet attribut est inutilisé. 
             */
            byte    _alignement[MEMORY_ALIGNMENT];
        };
    };
    
    BlocHeader * _pFirstBloc[MAXIMUM_POOL_ALLOC_SIZE+1];
    
    /**
     * Voir Core::Mutex pour plus d'explications sur ce hack.
     */
    byte    _internal[40];
};
//-----------------------------------------------------------------------------
}

#endif /*ALLOCATOR_H_*/
