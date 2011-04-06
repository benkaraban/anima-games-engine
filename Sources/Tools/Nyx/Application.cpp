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
#include <Nyx/Application.h>
#include <QPalette>

namespace Nyx
{
void changeColor(QPalette& pal,  QPalette::ColorGroup group, QPalette::ColorRole role, float lumFct)
{
    qreal h;
    qreal s;
    qreal l;
    qreal a;

    QColor color = pal.color(group,role);
        color.getHslF (&h, &s, &l, &a);
        color.setHslF(h, s, l*lumFct, a);

    pal.setColor(group, role, color);
}
//-----------------------------------------------------------------------------
Application::Application(int & argc, char ** argv):
ApplicationBase(L"Nyx", argc, argv)
{
    bool useNewPalette = false;
    
    float lumFct = 0.89f;
    QPalette    pal = palette();


    Core::List<QPalette::ColorGroup> listGroup;
        listGroup.push_back(QPalette::Active);
        listGroup.push_back(QPalette::Disabled);
        listGroup.push_back(QPalette::Inactive);


    Core::List<QPalette::ColorRole> listRole;
        listRole.push_back(QPalette::Window);
        listRole.push_back(QPalette::Background);
        listRole.push_back(QPalette::WindowText);
        listRole.push_back(QPalette::Foreground);
        listRole.push_back(QPalette::Base);
        listRole.push_back(QPalette::AlternateBase);
        listRole.push_back(QPalette::ToolTipBase);
        listRole.push_back(QPalette::ToolTipText);
        listRole.push_back(QPalette::Text);
        listRole.push_back(QPalette::ButtonText);
        listRole.push_back(QPalette::BrightText);
    

    for (int32 iGroup=0; iGroup<listGroup.size(); iGroup++)
    {
        for (int32 iRole=0; iRole<listRole.size(); iRole++)
        {
            changeColor(pal,  listGroup[iGroup], listRole[iRole], lumFct);
        }
    }



    if (useNewPalette == true)
        setPalette(pal);
}
//-----------------------------------------------------------------------------
Application::~Application()
{
}
//-----------------------------------------------------------------------------
bool Application::winEventFilter (MSG * msg, long * result)
{
    return ApplicationBase::winEventFilter (msg, result);
}
//-----------------------------------------------------------------------------
} //Nyx
