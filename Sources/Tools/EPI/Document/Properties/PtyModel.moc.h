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
#ifndef EPI_PROPERTY_MODEL_H_
#define EPI_PROPERTY_MODEL_H_

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <EPI/Document/Properties/PtyNode.moc.h>
#include <EPI/Document/Properties/PtyMaterial.moc.h>
#include <EPI/ImportInfo/ImportModelInfo.h>

namespace QtToolbox
{
class SmartGroupBox;
class LineEdit;
}//namespace QtToolbox

class QGridLayout;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QHBoxLayout;
class QSpacerItem;

namespace EPI
{

LM_ENUM_4(  EAnimPlayType, 
            ANIM_PLAY, 
            ANIM_PAUSE, 
            ANIM_PLAYLOOP, 
            ANIM_STOP)


//-----------------------------------------------------------------------------
class LM_API_EPI PtyModel : public PtyNodeEditableChildren
{
    Q_OBJECT

    friend class PtyWidgetModel;

public:
    PtyModel(   const Ptr<Universe::World>& pWorld,
                const Ptr<StateRecorder>& pStateRecorder,
                const Core::Ptr<Universe::Node> & pNodeInstance, 
                const Ptr<ImportModelInfo>& pModelInfo, 
                const Core::String& title = L"Model");

    virtual ~PtyModel();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual void updateData();
    void updateProperty();

    void getSelectedPtiesMaterials(Core::List<Ptr<Property> >& selectedPties);

    void saveModel();

    void setSelectedBoundingBoxVisible(bool flag);

    virtual String getIconFileName() {return L":/icons/ptyModel.png";}

Q_SIGNALS:
    void save(const Ptr<Assets::Model>& pModel);

private Q_SLOTS:
    virtual void editingSelectionChanged();

private:
    void animate(EAnimPlayType type);
    void renameAnim(const Core::String & newName);
    void deleteAnim();
    void mergeAnim(const Core::String & modelName);

private:
    void initPtyFromData(); //appeler par le constructeur

private:
    Ptr<Universe::RessourcePool>    _pRessourcePool;
    //Core::List<String>              _selectedPtiesMaterials;
    Core::List<String>              _animations;
    float                           _animDuration;
    int32                           _indexSelectedAnim;
    Core::List<Ptr<PtyMaterial> >   _ptiesMaterial;
    Ptr<ImportModelInfo>            _pImportModelInfo;
    //Ptr<Universe::NodeInstance>     _pNodeInstance;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetModel : public PropertyWidget
{
    Q_OBJECT
    friend class PtyModel;
    

public:
    PtyWidgetModel( const Ptr<PropertyWidgetDataProxy>& data, 
                    QWidget * parent = 0);
    virtual ~PtyWidgetModel();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private Q_SLOTS:
    void play();
    void playLoop();
    void stop();

    void renameAnim(QListWidgetItem  * item);
    void deleteAnim();
    void mergeAnim();

private:
    void updatePtyWidgetMaterial();

private:
    QGridLayout * _layout;
    //Anims
    QtToolbox::CollapsibleWidget *  _groupAnims;
    QListWidget *                   _listAnims;
    QWidget *                       _buttonBar;
    QHBoxLayout *                   _buttonLayout;
    QPushButton *                   _play;
    QPushButton *                   _playLoop;
    QPushButton *                   _stop;
    QSpacerItem *                   _buttonSpacer;
    QPushButton *                   _del;
    QtToolbox::LineEdit *           _mergeAnim;
    QtToolbox::LineEdit *           _duration;

    
    //Meshs
    QtToolbox::CollapsibleWidget *  _groupMeshes;
    QListWidget * _listMeshes;

    Ptr<PtyWidgetMaterial>          _pPtyWMaterial;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //EPI_PROPERTY_MODEL_H_