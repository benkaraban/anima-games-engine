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
#include <UTests.h>
#include <Core/Exception.h>
#include <Core/Containers.h>
#include <Core/StopWatch.h>

#include <Core/Math/ModuleMath.h>
#include <iomanip>

using namespace Core;

struct BoxClass : public Object, public IBoxable
{
    BoxClass(const AABoxf & box) : box(box) {};

    virtual const Core::AABoxf & getBoundingBox() const
    { return box; };

    virtual String toString() const
    { return Core::toString(box); };

    AABoxf box;
};

void testKDTree()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"KDTree")

    KDTree tree1;

    Ptr<BoxClass> pBoxA(new BoxClass(AABoxf(Vector3f(  1.0,  1.0,  0.0), Vector3f(  3.0,  3.0,  1.0))));
    Ptr<BoxClass> pBoxB(new BoxClass(AABoxf(Vector3f(  5.0,  1.0,  0.0), Vector3f(  7.0,  3.0,  1.0))));
    Ptr<BoxClass> pBoxC(new BoxClass(AABoxf(Vector3f(  9.0,  1.0,  0.0), Vector3f( 12.0,  3.0,  1.0))));
    Ptr<BoxClass> pBoxD(new BoxClass(AABoxf(Vector3f(  2.0,  4.0,  0.0), Vector3f(  4.0,  6.0,  1.0))));
    Ptr<BoxClass> pBoxE(new BoxClass(AABoxf(Vector3f(  6.0,  4.0,  0.0), Vector3f(  9.0,  6.0,  1.0))));
    Ptr<BoxClass> pBoxF(new BoxClass(AABoxf(Vector3f( 10.0,  4.0,  0.0), Vector3f( 14.0,  6.0,  1.0))));

    tree1.add(pBoxA); tree1.add(pBoxB); tree1.add(pBoxC);
    tree1.add(pBoxD); tree1.add(pBoxE); tree1.add(pBoxF);

    tree1.optimize(1);

    List<Ptr<IBoxable> > listEmpty;
    List<Ptr<IBoxable> > listA;
    List<Ptr<IBoxable> > listAB;
    List<Ptr<IBoxable> > listABC;
    List<Ptr<IBoxable> > listAD;
    List<Ptr<IBoxable> > listBE;
    List<Ptr<IBoxable> > listBEC;
    List<Ptr<IBoxable> > listBECF;
    List<Ptr<IBoxable> > listF;

    listA.push_back(pBoxA);
    listAB.push_back(pBoxA);
    listAB.push_back(pBoxB);

    listABC.push_back(pBoxA);
    listABC.push_back(pBoxB);
    listABC.push_back(pBoxC);

    listAD.push_back(pBoxA);
    listAD.push_back(pBoxD);

    listBE.push_back(pBoxB);
    listBE.push_back(pBoxE);

    listBEC.push_back(pBoxB);
    listBEC.push_back(pBoxE);
    listBEC.push_back(pBoxC);

    listBECF.push_back(pBoxB);
    listBECF.push_back(pBoxE);
    listBECF.push_back(pBoxC);
    listBECF.push_back(pBoxF);

    listF.push_back(pBoxF);

    List<Ptr<IBoxable> > listTestEmpty;
    List<Ptr<IBoxable> > listTestA;
    List<Ptr<IBoxable> > listTestAB;
    List<Ptr<IBoxable> > listTestABC;
    List<Ptr<IBoxable> > listTestAD;
    List<Ptr<IBoxable> > listTestBE;
    List<Ptr<IBoxable> > listTestBECF;
    List<Ptr<IBoxable> > listTestF;

    tree1.getBoxContent(AABoxf(Vector3f( 0.0f, 0.0f, 1.5f), Vector3f( 3.5f, 3.5f, 2.0f)), listTestEmpty);
    tree1.getBoxContent(AABoxf(Vector3f( 0.0f, 0.0f, 0.0f), Vector3f( 3.5f, 3.5f, 1.0f)), listTestA);
    tree1.getBoxContent(AABoxf(Vector3f( 0.0f, 0.0f, 0.0f), Vector3f( 7.0f, 3.5f, 1.0f)), listTestAB);
    tree1.getBoxContent(AABoxf(Vector3f( 0.0f, 0.0f, 0.0f), Vector3f(13.5f, 3.5f, 1.0f)), listTestABC);
    tree1.getBoxContent(AABoxf(Vector3f( 0.0f, 0.0f, 0.0f), Vector3f( 5.0f, 6.0f, 1.0f)), listTestAD);
    tree1.getBoxContent(AABoxf(Vector3f( 5.0f, 0.0f, 0.0f), Vector3f(10.5f, 6.0f, 1.0f)), listTestBE);
    tree1.getBoxIntersection(AABoxf(Vector3f( 5.0f, 0.0f, 0.5f), Vector3f(10.5f, 6.0f, 0.7f)), listTestBECF);
    tree1.getBoxIntersection(AABoxf(Vector3f(11.0f, 4.5f, 0.5f), Vector3f(12.0f, 5.5f, 0.7f)), listTestF);

    UTEST_EQU(sameContent(listEmpty, listTestEmpty), true);
    UTEST_EQU(sameContent(listA, listTestA), true);
    UTEST_EQU(sameContent(listAB, listTestAB), true);
    UTEST_EQU(sameContent(listABC, listTestABC), true);
    UTEST_EQU(sameContent(listAD, listTestAD), true);
    UTEST_EQU(sameContent(listBE, listTestBE), true);
    UTEST_EQU(sameContent(listBECF, listTestBECF), true);
    UTEST_EQU(sameContent(listF, listTestF), true);

    const int32 BOX_COUNT      = 100;
    const float BOX_DELTA_POS  = 100.0;
    const float BOX_DELTA_SIZE = 10.0;
    const int32 SELEC_COUNT    = 100000;
    const float BOX_DELTA_SELEC_SIZE = 30.0;

    List<Ptr<IBoxable> > boxes;
    KDTree tree2;

    for(int32 ii=0; ii < BOX_COUNT; ii++)
    {
        Vector3f position;
        randomize(position, -BOX_DELTA_POS, BOX_DELTA_POS);
        Vector3f size;
        randomize(size, 0.0, BOX_DELTA_SIZE);

        Ptr<IBoxable> pBox(new BoxClass(AABoxf(position, position + size)));
        boxes.push_back(pBox);
        tree2.add(pBox);
    }

    tree2.optimize(1);

    List<AABoxf>  selections;

    for(int32 ii=0; ii < SELEC_COUNT; ii++)
    {
        Vector3f position;
        randomize(position, -BOX_DELTA_POS, BOX_DELTA_POS);
        Vector3f size;
        randomize(size, 0.5f * BOX_DELTA_SELEC_SIZE, BOX_DELTA_SELEC_SIZE);
        selections.push_back(AABoxf(position, position + size));
    }

    /*  --- BENCH entre la force brute et un KDTree

    StopWatch watch;

    watch.reset();
    foreach(iSelec, selections)
    {
        List<Ptr<IBoxable> > listKDTree;
        tree2.getBoxIntersection(*iSelec, listKDTree);
        (void)listKDTree;
    }

    std::cout << watch.elapsed().toString() << std::endl;

    watch.reset();
    foreach(iSelec, selections)
    {
        List<Ptr<IBoxable> > listBruteForce;
        foreach(iBox, boxes)
        {
            const AABoxf & box = (*iBox)->getBoundingBox();
            if(box.inter(*iSelec))
                listBruteForce.push_back(*iBox);
        }
        (void)listBruteForce;
    }

    std::cout << watch.elapsed().toString() << std::endl;
    */

    UTEST_END_TEST;
}
