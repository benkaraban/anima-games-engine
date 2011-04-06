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
#ifndef APPLICATIONBASE_H_
#define APPLICATIONBASE_H_

//#include <QWidget>
#include <QApplication>

#include <Core/Standard.h>
//#include <EPI/GUI/GuiDocument.moc.h>
#include <EPI/Constants.h>
namespace EPI
{
//-----------------------------------------------------------------------------
class DocumentRenderer;
class GuiDocumentManager;
class RendererManager;
class ControllerMain;
//-----------------------------------------------------------------------------
/**
*   La classe ApplicationBase est la classe mere de l'application de chaque editeur
* Elle sert de classe principale en stockant notamment les documents ouverts.
* Elle sert egalement de controlleur principale de l'editeur en gerant les actions essentielles
* ouverture/fermeture des documents ...
*/
class LM_API_EPI ApplicationBase : public QApplication, public Core::Object
{
    Q_OBJECT

public:
    virtual ~ApplicationBase();
    int exec(const Ptr<ControllerMain>& pCtrl);
protected:
    /**
    *  le constructeur
    */
    ApplicationBase(const String& appName, int & argc, char ** argv);

    int exec() {return -1;}

private:
    void initialize();

private:
    ///
    bool _isInit;
    EpiLib  epiData;
};
//-----------------------------------------------------------------------------
} //EPI
#endif APPLICATIONBASE_H_
