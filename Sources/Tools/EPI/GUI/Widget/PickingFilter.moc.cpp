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
#include "PickingFilter.moc.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>

#include <EPI/Gui/GuiDocument.moc.h>

#include <EPI/Document/DocumentBase.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void filterParamDialog::setupUi(const pickingFilterParam& param)
{
    _param = param;
    _layout = new QVBoxLayout(this);

    _waterPatch      = new QCheckBox("water patch", this);
    _camera          = new QCheckBox("camera", this);
    _playerPosition  = new QCheckBox("player position", this);
    _sound           = new QCheckBox("sound", this);
    _instance        = new QCheckBox("instance", this);
    _decal           = new QCheckBox("decal", this);
    _pointLight      = new QCheckBox("point light", this);
    _skeleton        = new QCheckBox("skeleton", this);
    _trail           = new QCheckBox("trail", this);
    _emitter         = new QCheckBox("emitter", this);
    _terrain         = new QCheckBox("terrain", this);

    _buttonBox = new QDialogButtonBox(this);
        _buttonBox->setOrientation(Qt::Horizontal);
        _buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    _layout->addWidget(_instance);
    _layout->addWidget(_skeleton);
    _layout->addWidget(_trail);
    _layout->addWidget(_emitter);
    _layout->addWidget(_decal);
    _layout->addWidget(_sound);
    _layout->addWidget(_camera);
    _layout->addWidget(_pointLight);
    _layout->addWidget(_waterPatch);
    _layout->addWidget(_terrain);
    _layout->addWidget(_playerPosition);
    
    
    _skeleton->hide();
    

    _layout->addWidget(_buttonBox);

    _waterPatch->setChecked(param.waterPatch);
    _camera->setChecked(param.camera);
    _playerPosition->setChecked(param.playerPosition);
    _sound->setChecked(param.sound);
    _instance->setChecked(param.instance);
    _decal->setChecked(param.decal);
    _pointLight->setChecked(param.pointLight);
    _skeleton->setChecked(param.skeleton);
    _trail->setChecked(param.trail);
    _emitter->setChecked(param.emitter);
    _terrain->setChecked(param.terrain);

    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(rejected()));
    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));
}
//-----------------------------------------------------------------------------
void filterParamDialog::accepted()
{
    _param.waterPatch        = _waterPatch->isChecked();
    _param.camera            = _camera->isChecked();
    _param.playerPosition    = _playerPosition->isChecked();
    _param.sound             = _sound->isChecked();
    _param.instance          = _instance->isChecked();
    _param.decal             = _decal->isChecked();
    _param.pointLight        = _pointLight->isChecked();
    _param.skeleton          = _skeleton->isChecked();
    _param.trail             = _trail->isChecked();
    _param.emitter           = _emitter->isChecked();
    _param.terrain           = _terrain->isChecked();

    accept();
}
//-----------------------------------------------------------------------------
void filterParamDialog::rejected()
{
    reject();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PickingFilterWidget::PickingFilterWidget(const Ptr<GuiDocument>& pGDoc, QWidget* parent):
    QWidget(parent),
    _pGDoc(pGDoc)
{
    setupUi();
}
//-----------------------------------------------------------------------------
    PickingFilterWidget::~PickingFilterWidget()
{
}
//-----------------------------------------------------------------------------
void PickingFilterWidget::setupUi()
{
    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(0,0,0,0);
    _layout->setSpacing(3);

    _activeFilter    = new QPushButton(QIcon(":/icons/pickingFilter.png"), "", this);
    _configureFilter = new QPushButton(QIcon(":/icons/pickingFilterParam.png"), "", this);

    int32 val = Core::min(width(),  height());
    _activeFilter->setIconSize(QSize(val-6, val-6));
    _configureFilter->setIconSize(QSize(val-6, val-6));
    _activeFilter->setFlat(true);
    _configureFilter->setFlat(true);
    _activeFilter->setCheckable(true);

    _layout->addWidget(_activeFilter);
    _layout->addWidget(_configureFilter);
    setLayout(_layout);

    connect(_activeFilter, SIGNAL(toggled(bool)), this, SLOT(activeFilter(bool)));
    connect(_configureFilter, SIGNAL(released ()), this, SLOT(configure()));

    _activeFilter->setChecked(_pGDoc->getGuiContext()->pickingFilterActivated());
}  
//-----------------------------------------------------------------------------
void PickingFilterWidget::configure()
{
    filterParamDialog dlg (this, _pGDoc->getGuiContext()->getPickingFilterParam());
    if (dlg.exec() == QDialog::Accepted)
    {
        _pGDoc->getGuiContext()->setPickingFilterParam(dlg.getParam());
    }
}
//-----------------------------------------------------------------------------
void PickingFilterWidget::activeFilter(bool flag)
{
    _pGDoc->getGuiContext()->setPickingFilter(flag);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace EPI

