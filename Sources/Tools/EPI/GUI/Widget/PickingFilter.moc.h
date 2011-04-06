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
#ifndef EPI_PICKING_FILTER_H_
#define EPI_PICKING_FILTER_H_

#include <QWidget>
#include <QDialog>
#include <Core/Standard.h>
#include <EPI/Gui/GuiContext.moc.h>

class QPushButton;
class QHBoxLayout;
class QCheckBox;
class QDialogButtonBox;

namespace EPI
{
class GuiDocument;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class filterParamDialog : public QDialog
{
    Q_OBJECT

public:
    filterParamDialog(QWidget*  parent, const pickingFilterParam& param) : QDialog(parent) {setupUi(param);}
    ~filterParamDialog() {}
    const pickingFilterParam& getParam() const {return _param;}

protected:
    void setupUi(const pickingFilterParam& param);
    

protected Q_SLOTS:
    void accepted();
    void rejected();

private:
    pickingFilterParam      _param;
    QVBoxLayout *           _layout;
    QCheckBox *             _waterPatch;
    QCheckBox *             _camera;
    QCheckBox *             _playerPosition;
    QCheckBox *             _sound;
    QCheckBox *             _instance;
    QCheckBox *             _decal;
    QCheckBox *             _pointLight;
    QCheckBox *             _skeleton;
    QCheckBox *             _trail;
    QCheckBox *             _emitter;
    QCheckBox *             _terrain;

    QDialogButtonBox *      _buttonBox;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PickingFilterWidget : public QWidget
{
    Q_OBJECT

public:
    PickingFilterWidget(const Ptr<GuiDocument>& pGDoc, QWidget* parent);
    virtual ~PickingFilterWidget();

protected:
    void setupUi();
    
protected Q_SLOTS:
    void configure();
    void activeFilter(bool);

private:
    QHBoxLayout *           _layout;
    QPushButton *           _activeFilter;
    QPushButton *           _configureFilter;
    Ptr<GuiDocument>        _pGDoc;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace EPI

#endif EPI_PICKING_FILTER_H_