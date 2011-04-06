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

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Queue.h>
#include <Core/Set.h>
#include <Core/Map.h>
#include <Core/Buffer.h>
#include <Core/IO/BufferStream.h>
#include <UTests.h>

class MyException : public Core::Exception
{
public:
    MyException(const String & string) : Core::Exception(string)
    {};
};

int32 childCount = 0;
int32 stuffDone  = 0;

class Child : public Core::Object
{
public:
    Child() { childCount++; };
    ~Child() { childCount--; };
    virtual void doStuff() {};
};

class GreatChild : public Child
{
public:
    virtual void doStuff() {stuffDone++;};
};

void testTypes()
{
    UTEST_PACKAGE("Standard");

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Tailles types");

    UTEST_EQU(int32(sizeof(Core::byte)), 1);
    UTEST_EQU(int32(sizeof(Core::int8)), 1);
    UTEST_EQU(int32(sizeof(Core::uint8)), 1);
#ifdef _WIN32
    UTEST_EQU(int32(sizeof(wchar_t)), 2);
#else
    UTEST_EQU(int32(sizeof(wchar_t)), 4);
#endif
    UTEST_EQU(int32(sizeof(Core::int16)), 2);
    UTEST_EQU(int32(sizeof(Core::uint16)), 2);
    UTEST_EQU(int32(sizeof(Core::int32)), 4);
    UTEST_EQU(int32(sizeof(Core::uint32)), 4);
    UTEST_EQU(int32(sizeof(Core::int64)), 8);
    UTEST_EQU(int32(sizeof(Core::uint64)), 8);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Exception");

    try
    {
        throw MyException(L"Test");
    }
    catch(Core::Exception & exception)
    {
        UTEST_EQU(exception.getMessage(), L"Test");
        UTEST_EQU(toString(exception), L"MyException : Test");
    }

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"List");

    Core::List<int32> list;

    UTEST_EQU(toString(list), L"[]");
    list.push_back(1);
    UTEST_EQU(toString(list), L"[1]");
    list.push_back(2);
    UTEST_EQU(toString(list), L"[1, 2]");
    list.push_back(3);
    UTEST_EQU(toString(list), L"[1, 2, 3]");

    Core::List<int32> list2(list);
    UTEST_EQU(list, list2);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Queue");

    Core::Queue<int32> queue;

    UTEST_EQU(toString(queue), L"Queue(size = 0)");
    queue.push(1);
    UTEST_EQU(toString(queue), L"Queue(size = 1, first = 1)");
    queue.push(2);
    UTEST_EQU(toString(queue), L"Queue(size = 2, first = 1)");
    queue.push(3);
    UTEST_EQU(toString(queue), L"Queue(size = 3, first = 1)");

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Set");

    Core::Set<int32> set;

    UTEST_EQU(toString(set), L"()");
    set.insert(3);
    UTEST_EQU(toString(set), L"(3)");
    set.insert(1);
    UTEST_EQU(toString(set), L"(1, 3)");
    set.insert(2);
    UTEST_EQU(toString(set), L"(1, 2, 3)");
    set.insert(2);
    UTEST_EQU(toString(set), L"(1, 2, 3)");
    set.insert(1);
    UTEST_EQU(toString(set), L"(1, 2, 3)");

    Core::Set<int32> set2(set);
    UTEST_EQU(set, set2);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"List -> Set");

    Core::List<int32> list;

    list.push_back(2);
    list.push_back(1);
    list.push_back(3);

    Core::Set<int32> set(list.begin(), list.end());
    UTEST_EQU(toString(set), L"(1, 2, 3)");
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Set -> List");

    Core::Set<int32> set;
    set.insert(3);
    set.insert(2);
    set.insert(1);

    Core::List<int32> list(set.begin(), set.end());

    UTEST_EQU(toString(list), L"[1, 2, 3]");
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Map");

    Core::Map<Core::String, int32> map1;

    UTEST_EQU(toString(map1), L"()");
    map1[L"A"] = 1;
    UTEST_EQU(toString(map1), L"({A : 1})");
    map1[L"B"] = 2;
    UTEST_EQU(toString(map1), L"({A : 1}, {B : 2})");
    map1[L"C"] = 3;
    UTEST_EQU(toString(map1), L"({A : 1}, {B : 2}, {C : 3})");

    Core::Map<Core::String8, int32> map2;

    map2["Y"] = 6;
    map2["X"] = 5;

    UTEST_EQU(toString(map2), L"({X : 5}, {Y : 6})");

    Core::Map<Core::uint16, String> map3;

    map3[43] = L"事文字";
    map3[27] = L"仮名指";

    UTEST_EQU(toString(map3), L"({27 : 仮名指}, {43 : 事文字})");

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Buffer");

    Core::Buffer buffer(3);
    UTEST_EQU(buffer.size(), 3);
    byte * data = buffer.getData();
    data[0] = 0;
    data[1] = 1;
    data[2] = 2;
    UTEST_EQU(toString(buffer), L"[0, 1, 2]");

    Core::BufferStream bufferStream;
    buffer.write(bufferStream);

    Core::Buffer buffer2(bufferStream);
    UTEST_EQU(buffer2.size(), 3);

    UTEST_EQU(toString(buffer2), L"[0, 1, 2]");

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Ptr");

    {
        Ptr<Child> pChild1(new Child());
        pChild1->doStuff();
        UTEST_EQU(childCount, 1);
    }
    UTEST_EQU(childCount, 0);

    {
        Ptr<Child> pChild1(new Child());
        UTEST_EQU(childCount, 1);

        Ptr<Child> pChild2(new Child());
        UTEST_EQU(childCount, 2);

        Ptr<Child> pChild3(pChild1);
        UTEST_EQU(childCount, 2);

        pChild2 = pChild3;
        UTEST_EQU(childCount, 1);
    }
    UTEST_EQU(childCount, 0);

    {
        Ptr<Child> pChild1(new GreatChild());
        UTEST_EQU(childCount, 1);
        pChild1->doStuff();
        UTEST_EQU(stuffDone, 1);

        String expected(L"<GreatChild @ 0x");

        expected << toString(pChild1.get()) << L">";

        UTEST_EQU(toString(pChild1), expected);
    }
    UTEST_EQU(childCount, 0);
    UTEST_EQU(stuffDone, 1);

    Core::List<Ptr<Child> > children;

    for(int32 ii=1; ii <= 100; ii++)
    {
        Ptr<Child> pChild(new Child());
        children.push_back(pChild);
        UTEST_EQU(childCount, ii);
    }

    children.push_back(children[0]);
    UTEST_EQU(childCount, 100);
    children.push_back(children[1]);
    UTEST_EQU(childCount, 100);
    children.pop_back();
    UTEST_EQU(childCount, 100);
    children.pop_back();
    UTEST_EQU(childCount, 100);
    children.pop_back();
    UTEST_EQU(childCount, 99);
    children.clear();
    UTEST_EQU(childCount, 0);

    for(int32 ii=1; ii <= 100; ii++)
    {
        Ptr<Child> pChild(new Child());
        children.push_back(pChild);
        UTEST_EQU(childCount, ii);
    }

    children.push_back(children[0]);
    UTEST_EQU(childCount, 100);
    children.push_back(children[1]);
    UTEST_EQU(childCount, 100);
    children.pop_back();
    UTEST_EQU(childCount, 100);
    children.pop_back();
    UTEST_EQU(childCount, 100);
    children.pop_back();
    UTEST_EQU(childCount, 99);
    children.clear();
    UTEST_EQU(childCount, 0);

    UTEST_END_TEST;


}
