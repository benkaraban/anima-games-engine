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

#include <stdio.h>
#include <vector>
#include <algorithm>

#include <Core/Memory/Allocator.h>
#include <Core/Standard.h>
#include <UTests.h>


void fillData(byte * pData, int32 size)
{
    byte value = size & 0xFF;    
    for(int ii=0; ii < size; ii++)
    {
        pData[ii] = value;
    }
}

void checkData(byte * pData, int32 size)
{
    byte expectedValue = size & 0xFF;
    bool isOK = true;
    int32 value = 0;
    int32 ii = 0;
    
    while(ii < size && isOK)
    {
        value = pData[ii];
        isOK = (value == expectedValue);
        UTEST_ASSERT(isOK, "Values in allocated memory were corrupted!");
        ii++;
    }
    
    if(!isOK)
    {
        std::cerr << "Check error, size : " << size << ", value " 
                  << value << ", at position " << ii - 1 << std::endl;
    }
}

struct AllocData
{
    int32   _size;
    byte *  _pData;    
};

static void * ptr = NULL;
static const int32 SIZE = 1000;
static const int32 MAX_ALLOC_SIZE = 1200;
static const int32 BLOC_COUNT = MAX_ALLOC_SIZE;
static byte * pBlocs1[BLOC_COUNT];

static void testAllocatorSimple()
{
    Core::Allocator & allocator = Core::Allocator::getInstance();
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Simple allocation");   
    ptr = allocator.allocate(SIZE);   
    UTEST_END_TEST_NO_EXCEPTION;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Simple deallocation");   
    allocator.free(ptr);   
    UTEST_END_TEST_NO_EXCEPTION;
}

static void testAllocatorMultiple()
{
    Core::Allocator & allocator = Core::Allocator::getInstance();

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Multiple allocation (1)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        pBlocs1[ii] = reinterpret_cast<byte*>(allocator.allocate(ii));
    }
    UTEST_END_TEST_NO_EXCEPTION;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Memory write (1)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        fillData(pBlocs1[ii], ii);
    }
    UTEST_END_TEST_NO_EXCEPTION;

    byte * pBlocs2[BLOC_COUNT];
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Checking data (1)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        checkData(pBlocs1[ii], ii);
    }
    UTEST_END_TEST_NO_EXCEPTION;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Multiple allocation (2)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        pBlocs2[ii] = reinterpret_cast<byte*>(allocator.allocate(ii));
    }
    UTEST_END_TEST_NO_EXCEPTION;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Checking data (1)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        checkData(pBlocs1[ii], ii);
    }
    UTEST_END_TEST_NO_EXCEPTION;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Memory write (2)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        fillData(pBlocs2[ii], ii);
    }
    UTEST_END_TEST_NO_EXCEPTION;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Checking data (2)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        checkData(pBlocs2[ii], ii);
    }
    UTEST_END_TEST_NO_EXCEPTION;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Multiple deallocation (1)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        allocator.free(pBlocs1[ii]);
    }
    UTEST_END_TEST_NO_EXCEPTION;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Checking data (2)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        checkData(pBlocs2[ii], ii);
    }
    UTEST_END_TEST_NO_EXCEPTION;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Multiple deallocation (2)");
    for(int32 ii=0; ii < BLOC_COUNT; ii++)
    {
        allocator.free(pBlocs2[ii]);
    }
    UTEST_END_TEST_NO_EXCEPTION;
}

static void testAllocatorRandom()
{
    Core::Allocator & allocator = Core::Allocator::getInstance();
    
    srand(0);
    
    const int32 ALLOCATION_COUNT = 2000;
    const int32 ITERATION_COUNT  = 50;
    const int32 TURNOVER_COUNT   = 1000;
        
    std::vector<AllocData> allocData;
    
    allocData.reserve(ALLOCATION_COUNT);
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Random allocation/deallocation");
    
    for(int32 ii=0; ii < ALLOCATION_COUNT; ii++)
    {
        AllocData ad;
        ad._size = rand() % MAX_ALLOC_SIZE + 1;
        ad._pData = reinterpret_cast<byte*>(allocator.allocate(ad._size));
        fillData(ad._pData, ad._size);
        allocData.push_back(ad);
    }
    
    for(int32 ii=0; ii < ITERATION_COUNT; ii++)
    {
        std::random_shuffle(allocData.begin(), allocData.end());
        for(int32 iTurnOver=0; iTurnOver < TURNOVER_COUNT; iTurnOver++)
        {
            checkData(allocData[iTurnOver]._pData, allocData[iTurnOver]._size);
            allocator.free(allocData[iTurnOver]._pData);
            allocData[iTurnOver]._size = rand() % MAX_ALLOC_SIZE + 1;
            allocData[iTurnOver]._pData = reinterpret_cast<byte*>(allocator.allocate(allocData[iTurnOver]._size));
            fillData(allocData[iTurnOver]._pData, allocData[iTurnOver]._size);
        }
    }
    
    for(int32 ii=0; ii < ALLOCATION_COUNT; ii++)
    {
        checkData(allocData[ii]._pData, allocData[ii]._size);
        allocator.free(allocData[ii]._pData);
    }
    
    UTEST_END_TEST_NO_EXCEPTION;
}

static void testAllocatorAlignment()
{
    Core::Allocator & allocator = Core::Allocator::getInstance();

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Address alignment");
    
    std::vector<void *> alignData;
    
    for(int32 ii=0; ii < 10; ii++)
    {
        for (int32 size=0; size < 1200; size++)
        {
            void * pData = allocator.allocate(size);
            UTEST_EQU((int32(int64(pData)) % Core::Allocator::MEMORY_ALIGNMENT), 0);
            alignData.push_back(pData);
        }
    }
       
    for(int32 ii=0; ii < int32(alignData.size()); ii++)
    {
        allocator.free(alignData[ii]);
    }    
    
    UTEST_END_TEST_NO_EXCEPTION;
    
    return;

    //-------------------------------------------------------------------------
    /*
    const int32 SPEED_ITERATION_COUNT = 200;
    
    std::vector<int32> sizes;
    
    for(int32 ii=0; ii < ALLOCATION_COUNT; ii++)
    {
        sizes.push_back(rand() % MAX_ALLOC_SIZE - 200);
    }
    
    std::vector<void*> pBlocs;
    
    DWORD startTick = GetTickCount();
    pBlocs.resize(sizes.size());
    
    for(int iteration=0; iteration < SPEED_ITERATION_COUNT; iteration++)
    {
        for(uint32 ii=0; ii < pBlocs.size(); ii++)
        {
            pBlocs[ii] = allocator.allocate(sizes[ii]);
        }
        for(uint32 ii=0; ii < pBlocs.size(); ii++)
        {
            allocator.free(pBlocs[ii]);
        }
    }        
    
    std::cout << "Time elapsed : " << (GetTickCount() - startTick) << std::endl;
    std::cout << "Random stuff : " << pBlocs[rand() % ALLOCATION_COUNT] << std::endl;

    startTick = GetTickCount();
    
    for(int iteration=0; iteration < SPEED_ITERATION_COUNT; iteration++)
    {
        for(uint32 ii=0; ii < pBlocs.size(); ii++)
        {
            pBlocs[ii] = malloc(sizes[ii]);
        }
        for(uint32 ii=0; ii < pBlocs.size(); ii++)
        {
            free(pBlocs[ii]);
        }
    }        
    
    std::cout << "Time elapsed : " << (GetTickCount() - startTick) << std::endl;
    std::cout << "Random stuff : " << pBlocs[rand() % ALLOCATION_COUNT] << std::endl;
    */
}

void testAllocator()
{
    UTEST_PACKAGE("Allocator");

    testAllocatorSimple();
    testAllocatorMultiple();
    testAllocatorRandom();
    testAllocatorAlignment();
}