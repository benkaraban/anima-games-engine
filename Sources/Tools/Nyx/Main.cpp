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



/*
#include "qt_windows.h"

#include "qlibrary.h"

#include "Windows.h" // optional
*/
#include <QKeyEvent>


#include <Nyx/Application.h>
#include <Nyx/ApplicationWindow.moc.h>

#include <Core/Logger.h>
#include <Core/VFS/VFSDirectMapping.h>

#include <EPI/RendererManager.h>
#include <EPI/Gui/GuiDocumentManager.h>
#include <EPI/ControllerMain.moc.h>
#include <EPI/Gui/GuiDocument.moc.h>
#include <EPI/Gui/GuiContext.moc.h>
//#include <QMessageBox>

#include <EPI/Gui/Widget/DialogChooseVfsMountPoint.moc.h>
#include <EPI/Constants.h>

#include "Constants.h"
#include "NyxSetting.h"


#include <TCHAR.H>
#pragma warning(disable:4996)

bool FileExists(const String& str)
{
    String8 str8 (str);
    const TCHAR *fileName = str8.c_str();
    DWORD       fileAttr;

    fileAttr = GetFileAttributes(fileName);
    if (0xFFFFFFFF == fileAttr)
        return false;
    return true;
}
//-----------------------------------------------------------------------------
bool getPathAndNameInitFile(Core::String& pathAndNameInitFile)
{
    Core::String appData(Core::System::getUserLocalAppDir() + L"/AnimaGames/");

    if(!appData.empty())
    {
        pathAndNameInitFile = appData + L"\\Nyx\\" + Nyx::NYX_INIT_FILE_NAME;
        return true;
	}
    else
    {
        return false;
    }
}
//-----------------------------------------------------------------------------
void makeSettingBeforeAllAction(const Ptr<Nyx::NyxSetting>& pSet)
{
    Ptr<Nyx::GeneralSetting> pGS = LM_DEBUG_PTR_CAST<Nyx::GeneralSetting>(pSet->getSetting(Nyx::GENERAL_SETTING));

    if (pGS->showConsol == true)
        if (AllocConsole()) {freopen("CONOUT$", "w", stdout); freopen("CONOUT$","w",stderr);}
}
//-----------------------------------------------------------------------------
void initialiseSetting(const Ptr<Nyx::NyxSetting>& pSetting, const Ptr<EPI::ControllerMain>& pCtrl)
{
    Ptr<Nyx::GeneralSetting> pGS = LM_DEBUG_PTR_CAST<Nyx::GeneralSetting>(pSetting->getSetting(Nyx::GENERAL_SETTING));

    pCtrl->setMakeSleepForGDoc(pGS->sleepBetweenTwoUpdateWorld);
    
    EPI::MiscConst::pathWorldDefault   = pGS->paths.defaultWorld;
    EPI::MiscConst::defaultTagGameplay = pGS->paths.defaultTagGameplay;
    EPI::MiscConst::memoPathShared     = pGS->paths.memoPathShared;
    EPI::MiscConst::memoPathVfs        = pGS->paths.memoPathVfs;

    if (pGS->playSound == true)
        EPI::MiscConst::sSoundGlobalGain              = 1.f;
    else
        EPI::MiscConst::sSoundGlobalGain              = 0.f;
}

//-----------------------------------------------------------------------------

Nyx::ApplicationWindow* gAppWindow = NULL;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam )
{
    BOOL fEatKeystroke = FALSE;

    
    if (nCode == HC_ACTION)
    {
        switch (wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
            
              /*
            if ((p->flags & LLKHF_ALTDOWN) && (!GetKeyState(VK_CONTROL)))
            {
                if (gAppWindow!=NULL)
                {
                    Ptr<EPI::GuiDocument> pGDoc =  gAppWindow->getControllerMain()->getDocumentManager()->getCurrentGuiDocument();
                    if (pGDoc!=null)
                    {
                        EPI::WorldViewportContainer* pViewC = pGDoc->getGuiContext()->getCurrentViewport();
                        if (pViewC!=NULL)
                        {
                            pViewC->getView()->setFocus();
                        }
                    }
                }
                
            }*/

            if ((p->flags & LLKHF_ALTDOWN) && p->vkCode == VK_SPACE)
            {
                fEatKeystroke = TRUE;
                QKeyEvent event (QKeyEvent::KeyPress, Qt::Key_Space, Qt::AltModifier, "", true);
                QApplication::sendEvent(QApplication::focusWidget(), &event);
            }

/*
            //pour virer le window Tab
            fEatKeystroke =
                        ((p->vkCode == VK_TAB)    && ((p->flags & LLKHF_ALTDOWN) != 0))  ||
                        ((p->vkCode == VK_ESCAPE) && ((p->flags & LLKHF_ALTDOWN) != 0))  ||
                        (p->vkCode == VK_LWIN) ||  (p->vkCode == VK_RWIN)                ||
                        (p->vkCode== VK_APPS)  ||
                        ((p->vkCode == VK_ESCAPE) && ((GetKeyState(VK_CONTROL) & 0x8000)!= 0));*/
         break;
      }
   }
    
    if (fEatKeystroke == FALSE) return CallNextHookEx(NULL, nCode, wParam, lParam);

    
    return 1;
}

//int main(int __argc, char * __argv[])
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    
    int32  result = 1;

    String initFileName;
        bool goodPath = getPathAndNameInitFile(initFileName);
        if (goodPath==false)    {return -1;}

    Core::String fileName;
        if(__argc > 1) fileName = Core::String(__argv[1]);

    Ptr<Nyx::NyxSetting>     pSetting (new Nyx::NyxSetting (initFileName));
    if (FileExists(initFileName) == false)
        pSetting->writeSetting();

    Ptr<Nyx::GeneralSetting> pGeneral = LM_DEBUG_PTR_CAST<Nyx::GeneralSetting>(pSetting->getSetting(Nyx::GENERAL_SETTING));

    makeSettingBeforeAllAction(pSetting);
    
    try
    {
        Ptr<Nyx::Application> app = Ptr<Nyx::Application>(new Nyx::Application(__argc, __argv));
        HHOOK hhkLowLevelKybd  = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardProc, hInst, 0);

        if (pGeneral->showWorkingDirectoryDialog == true || pGeneral->workingDirectoryPaths.size() == 0)
        {
            EPI::DialogChooseVfsMountPoint window(pGeneral->workingDirectoryPaths, pGeneral->workingDirectoryIndex);

            if (window.exec() == QDialog::Accepted)
            {
                window.getDialogOption(pGeneral->workingDirectoryPaths, pGeneral->workingDirectoryIndex, pGeneral->showWorkingDirectoryDialog);
                pGeneral->writeSetting();
            }
            else return 0;
        } 
        

        String workDir = pGeneral->workingDirectoryPaths[pGeneral->workingDirectoryIndex];
        SetCurrentDirectory(String8(workDir).c_str());

        Ptr<EPI::RendererManager>    pRdrMng  (new EPI::RendererManager());
        Ptr<EPI::GuiDocumentManager> pGDocMng (new EPI::GuiDocumentManager());
        Ptr<EPI::ControllerMain>     pCtrl    (new EPI::ControllerMain (pGDocMng, pRdrMng));

        initialiseSetting(pSetting, pCtrl);

        Core::logStdOut_Off();
        Ptr<Nyx::RendererSetting> pNyxRdrSet = LM_DEBUG_PTR_CAST<Nyx::RendererSetting>(pSetting->getSetting(Nyx::RENDERER_SETTING));
        pNyxRdrSet->apply(pCtrl->getRendererManager()->getDefaultRenderer());
        Core::logStdOut_On();
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED | ES_DISPLAY_REQUIRED);

        gAppWindow = new Nyx::ApplicationWindow (pSetting, pCtrl, 0, 0);
       // HPOWERNOTIFY  handleRegisterPwSetting = RegisterPowerSettingNotification(gAppWindow->winId(), &GUID_ACDC_POWER_SOURCE, DEVICE_NOTIFY_WINDOW_HANDLE);
            gAppWindow->show();
        
        if(fileName != Core::String::EMPTY)
            gAppWindow->openVFSFile(fileName);

        result = app->exec(pCtrl);
      //  UnregisterPowerSettingNotification(handleRegisterPwSetting);

        if (result == 0)
        {
            pGeneral->paths.memoPathShared = EPI::MiscConst::memoPathShared;
            pGeneral->writeSetting();
        }
    }
    catch(Core::Exception & e)
    {
        pGeneral->showWorkingDirectoryDialog = true;
        pGeneral->writeSetting();
        ERR << L"Unhandled exception : " << e.getMessage() << L"\n";
        result = -1;
    }

    return result;
}
//-----------------------------------------------------------------------------
