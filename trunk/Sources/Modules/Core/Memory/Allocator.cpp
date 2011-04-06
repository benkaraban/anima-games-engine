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

#ifdef _WIN32
#include "winsafe.h"
#else
#include <sys/types.h>
#include <pthread.h>
#endif

#include <malloc.h>
#include <Core/Debug/Assert.h>
#include "Allocator.h"

///
#define STANDARD_ALLOCATOR      0
#define LOG_USAGE               0
#define LOG_USAGE_THRESHOLD     (1 * 1024)
///

#if LOG_USAGE
#include <Core/Debug/CallStack.h>
#include <Core/Logger.h>
#endif

namespace Core
{
#if LOG_USAGE

struct LogNode
{
    static const int32 BUFFER_SIZE = 4096;

    LogNode *   pNext;

    void *      pData;
    int32       size;
    char        stack[BUFFER_SIZE + 1];
};

static bool         spDumpInProgress    = false;
static LogNode *    spNode              = NULL;

void logAlloc(void * pData, int32 size)
{
    if(size >= LOG_USAGE_THRESHOLD && !spDumpInProgress)
    {
        LogNode * pNode = (LogNode*)malloc(sizeof(LogNode));
        pNode->pData = pData;
        pNode->size = size;
        Core::getCallStackLowLevel(pNode->stack, LogNode::BUFFER_SIZE);

        LogNode * pPrev = NULL;
        LogNode * pNext = spNode;

        while(pNext != NULL && pNext->size < size)
        {
            pPrev = pNext;
            pNext = pNext->pNext;
        }

        if(pPrev != NULL)
            pPrev->pNext = pNode;
        else
            spNode = pNode;

        pNode->pNext = pNext;
    }
}

void logFree(void * pData, int32 size)
{
    if(size >= LOG_USAGE_THRESHOLD && !spDumpInProgress)
    {
        LogNode * pPrev = NULL;
        LogNode * pNode = spNode;

        while(pNode != NULL && pNode->pData != pData)
        {
            pPrev = pNode;
            pNode = pNode->pNext;
        }

        if(pNode != NULL)
        {
            if(pPrev != NULL)
                pPrev->pNext = pNode->pNext;
            else
                spNode = pNode->pNext;

            free(pNode);
        }
    }
}

#endif

static const int32 MAGIC_FREE = 0xDEAD0000;
static const int32 MAGIC_USED = 0xF00D0000;

//-----------------------------------------------------------------------------
Allocator & Allocator::getInstance()
{
    static Allocator sInstance; 
    return sInstance;
};
#ifdef _WIN32
//-----------------------------------------------------------------------------
void Allocator::lock()
{
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(_internal));
}
//-----------------------------------------------------------------------------
void Allocator::unlock()
{
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(_internal));
}
//-----------------------------------------------------------------------------
void Allocator::initLock()
{
    LM_STATIC_ASSERT(sizeof(CRITICAL_SECTION) <= sizeof(Allocator::_internal));
    InitializeCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(_internal));
}
//-----------------------------------------------------------------------------
void Allocator::destroyLock()
{
    DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(_internal));
}
#else
//-----------------------------------------------------------------------------
void Allocator::lock()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t *>(_internal));
}
//-----------------------------------------------------------------------------
void Allocator::unlock()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t *>(_internal));
}
//-----------------------------------------------------------------------------
void Allocator::initLock()
{
    LM_STATIC_ASSERT(sizeof(pthread_mutex_t) <= sizeof(Allocator::_internal));
    int32 returnCode = 0;
    returnCode = pthread_mutex_init(reinterpret_cast<pthread_mutex_t *>(_internal) , NULL);
}
//-----------------------------------------------------------------------------
void Allocator::destroyLock()
{
    int32 returnCode = 0;
    returnCode = pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t *>(_internal));
}
#endif
//-----------------------------------------------------------------------------
Allocator::Allocator()
{
    LM_STATIC_ASSERT(sizeof(BlocHeader) == Allocator::MEMORY_ALIGNMENT);

    initLock();
    for(int32 ii=0; ii <= MAXIMUM_POOL_ALLOC_SIZE; ii++)
    {
        _pFirstBloc[ii] = NULL;
    }
}
//-----------------------------------------------------------------------------
Allocator::~Allocator()
{
    destroyLock();
}
//-----------------------------------------------------------------------------
void Allocator::createSuperBloc(int32 blocSize)
{
    int32 superBlocSize = POOL_ALLOC_SUPERBLOC_SIZE - POOL_ALLOC_SUPERBLOC_SIZE % blocSize;
    int32 nbBlocs = (superBlocSize - MEMORY_ALIGNMENT) / blocSize;
    byte * pSuperBlocStart = (byte*)malloc(superBlocSize);
    int32 alignement = int32(int64(pSuperBlocStart)) % MEMORY_ALIGNMENT;
    if(alignement != 0)
    {
        pSuperBlocStart += MEMORY_ALIGNMENT - alignement;
    }
    int32 iBloc = 0;

    while(iBloc < nbBlocs - 1)
    {
        BlocHeader * pBlocHeader = reinterpret_cast<BlocHeader*>(&pSuperBlocStart [iBloc * blocSize]);
        pBlocHeader->_blocSize = blocSize;
        pBlocHeader->_nextFreeBloc = reinterpret_cast<BlocHeader*>(&pSuperBlocStart [(iBloc + 1) * blocSize]);
#ifdef _DEBUG
        pBlocHeader->_sentinel = MAGIC_FREE;
#endif

        iBloc++;
    }

    BlocHeader * pBlocHeader = reinterpret_cast<BlocHeader*>(&pSuperBlocStart [iBloc * blocSize]);
    pBlocHeader->_blocSize = blocSize;
    pBlocHeader->_nextFreeBloc = _pFirstBloc[blocSize];
#ifdef _DEBUG
    pBlocHeader->_sentinel = MAGIC_FREE;
#endif
    _pFirstBloc[blocSize] = reinterpret_cast<BlocHeader*>(pSuperBlocStart);
}
//-----------------------------------------------------------------------------
void * Allocator::allocate(int32 size)
{
#if STANDARD_ALLOCATOR
    return _mm_malloc(size, MEMORY_ALIGNMENT);
    //return ::malloc(size);
#else
    lock();
    size += int32(sizeof(BlocHeader));

    // La taille des blocs est ajustée pour être un multiple entier de
    // l'alignement. De cette manière il suffit que le premier bloc
    // d'une liste contigüe de blocs soit aligné pour que les suivants
    // le soient également.
    int32 paddedSize = size;
    int32 padding = size % MEMORY_ALIGNMENT;
    if (padding != 0)
    {
        paddedSize += MEMORY_ALIGNMENT - padding;
    }

    void * result = NULL;

    if(paddedSize >= 0 && paddedSize <= MAXIMUM_POOL_ALLOC_SIZE)
    {
        if(_pFirstBloc[paddedSize] == NULL)
        {
            createSuperBloc(paddedSize);
        }
        BlocHeader * pBlocHeader = reinterpret_cast<BlocHeader*>(_pFirstBloc[paddedSize]);
        _pFirstBloc[paddedSize] = pBlocHeader->_nextFreeBloc;
#ifdef _DEBUG
        LM_ASSERT(pBlocHeader->_sentinel == MAGIC_FREE);
        pBlocHeader->_sentinel = MAGIC_USED;
#endif
        // Pointe sur ce qu'il y a juste après le header (ie. les données effectives du bloc)
        result = &pBlocHeader [1];
    }
    else
    {
        size += MEMORY_ALIGNMENT - 1;
        byte * allocatedMem = reinterpret_cast<byte*>(malloc(size));
        BlocHeader * pBlocHeader = NULL;

        int32 alignement = int32(int64(allocatedMem)) % MEMORY_ALIGNMENT;
        if(alignement != 0)
        {
            pBlocHeader = reinterpret_cast<BlocHeader*>(allocatedMem + (MEMORY_ALIGNMENT - alignement));
        }
        else
        {
            pBlocHeader = reinterpret_cast<BlocHeader*>(allocatedMem);
        }

        pBlocHeader->_effectiveBlocStart = allocatedMem;
        pBlocHeader->_blocSize = paddedSize;
#ifdef _DEBUG
        pBlocHeader->_sentinel = MAGIC_USED;
#endif
        result = &pBlocHeader [1];
    }

#if LOG_USAGE
    logAlloc(result, paddedSize);
#endif
    unlock();

    return result;
#endif
}
//-----------------------------------------------------------------------------
void Allocator::free(void * pointer)
{
#if STANDARD_ALLOCATOR
    return _mm_free(pointer);
    //return ::free(pointer);
#else
    lock();

    if(pointer != 0)
    {
        byte * bytePointer = reinterpret_cast<byte*>(pointer);
        BlocHeader * pBlocHeader = reinterpret_cast<BlocHeader*>(&bytePointer[-MEMORY_ALIGNMENT]);
#ifdef _DEBUG
        LM_ASSERT(pBlocHeader->_sentinel == MAGIC_USED);
        pBlocHeader->_sentinel = MAGIC_FREE;
#endif
#if LOG_USAGE
    logFree(pointer, pBlocHeader->_blocSize);
#endif
        if(pBlocHeader->_blocSize >= 0 && pBlocHeader->_blocSize <= MAXIMUM_POOL_ALLOC_SIZE)
        {
            pBlocHeader->_nextFreeBloc = _pFirstBloc[pBlocHeader->_blocSize];
            _pFirstBloc[pBlocHeader->_blocSize] = pBlocHeader;
        }
        else
        {
            ::free(pBlocHeader->_effectiveBlocStart);
        }
    }
    unlock();
#endif
}
//-----------------------------------------------------------------------------
void Allocator::dumpMemoryUsage()
{
#if LOG_USAGE
    lock();
    spDumpInProgress = true;
    unlock();

    INF << L"*******************************************\n";
    INF << L"* Memory dump          - BEGIN -          *\n";
    INF << L"*******************************************\n";

    LogNode * pNode = spNode;

    while(pNode != NULL)
    {
        INF << L"===========================================\n";
        INF << L" Block of size " << Core::toString(pNode->size, 4) << L" at address 0x" << Core::toStringHex(int32(pNode->pData), 8) << L"\n";
        INF << L"===========================================\n";
        INF << String(String8(pNode->stack));

        pNode = pNode->pNext;
    }

    INF << L"*******************************************\n";
    INF << L"* Memory dump          -  END  -          *\n";
    INF << L"*******************************************\n";

    lock();
    spDumpInProgress = false;
    unlock();
#endif
}
//-----------------------------------------------------------------------------
}
