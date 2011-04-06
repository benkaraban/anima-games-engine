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

#include <iostream>
#include <UTests.h>
#include <Core/List.h>
#include <Core/Queue.h>
#include <Core/Standard.h>
#include <Core/RT/Thread.h>
#include <Core/RT/Mutex.h>
#include <Core/RT/ConditionVar.h>
#include <Core/RT/Semaphore.h>
#include <Core/RT/Synchronized.h>
#include <Core/RT/SyncQueue.h>
#include <Core/RT/Task.h>

using Core::Thread;
using Core::Semaphore;
using Core::Mutex;
using Core::ScopedLock;
using Core::Queue;
using Core::SyncQueue;
using Core::Synchronized;
using Core::Task;
using Core::List;

Mutex           sMutex;
Semaphore       sSem(10, 0);
Queue<int32>    sQueue;
int32           sValue = 0;
bool            sFinished = false;

SyncQueue<int32>   sSyncQueue;
Synchronized<int32> sSynchro;

class ThreadTestClass : public Core::Object
{
public:
    ThreadTestClass() : _testDone(false) {};
    
    void doTest()
    {
        _testDone = true;
    }

    bool _testDone;    
};

void threadFunc1()
{
    for(int32 ii=0; ii < 10; ii++)
    {
        {
            ScopedLock  lock(sMutex);
            int32 initialValue = sValue;
            Thread::sleep(5);
            UTEST_EQU(initialValue, sValue);
            sValue = initialValue + 1;
            Thread::sleep(3);
            UTEST_EQU(initialValue + 1, sValue);
        }
        Thread::sleep(10);                
    }
}

void threadFunc2()
{
    for(int32 ii=0; ii < 10; ii++)
    {
        {
            ScopedLock  lock(sMutex);
            int32 initialValue = sValue;
            Thread::sleep(1);
            UTEST_EQU(initialValue, sValue);
            sValue = initialValue + 1;
            Thread::sleep(10);
            UTEST_EQU(initialValue + 1, sValue);
        }
        Thread::sleep(5);
    }
}

void threadProdSQueue()
{
    for(int32 ii=0; ii < 10; ii++)
    {
        sSyncQueue.push(ii);
        Thread::sleep(1);
    }
}

void threadConsSQueue()
{
    try
    {
        for(int32 ii=0; ii < 10; ii++)
        {
            int32 value = sSyncQueue.pop();
            UTEST_EQU(value, ii);
        }
    }
    catch(Core::Exception & exception)
    {
        std::cout << exception.toString() << std::endl;
        UTEST_TEST_FAILED;
    }
}

void threadProdSQueue2()
{
    for(int32 ii=0; ii < 5; ii++)
    {
        sSyncQueue.push(ii);
        Thread::sleep(2);
    }
}

void threadConsSQueue2()
{
    int32 count = 0;
    while(count < 5)
    {
        int32 value = 0; 
        bool hasData = sSyncQueue.popIfAvailable(value);
        if(hasData)
        {
            UTEST_EQU(value, count);
            count ++;
        }
        else
        {
            Thread::sleep(1);
        }
    }
}

void threadProdSQueue3()
{
    Thread::sleep(3);
    sSyncQueue.kill();    
}
void threadConsSQueue3()
{
    UTEST_CATCH_EXCEPTION(Core::RTInterruptedException, sSyncQueue.pop());    
    UTEST_CATCH_EXCEPTION(Core::RTInterruptedException, sSyncQueue.pop());    
}

void threadSync1()
{
    sSynchro.waitGreaterThan(0);    
    UTEST_EQU(sSynchro, 1);
    sSynchro = 2;
}

void threadSync2()
{
    sSynchro.waitEquals(2);    
    UTEST_EQU(sSynchro, 2);
    sSynchro = 3;
}

void threadSync3()
{
    sSynchro.waitGreatEquals(3);    
    UTEST_EQU(sSynchro, 3);
    sSynchro = 5;
}

void threadSync4()
{
    sSynchro.waitGreatEquals(4);    
    UTEST_EQU(sSynchro, 5);
    sSynchro = -10;
}

void threadSync5()
{
    sSynchro.waitLesserThan(-5);    
    UTEST_EQU(sSynchro, -10);
    sSynchro = -15;
}

void threadSync6()
{
    sSynchro.waitLessEquals(-15);    
    UTEST_EQU(sSynchro, -15);
    sSynchro = -20;
}

void threadSync7()
{
    sSynchro.waitLessEquals(-18);    
    UTEST_EQU(sSynchro, -20);
    sSynchro = 21;
}

bool is7Multiple(const volatile int32 & value)
{
    return (value != 0) && ((value % 7) == 0);
}

void threadSync8()
{
    sSynchro.waitCondition(is7Multiple);
    UTEST_EQU(sSynchro, 21);
    sSynchro = 100;
}

class TaskFindMax : public Task
{
public:
    TaskFindMax(const List<int32> & list) : _rList(list)
    {};
    
    virtual void process()
    {
        _result = _rList[0];
        for(List<int32>::const_iterator iValue = _rList.begin(); iValue != _rList.end(); ++iValue)
        {
            if(*iValue > _result)
                _result = *iValue;
        }
    }
    
    int32 getMax() const
    {
        return _result;
    }
    
    const List<int32> & _rList;
    int32               _result;
};

void testRT()
{
    UTEST_PACKAGE("Real Time");
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Thread");

    ThreadTestClass object;
    UTEST_EQU(object._testDone, false);

    Thread thread1(&ThreadTestClass::doTest, object);

    thread1.join();
    UTEST_EQU(object._testDone, true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Mutex");
    
    Thread thread1(&threadFunc1);
    Thread thread2(&threadFunc1);
    
    thread1.join();
    thread2.join();
    
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    
    UTEST_START_TEST(L"Semaphore");
    
    sFinished = false;
    Thread threadProd(&threadFunc1);
    Thread threadCons1(&threadFunc1);
    Thread threadCons2(&threadFunc1);
    Thread threadCons3(&threadFunc1);
    
    threadProd.join();
    threadCons1.join();
    threadCons2.join();
    threadCons3.join();

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"SyncQueue / ConditionVar");
    
    Thread threadProd(&threadProdSQueue);
    Thread threadCons(&threadConsSQueue);
    
    threadProd.join();
    threadCons.join();
    
    UTEST_END_TEST;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Non blocking SyncQueue");
    
    Thread threadProd(&threadProdSQueue2);
    Thread threadCons(&threadConsSQueue2);
    
    threadProd.join();
    threadCons.join();
    
    UTEST_END_TEST;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Interrupted SyncQueue");
    
    Thread threadProd(&threadProdSQueue3);
    Thread threadCons(&threadConsSQueue3);
    
    threadProd.join();
    threadCons.join();
    
    UTEST_END_TEST;
    
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Synchronized");

    sSynchro = 0;
    
    Thread thread1(&threadSync1);
    Thread thread2(&threadSync2);
    Thread thread3(&threadSync3);
    Thread thread4(&threadSync4);
    Thread thread5(&threadSync5);
    Thread thread6(&threadSync6);
    Thread thread7(&threadSync7);
    Thread thread8(&threadSync8);
    
    sSynchro = 1;
    
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
    thread6.join();
    thread7.join();
    thread8.join();
    
    UTEST_EQU(sSynchro, 100);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Task");

    const int32 TASK_COUNT = 1000;
    const int32 ARRAY_SIZE = 1000;
    
    List<int32>  lists[TASK_COUNT];
    List<Ptr<TaskFindMax> >    tasks;
        
    for(int32 ii=0; ii < TASK_COUNT; ii++)
    {
        lists[ii].resize(ARRAY_SIZE, 0);
        lists[ii][rand()%ARRAY_SIZE] = ii + 1;
    }
    
    for(int32 ii=0; ii < TASK_COUNT; ii++)
    {
        Ptr<TaskFindMax> pTask(new TaskFindMax(lists[ii])); 
        tasks.push_back(pTask);
    }
    
    for(int32 ii=0; ii < TASK_COUNT; ii++)
    {
        Core::Task::startTask(tasks[ii].get());
    }
    
    for(int32 ii=0; ii < TASK_COUNT; ii++)
    {
        tasks[ii]->waitCompletion();
        UTEST_EQU(tasks[ii]->getMax(), ii + 1);
    }
    
    UTEST_END_TEST;

}

