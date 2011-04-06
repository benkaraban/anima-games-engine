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

#include <Core/Standard.h>
#include <Core/XML/XMLDocument.h>
#include <Core/IO/BufferStream.h>

void testXML()
{
    UTEST_PACKAGE("XML");
    Core::XMLDocument document;

    Core::XMLDocument root;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Initialisation");
    UTEST_EQU(document.setAttribute(L"config-gfx", L"detail", L"High"), true);
    UTEST_EQU(document.setAttribute(L"config-gfx", L"stuff", 17), true);
    UTEST_EQU(document.setValue(L"config-gfx.resolution.width", 1680), true);
    UTEST_EQU(document.setValue(L"config-gfx.resolution.height", 1050), true);
    UTEST_EQU(document.setAttribute(L"config-gfx.resolution", L"depth", 32), true);
    UTEST_EQU(document.setValue(L"config-gfx.input.keyboard", L"Azerty"), true);
    UTEST_EQU(document.setAttribute(L"config-gfx.input.keyboard", L"keys-count", 106), true);

    UTEST_EQU(document.setAttribute(L".should.not", L"work", 666), false);
    UTEST_EQU(document.setValue(L"should..not.work", 666), false);
    UTEST_EQU(document.setAttribute(L"should.not.", L"work", 666), false);
    UTEST_EQU(document.setValue(L"", 666), false);

    Core::List<Core::String> values;
    values.push_back(L"First");
    values.push_back(L"Second");
    UTEST_EQU(document.setValues(L"config-gfx.test", values), true);
    UTEST_EQU(document.setAttribute(L"config-gfx.test", L"first-attribute", L"things", L"First"), true);
    UTEST_EQU(document.setAttribute(L"config-gfx.test", L"second-attribute", L"stuffs", L"Second"), true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Lecture des data");

    int32 intValue;
    String value;
    UTEST_EQU(document.getAttribute(L"config-gfx", L"detail", value) && value == L"High", true);
    UTEST_EQU(document.getAttribute(L"config-gfx", L"stuff", intValue) && intValue == 17, true);
    UTEST_EQU(document.getValue(L"config-gfx.resolution.width", intValue) && intValue == 1680, true);
    UTEST_EQU(document.getValue(L"config-gfx.resolution.height", intValue)&& intValue == 1050, true);
    UTEST_EQU(document.getAttribute(L"config-gfx.resolution", L"depth", intValue) && intValue == 32, true);
    UTEST_EQU(document.getValue(L"config-gfx.input.keyboard", value) && value == L"Azerty", true);
    UTEST_EQU(document.getAttribute(L"config-gfx.input.keyboard", L"keys-count", intValue) && intValue == 106, true);

    UTEST_EQU(document.getValue(L"should.not.work", intValue), false);
    UTEST_EQU(document.getAttribute(L".should.not", L"work", intValue), false);
    UTEST_EQU(document.getValue(L"should..not.work", intValue), false);
    UTEST_EQU(document.getAttribute(L"should.not.", L"work", intValue), false);
    UTEST_EQU(document.getValue(L"", intValue), false);

    Core::List<Core::String> values;
    UTEST_EQU(document.getValues(L"config-gfx.test", values)&& (values.size() == 2), true);
    UTEST_EQU(document.getAttribute(L"config-gfx.test", L"first-attribute", value, L"First") && value == L"things", true);
    UTEST_EQU(document.getAttribute(L"config-gfx.test", L"second-attribute", value, L"Second") && value == L"stuffs", true);

    UTEST_END_TEST;
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Modification des data");
    UTEST_EQU(document.setAttribute(L"config-gfx", L"detail", L"Low"), true);
    UTEST_EQU(document.setAttribute(L"config-gfx", L"stuff", L"Things"), true);
    UTEST_EQU(document.setValue(L"config-gfx.resolution.width", 1024), true);
    UTEST_EQU(document.setValue(L"config-gfx.resolution.height", 768), true);
    UTEST_EQU(document.setAttribute(L"config-gfx.resolution", L"depth", 16), true);

    String value;
    int32 intValue;

    UTEST_EQU(document.getAttribute(L"config-gfx", L"detail", value) && value == L"Low", true);
    UTEST_EQU(document.getAttribute(L"config-gfx", L"stuff", value) && value == L"Things", true);
    UTEST_EQU(document.getValue(L"config-gfx.resolution.width", intValue) && intValue == 1024, true);
    UTEST_EQU(document.getValue(L"config-gfx.resolution.height", intValue)&& intValue == 768, true);
    UTEST_EQU(document.getAttribute(L"config-gfx.resolution", L"depth", intValue) && intValue == 16, true);

    UTEST_END_TEST;
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Suppression de data");
    int32 intValue;
    String value;
    UTEST_EQU(document.deleteAttribute(L"config-gfx.input.keyboard", L"keys-count"), true);
    UTEST_EQU(document.getAttribute(L"config-gfx.input.keyboard", L"keys-count", intValue), false);

    UTEST_EQU(document.deleteNode(L"config-gfx", L"input"), true);
    UTEST_EQU(document.getValue(L"config-gfx.input.keyboard", value), false);

    //Vérification que les autres données soient toujours présentes.
    UTEST_EQU(document.getAttribute(L"config-gfx", L"detail", value) && value == L"Low", true);
    UTEST_EQU(document.getAttribute(L"config-gfx", L"stuff", value) && value == L"Things", true);
    UTEST_EQU(document.getValue(L"config-gfx.resolution.width", intValue) && intValue == 1024, true);
    UTEST_EQU(document.getValue(L"config-gfx.resolution.height", intValue)&& intValue == 768, true);
    UTEST_EQU(document.getAttribute(L"config-gfx.resolution", L"depth", intValue) && intValue == 16, true);

    UTEST_EQU(document.deleteNode(L"config-gfx", L"test"), true);
    Core::List<Core::String> values;
    UTEST_EQU(document.getValues(L"config-gfx.test", values), false);

    UTEST_END_TEST;
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Gestion des noeuds");

    Ptr<Core::XMLNode> node1(new Core::XMLNode(L"effects"));
    Ptr<Core::XMLNode> node2(new Core::XMLNode(L"effects"));
    Ptr<Core::XMLNode> node3(new Core::XMLNode(L"effects"));

    node1->setAttribute(L"effect", L"type", L"force");
    node1->setValue(L"effect.force.x", 100);
    node1->setValue(L"effect.force.y", 200);
    node1->setValue(L"effect.force.z", 300);
    node1->setValue(L"effect.force.acceleration", 400);

    node2->setAttribute(L"effect", L"type", L"force");
    node2->setValue(L"effect.force.x", 1);
    node2->setValue(L"effect.force.y", 2);
    node2->setValue(L"effect.force.z", 3);
    node2->setValue(L"effect.force.acceleration", 4);

    node3->setAttribute(L"effect", L"type", L"fade2");
    node3->setValue(L"effect.fade2.color1", L"Blue");
    node3->setValue(L"effect.fade2.color2", L"White");
    node3->setValue(L"effect.fade2.color3", L"Red");

    UTEST_EQU(root.addNode(L"node-emitter", node1), true);
    UTEST_EQU(root.addNode(L"node-emitter", node2), true);
    UTEST_EQU(root.addNode(L"node-emitter", node3), true);

    Core::List<Core::Ptr<Core::XMLNode> > nodes;
    UTEST_EQU(root.getNodes(L"node-emitter", L"effects", nodes) && nodes.size() == 3, true);
    for(int32 ii = 0; ii < nodes.size(); ++ii)
    {
        bool forceOrFade2 = false;
        String value;
        int32 intValue;
        nodes[ii]->getAttribute(L"effect", L"type", value);
        if(value == L"force")
        {
            forceOrFade2 = true;
            UTEST_EQU(nodes[ii]->getValue(L"effect.force.x", intValue), true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.force.y", intValue), true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.force.z", intValue), true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.force.acceleration", intValue), true);
        }
        else if(value == L"fade2")
        {
            forceOrFade2 = true;
            UTEST_EQU(nodes[ii]->getValue(L"effect.fade2.color1", value) && value == L"Blue", true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.fade2.color2", value) && value == L"White", true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.fade2.color3", value) && value == L"Red", true);
        }

        UTEST_EQU(forceOrFade2, true);
    }

    UTEST_END_TEST;
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Sauvegarde/Chargement");

    Core::List<Core::String> values;
    values.push_back(L"First");
    values.push_back(L"Second");
    UTEST_EQU(document.setValues(L"config-gfx.test", values), true);
    UTEST_EQU(document.setAttribute(L"config-gfx.test", L"first-attribute", L"things", L"First"), true);
    UTEST_EQU(document.setAttribute(L"config-gfx.test", L"second-attribute", L"stuffs", L"Second"), true);

    UTEST_EQU(document.saveDocument(L"Data/TUData/Tmp/appSettings.xml"), true);
    UTEST_EQU(root.saveDocument(L"Data/TUData/Tmp/effects.xml"), true);

    Core::XMLDocument loadingTest;
    UTEST_EQU(loadingTest.loadDocument(L"Data/TUData/Tmp/appSettings.xml"), true);

    int32 intValue;
    String value;
    UTEST_EQU(loadingTest.getAttribute(L"config-gfx", L"detail", value) && value == L"Low", true);
    UTEST_EQU(loadingTest.getAttribute(L"config-gfx", L"stuff", value) && value == L"Things", true);
    UTEST_EQU(loadingTest.getValue(L"config-gfx.resolution.width", intValue) && intValue == 1024, true);
    UTEST_EQU(loadingTest.getValue(L"config-gfx.resolution.height", intValue)&& intValue == 768, true);
    UTEST_EQU(loadingTest.getAttribute(L"config-gfx.resolution", L"depth", intValue) && intValue == 16, true);
    Core::List<Core::String> loadValues;
    UTEST_EQU(loadingTest.getValues(L"config-gfx.test", loadValues)&& (loadValues.size() == 2), true);
    UTEST_EQU(loadingTest.getAttribute(L"config-gfx.test", L"first-attribute", value, L"First") && value == L"things", true);
    UTEST_EQU(loadingTest.getAttribute(L"config-gfx.test", L"second-attribute", value, L"Second") && value == L"stuffs", true);

    Core::XMLDocument loadingTest2;
    Core::List<Core::Ptr<Core::XMLNode> > nodes;
    UTEST_EQU(root.getNodes(L"node-emitter", L"effects", nodes) && nodes.size() == 3, true);
    for(int32 ii = 0; ii < nodes.size(); ++ii)
    {
        bool forceOrFade2 = false;
        String value;
        int32 intValue;
        nodes[ii]->getAttribute(L"effect", L"type", value);
        if(value == L"force")
        {
            forceOrFade2 = true;
            UTEST_EQU(nodes[ii]->getValue(L"effect.force.x", intValue), true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.force.y", intValue), true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.force.z", intValue), true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.force.acceleration", intValue), true);
        }
        else if(value == L"fade2")
        {
            forceOrFade2 = true;
            UTEST_EQU(nodes[ii]->getValue(L"effect.fade2.color1", value) && value == L"Blue", true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.fade2.color2", value) && value == L"White", true);
            UTEST_EQU(nodes[ii]->getValue(L"effect.fade2.color3", value) && value == L"Red", true);
        }

        UTEST_EQU(forceOrFade2, true);
    }

    //Sauvegarde et chargement dans des STREAMS maintenant
    Core::String8 xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\
    <config-gfx detail=\"High\" stuff=\"17\">\
        <resolution depth=\"32\">\
            <width>1680</width>\
            <height>1050</height>\
        </resolution>\
    </config-gfx>";

    Core::XMLDocument xmlDocument3;

    Core::BufferStream bufferStream;
    bufferStream.writeData(xmlString.c_str(), xmlString.length());

    //1) On charge le bufferstream dans le doc XML
    UTEST_EQU(xmlDocument3.loadDocument(bufferStream), true);

    //2) On vérifie que les valeurs sont exactes
    UTEST_EQU(xmlDocument3.getAttribute(L"config-gfx",L"detail", value) && value == L"High", true);
    UTEST_EQU(xmlDocument3.getAttribute(L"config-gfx",L"stuff", intValue) && intValue == 17, true);
    UTEST_EQU(xmlDocument3.getValue(L"config-gfx.resolution.width", intValue) && intValue == 1680, true);
    UTEST_EQU(xmlDocument3.getValue(L"config-gfx.resolution.height", intValue) && intValue == 1050, true);
    UTEST_EQU(xmlDocument3.getAttribute(L"config-gfx.resolution",L"depth", intValue) && intValue == 32, true);

    Core::BufferStream bufferStream2;
    //3) On sauve dans un deuxième bufferstream
    UTEST_EQU(xmlDocument3.saveDocument(bufferStream2), true);

    Core::XMLDocument xmlDocument4;
    //4) On le charge dans un document vierge
    UTEST_EQU(xmlDocument4.loadDocument(bufferStream2), true);
    //5) On vérifie que les données sont toujours correctes
    UTEST_EQU(xmlDocument4.getAttribute(L"config-gfx",L"detail", value) && value == L"High", true);
    UTEST_EQU(xmlDocument4.getAttribute(L"config-gfx",L"stuff", intValue) && intValue == 17, true);
    UTEST_EQU(xmlDocument4.getValue(L"config-gfx.resolution.width", intValue) && intValue == 1680, true);
    UTEST_EQU(xmlDocument4.getValue(L"config-gfx.resolution.height", intValue) && intValue == 1050, true);
    UTEST_EQU(xmlDocument4.getAttribute(L"config-gfx.resolution",L"depth", intValue) && intValue == 32, true);

    UTEST_END_TEST;
    //-------------------------------------------------------------------------
}
