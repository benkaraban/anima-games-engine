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
#include "ConfigDialog.moc.h"

#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>

#include <EPI/Constants.h>

namespace Nyx
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ConfigGeneral::ConfigGeneral(const Ptr<ISetting>& pSet, QWidget* parent):
    QWidget(parent)
{
    setupUi();

    initialize(pSet);
}
//-----------------------------------------------------------------------------
void ConfigGeneral::setupUi()
{
    pShowStartupDialog          = new QCheckBox("Show select working directory dialog at startup", this);
    pShowConsol                 = new QCheckBox("Show consol", this);
    pPlaySound                  = new QCheckBox("Play Sound", this);
    pMakeSleepBetweenTwoUpdate  = new QCheckBox("Sleep between two update", this);
    pWorldDefault               = new QtToolbox::LineEdit(this, "Drop world");
        pWorldDefault->pushAuthorizedDropMimeData("asset/world");
        pWorldDefault->setReadOnly(true);
        pWorldDefault->setFixedHeight(40);
    pDefaultTagGameplay         = new QtToolbox::LineEdit(this, "Gameplay Tags");
        pDefaultTagGameplay->setFixedHeight(40);

    QGridLayout* mainLayout = new QGridLayout(this);

    mainLayout->addWidget(pShowStartupDialog,                   1, 0, 10, 1, Qt::AlignTop);
    mainLayout->addWidget(pShowConsol,                          2, 0, 10, 1, Qt::AlignTop);
    mainLayout->addWidget(pPlaySound,                           3, 0, 10, 1, Qt::AlignTop);
    mainLayout->addWidget(pMakeSleepBetweenTwoUpdate,           4, 0, 10, 1, Qt::AlignTop);
    mainLayout->addWidget(pWorldDefault,                        5, 0, 10, 1, Qt::AlignTop);
    mainLayout->addWidget(pDefaultTagGameplay,                  7, 0, 10, 1, Qt::AlignTop);
    
    
    
    setLayout(mainLayout);
}
//-----------------------------------------------------------------------------
void ConfigGeneral::initialize(const Ptr<ISetting>& pSet)
{
    Ptr<GeneralSetting> pGS = LM_DEBUG_PTR_CAST<GeneralSetting>(pSet);

    pShowStartupDialog->setChecked(pGS->showWorkingDirectoryDialog);
    pShowConsol->setChecked(pGS->showConsol);
    pPlaySound->setChecked(pGS->playSound);
    pMakeSleepBetweenTwoUpdate->setChecked(pGS->sleepBetweenTwoUpdateWorld);
    pWorldDefault->setText(StrToQStr(pGS->paths.defaultWorld));
    pDefaultTagGameplay->setText(StrToQStr(pGS->paths.defaultTagGameplay));
}
//-----------------------------------------------------------------------------
void ConfigGeneral::writeConfig(const Ptr<ISetting>& pSet)
{
    Ptr<GeneralSetting> pGS = LM_DEBUG_PTR_CAST<GeneralSetting>(pSet);

    pGS->showWorkingDirectoryDialog = pShowStartupDialog->isChecked();
    pGS->showConsol                 = pShowConsol->isChecked();
    pGS->playSound                  = pPlaySound->isChecked();

    pGS->sleepBetweenTwoUpdateWorld = pMakeSleepBetweenTwoUpdate->isChecked();
    pGS->paths.defaultWorld         = QStrToStr(pWorldDefault->text());
    pGS->paths.defaultTagGameplay   = QStrToStr(pDefaultTagGameplay->text());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ConfigRenderer::ConfigRenderer(const Ptr<ISetting>& pSet, QWidget* parent):
    QWidget(parent)
{
    setupUi();
    initialize(pSet);
}
//-----------------------------------------------------------------------------

    
void ConfigRenderer::setupUi()
{
    QGridLayout* mainLayout = new QGridLayout(this);

    _pGlobalLevel   = new QtToolbox::ComboBox(this, "Configure to");
    _pShaderLevel   = new QtToolbox::ComboBox(this, "Shader Level");
    _pShadowLevel   = new QtToolbox::ComboBox(this, "Shadow Level");
    _pTextureLevel  = new QtToolbox::ComboBox(this, "Texture Level");
    _pReflecLevel   = new QtToolbox::ComboBox(this, "Reflec Level");
    _pRefracLevel   = new QtToolbox::ComboBox(this, "Refrac Level");
    _pFilterLevel   = new QtToolbox::ComboBox(this, "Filter Level");
    _pMsaaLevel     = new QtToolbox::ComboBox(this, "Msaa Level");

    QPushButton* pGlobalButton = new QPushButton(tr("Initialize"));
    connect(pGlobalButton, SIGNAL(clicked()), this, SLOT(initToGlobal()));

    ADD_ITEM_COMBOBOX_LM_ENUM (_pGlobalLevel,   Renderer::EGlobalLevel, 7)
    ADD_ITEM_COMBOBOX_LM_ENUM (_pShaderLevel,   Renderer::EShaderLevel, 7)
    ADD_ITEM_COMBOBOX_LM_ENUM (_pShadowLevel,   Renderer::EShadowLevel, 7)
    ADD_ITEM_COMBOBOX_LM_ENUM (_pTextureLevel,  Renderer::ETextureLevel, 8)
    ADD_ITEM_COMBOBOX_LM_ENUM (_pReflecLevel,   Renderer::EReflecLevel, 7)
    ADD_ITEM_COMBOBOX_LM_ENUM (_pRefracLevel,   Renderer::ERefracLevel, 7)
    ADD_ITEM_COMBOBOX_LM_ENUM (_pFilterLevel,   Renderer::EFilterLevel, 7)
    ADD_ITEM_COMBOBOX_LM_ENUM (_pMsaaLevel,     Renderer::EMSAALevel, 5)

    mainLayout->addWidget(_pGlobalLevel,    0, 0, 1, 5, Qt::AlignLeft);
    mainLayout->addWidget(pGlobalButton,    0, 1, 1, 1, Qt::AlignLeft);
    mainLayout->addWidget(_pShaderLevel,    2, 0, Qt::AlignLeft);
    mainLayout->addWidget(_pShadowLevel,    3, 0, Qt::AlignLeft);
    mainLayout->addWidget(_pTextureLevel,   4, 0, Qt::AlignLeft);
    mainLayout->addWidget(_pReflecLevel,    5, 0, Qt::AlignLeft);
    mainLayout->addWidget(_pRefracLevel,    6, 0, Qt::AlignLeft);
    mainLayout->addWidget(_pFilterLevel,    7, 0, Qt::AlignLeft);
    mainLayout->addWidget(_pMsaaLevel,      8, 0, Qt::AlignLeft);

    setLayout(mainLayout);
}
//-----------------------------------------------------------------------------
void ConfigRenderer::initialize(const Ptr<ISetting>& pSet)
{
    Ptr<RendererSetting> pRS = LM_DEBUG_PTR_CAST<RendererSetting>(pSet);
        _pShaderLevel->selectIndex((int32)pRS->shaderLevel);
        _pShadowLevel->selectIndex((int32)pRS->shadowLevel);
        _pTextureLevel->selectIndex((int32)pRS->textureLevel);
        _pReflecLevel->selectIndex((int32)pRS->reflecLevel);
        _pRefracLevel->selectIndex((int32)pRS->refracLevel);
        _pFilterLevel->selectIndex((int32)pRS->filterLevel);
        _pMsaaLevel->selectIndex((int32)pRS->msaaLevel);

        int a = (int32)pRS->shaderLevel;
}
//-----------------------------------------------------------------------------
void ConfigRenderer::writeConfig(const Ptr<ISetting>& pSet)
{
    Ptr<RendererSetting> pRS = LM_DEBUG_PTR_CAST<RendererSetting>(pSet);
        pRS->shaderLevel  = (Renderer::EShaderLevel)    _pShaderLevel->selectedIndex();
        pRS->shadowLevel  = (Renderer::EShadowLevel)    _pShadowLevel->selectedIndex();
        pRS->textureLevel = (Renderer::ETextureLevel)   _pTextureLevel->selectedIndex();
        pRS->reflecLevel  = (Renderer::EReflecLevel)    _pReflecLevel->selectedIndex();
        pRS->refracLevel  = (Renderer::ERefracLevel)    _pRefracLevel->selectedIndex();
        pRS->filterLevel  = (Renderer::EFilterLevel)    _pFilterLevel->selectedIndex();
        pRS->msaaLevel    = (Renderer::EMSAALevel)      _pMsaaLevel->selectedIndex();
}
//-----------------------------------------------------------------------------
void ConfigRenderer::initToGlobal()
{
    Renderer::EGlobalLevel level = (Renderer::EGlobalLevel) _pGlobalLevel->selectedIndex();

    Renderer::RendererSettings globalSet(level);
        _pShaderLevel->selectIndex((int32)globalSet.shaderLevel);
        _pShadowLevel->selectIndex((int32)globalSet.shadowLevel);
        _pTextureLevel->selectIndex((int32)globalSet.textureLevel);
        _pReflecLevel->selectIndex((int32)globalSet.reflecLevel);
        _pRefracLevel->selectIndex((int32)globalSet.refracLevel);
        _pFilterLevel->selectIndex((int32)globalSet.filterLevel);
        _pMsaaLevel->selectIndex((int32)globalSet.msaaLevel);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ConfigDialog::ConfigDialog(const Ptr<NyxSetting>& pSetting, QWidget* parent) :
    QDialog(parent),
    _pSetting(pSetting),
    _updateRdr(false),
    _updateGeneral(false)
{
    setupUi();
}
//-----------------------------------------------------------------------------
void ConfigDialog::setupUi()
{
    _pContentsWidget = new QListWidget;
        _pContentsWidget->setViewMode(QListView::IconMode);
        _pContentsWidget->setIconSize(QSize(30, 30));
        _pContentsWidget->setMovement(QListView::Static);
        _pContentsWidget->setMaximumHeight(80);
        _pContentsWidget->setSpacing(12);
        //_pContentsWidget->setFlow(QListView::LeftToRight);

    _pGeneralPage  = new ConfigGeneral(_pSetting->getSetting(GENERAL_SETTING));
    _pRendererPage = new ConfigRenderer(_pSetting->getSetting(RENDERER_SETTING));

    _pPagesWidget = new QStackedWidget;
        _pPagesWidget->addWidget(_pGeneralPage);
        _pPagesWidget->addWidget(_pRendererPage);

    QPushButton* pOkButton = new QPushButton(tr("Ok"));
    QPushButton* pCancelButton = new QPushButton(tr("Cancel"));

    _pContentsWidget->setCurrentRow(0);

    QVBoxLayout * contentPageslayout = new QVBoxLayout;
        contentPageslayout->addWidget(_pContentsWidget);
        contentPageslayout->addWidget(_pPagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
        buttonsLayout->addStretch(1);
        buttonsLayout->addWidget(pOkButton);
        buttonsLayout->addWidget(pCancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(contentPageslayout);
        mainLayout->addStretch(1);
        mainLayout->addSpacing(12);
        mainLayout->addLayout(buttonsLayout);
        setLayout(mainLayout);

    connect(pOkButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(pCancelButton, SIGNAL(clicked()), this, SLOT(close()));
   
    setWindowTitle(tr("Config Dialog"));
    createIcons();
    setFixedSize(800, 500);
}
//-----------------------------------------------------------------------------
void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
         current = previous;

    int32 o = _pContentsWidget->row(current);

     _pPagesWidget->setCurrentIndex(_pContentsWidget->row(current));
}
//-----------------------------------------------------------------------------
void ConfigDialog::createIcons()
{
     QListWidgetItem *generalButton = new QListWidgetItem(_pContentsWidget);
         generalButton->setIcon(QIcon(":/icons/general.png"));
         generalButton->setText(tr("General"));
         generalButton->setTextAlignment(Qt::AlignHCenter);
         generalButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

     QListWidgetItem *rendererButton = new QListWidgetItem(_pContentsWidget);
         rendererButton->setIcon(QIcon(":/icons/renderer.png"));
         rendererButton->setText(tr("Renderer"));
         rendererButton->setTextAlignment(Qt::AlignHCenter);
         rendererButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

     connect(_pContentsWidget,
             SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
             this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}
//-----------------------------------------------------------------------------
void ConfigDialog::ok()
{
    _updateRdr = true;
    _updateGeneral = true;
    _pGeneralPage->writeConfig(_pSetting->getSetting(GENERAL_SETTING));
    _pRendererPage->writeConfig(_pSetting->getSetting(RENDERER_SETTING));

    _pSetting->writeSetting();

    accept();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace Nyx
