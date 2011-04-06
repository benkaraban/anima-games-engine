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
 #ifndef NYX_CONFIGDIALOG_H
 #define NYX_CONFIGDIALOG_H

#include <Core/Standard.h>

#include <QDialog>

#include "NyxSetting.h"

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QCheckBox;


namespace QtToolbox
{
    class LineEdit;
    class ComboBox;
}

namespace Nyx
{


//-----------------------------------------------------------------------------
class ConfigGeneral : public QWidget
{
public:
    ConfigGeneral(const Ptr<ISetting>& pSet, QWidget* parent=0);
    void writeConfig(const Ptr<ISetting>& pSet);

private:
    void setupUi();
    void initialize(const Ptr<ISetting>& pSet);

private:
    QCheckBox*                  pShowStartupDialog;
    QCheckBox*                  pShowConsol;
    QCheckBox*                  pMakeSleepBetweenTwoUpdate;
    QtToolbox::LineEdit *       pWorldDefault;
    QtToolbox::LineEdit *       pDefaultTagGameplay;
    QCheckBox*                  pPlaySound;
};
//-----------------------------------------------------------------------------
class ConfigRenderer : public QWidget
{
    Q_OBJECT

public:
    ConfigRenderer(const Ptr<ISetting>& pSet, QWidget* parent=0);
    void writeConfig(const Ptr<ISetting>& pSet);

private:
    void setupUi();
    void initialize(const Ptr<ISetting>& pSet);

private Q_SLOTS:
    void initToGlobal();

private:
    QtToolbox::ComboBox* _pGlobalLevel;
    QtToolbox::ComboBox* _pShaderLevel;
    QtToolbox::ComboBox* _pShadowLevel;
    QtToolbox::ComboBox* _pTextureLevel;
    QtToolbox::ComboBox* _pReflecLevel;
    QtToolbox::ComboBox* _pRefracLevel;
    QtToolbox::ComboBox* _pFilterLevel;
    QtToolbox::ComboBox* _pMsaaLevel;
};
//-----------------------------------------------------------------------------
class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog(const Ptr<NyxSetting>& pSetting, QWidget* parent = 0);
    bool updateRenderer() {return _updateRdr;}
    bool updateGeneral()  {return _updateGeneral;}
private:
    void createIcons();
    void setupUi();

    
public Q_SLOTS:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private Q_SLOTS:
    void ok();

private:
    QListWidget*        _pContentsWidget;
    QStackedWidget*     _pPagesWidget;
    ConfigGeneral*      _pGeneralPage;
    ConfigRenderer*     _pRendererPage;
    Ptr<NyxSetting>     _pSetting;
    bool                _updateRdr;
    bool                _updateGeneral;
};
//-----------------------------------------------------------------------------
}  // namespace Nyx

 #endif // NYX_CONFIGDIALOG_H