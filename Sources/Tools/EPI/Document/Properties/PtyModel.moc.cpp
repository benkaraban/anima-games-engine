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
#include "PtyModel.moc.h"

#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <QMessageBox>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/LineEdit.moc.h>


#include <EPI/Document/Properties/PtySun.moc.h>

#include <Universe/Tools.h>
#include <EPI/Document/PropertySelection.moc.h>

namespace EPI
{
const int32 ANIM_NO_SELECTION = -1;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyModel::PtyModel( const Ptr<Universe::World>& pWorld,
                    const Ptr<StateRecorder>& pStateRecorder,
                    const Core::Ptr<Universe::Node> & pNodeInstance,
                    const Ptr<ImportModelInfo>& pModelInfo, 
                    const Core::String& title):
    _pRessourcePool(pWorld->getRessourcesPool()),
    PtyNodeEditableChildren(pStateRecorder, pWorld, pNodeInstance, title),
    _indexSelectedAnim(ANIM_NO_SELECTION),
    _pImportModelInfo(pModelInfo),
    //_pNodeInstance(pNodeInstance),
    _animDuration(3)

{
    initPtyFromData();
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyModel::~PtyModel()
{}
//-----------------------------------------------------------------------------
//la fonction ne doit etre appele que lors de la creation de la Pty
void PtyModel::initPtyFromData()
{
    Ptr<Universe::Node> pNodeInst = getUniverseNode();
    _ptiesMaterial.clear();

    Core::List<Ptr<Universe::Node> > children;
    Core::List<Ptr<Universe::Node> > nodeForPtiesMaterials;

    // TODOINST children.push_back(pNodeInst)
    Universe::getAllChildren(children, *pNodeInst);

    Core::List<Universe::ENodeType> nodeTypes;
        nodeTypes.push_back(Universe::NODE_MESH);
        nodeTypes.push_back(Universe::NODE_SKELETON);
        Universe::filterByTypes(nodeForPtiesMaterials, children, nodeTypes);

    nodeForPtiesMaterials.push_back(pNodeInst);

    Core::Map<Core::String, Core::List<Ptr<Universe::NodeMesh> > >  mapMesh;
    Core::Map<Core::String, Core::List<Ptr<Universe::SkeletonSkin> > >  mapSkin;

        for (int32 ii=0; ii<nodeForPtiesMaterials.size(); ++ii)
        {
            if (nodeForPtiesMaterials[ii]->getNodeType() == Universe::NODE_MESH)
            {
                Ptr<Universe::NodeMesh> pNodeM = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(nodeForPtiesMaterials[ii]);
                mapMesh[pNodeM->getMeshName()].push_back(pNodeM);
            }
            else if (nodeForPtiesMaterials[ii]->getNodeType() == Universe::NODE_SKELETON)
            {
                Ptr<Universe::NodeSkeleton> pNodeS = LM_DEBUG_PTR_CAST<Universe::NodeSkeleton>(nodeForPtiesMaterials[ii]);
                bool meshFound = false;
                Core::List<Ptr<Universe::Node> > pNodes;
                for (int32 iSkin=0; iSkin<pNodeS->getSkinCount(); ++iSkin)
                {
                    mapSkin[pNodeS->getSkin(iSkin)->getMeshName()].push_back(pNodeS->getSkin(iSkin));
                }
            }
        }

    typedef Core::Map<Core::String, Core::List<Ptr<Universe::NodeMesh> > >::iterator IteratorMesh;
    IteratorMesh itMesh = mapMesh.begin();
    for (; itMesh!=mapMesh.end(); ++itMesh)
    {
        _ptiesMaterial.push_back(Ptr<PtyMaterial>(new PtyMaterial(_pRessourcePool, itMesh->second, itMesh->first, false)));
        addChild(_ptiesMaterial.back());
    }

    typedef Core::Map<Core::String, Core::List<Ptr<Universe::SkeletonSkin> > >::iterator IteratorSkin;
    IteratorSkin itSkin = mapSkin.begin();
    for (; itSkin!=mapSkin.end(); ++itSkin)
    {
        _ptiesMaterial.push_back(Ptr<PtyMaterial>(new PtyMaterial(_pRessourcePool, itSkin->second, itSkin->first, false)));
        addChild(_ptiesMaterial.back());
    }
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyModel::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetModel> pPW (new PtyWidgetModel(pDataProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyModel::updateData()
{

}
//-----------------------------------------------------------------------------
void PtyModel::updateProperty()
{
    _animations.clear();
    Ptr<Universe::Node> pNodeInst = getUniverseNode();

    if (pNodeInst->getNodeType() == Universe::NODE_SKELETON)
    {
        Ptr<Universe::NodeSkeleton> pNodeS = LM_DEBUG_PTR_CAST<Universe::NodeSkeleton>(pNodeInst);

        for (int32 ii=0; ii < pNodeS->getAnimCount(); ++ii)
        {
            _animations.push_back(pNodeS->getAnimName(ii));
        }

        if(pNodeS->getAnimCount() > 0)
            _indexSelectedAnim = 0;
    }
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyModel::clone() const
{
    return Ptr<Property>(new PtyModel( *this ));
}
//-----------------------------------------------------------------------------
void PtyModel::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNodeEditableChildren::internalCopy(pSrc);

    Ptr<PtyModel> pPty = LM_DEBUG_PTR_CAST<PtyModel>(pSrc);

    _pRessourcePool         = pPty->_pRessourcePool;
    //_selectedPtiesMaterials = pPty->_selectedPtiesMaterials;
    //_pNodeInstance          = pPty->_pNodeInstance;

    _ptiesMaterial = pPty->_ptiesMaterial;

    if(pPty->_indexSelectedAnim < _animations.size())
        _indexSelectedAnim = pPty->_indexSelectedAnim;
    _animDuration = pPty->_animDuration;

    updateData();
}
//-----------------------------------------------------------------------------
void PtyModel::saveModel()
{
    bool savePossible = true;

    for (int32 ii=0; ii<_ptiesMaterial.size(); ++ii)
    {
        String textureName;

        _ptiesMaterial[ii]->getDataDiffuseTexture(textureName);
        if(textureName.length() > 0 && !textureName.startsWith(L"/"))
        {
            savePossible = false;
        }

        _ptiesMaterial[ii]->getDataSpecularTexture(textureName);
        if(textureName.length() > 0 && !textureName.startsWith(L"/"))
        {
            savePossible = false;
        }

        _ptiesMaterial[ii]->getDataNormalTexture(textureName);
        if(textureName.length() > 0 && !textureName.startsWith(L"/"))
        {
            savePossible = false;
        }

        _ptiesMaterial[ii]->getDataLightTexture(textureName);
        if(textureName.length() > 0 && !textureName.startsWith(L"/"))
        {
            savePossible = false;
        }

        _ptiesMaterial[ii]->getDataCustomTexture0(textureName);
        if(textureName.length() > 0 && !textureName.startsWith(L"/"))
        {
            savePossible = false;
        }

        _ptiesMaterial[ii]->getDataCustomTexture1(textureName);
        if(textureName.length() > 0 && !textureName.startsWith(L"/"))
        {
            savePossible = false;
        }

        _ptiesMaterial[ii]->getDataCustomTexture2(textureName);
        if(textureName.length() > 0 && !textureName.startsWith(L"/"))
        {
            savePossible = false;
        }
    }


    if( !savePossible )
    {
        Core::String message = L"You cannot save a model with material textures not in the VFS!";
        QMessageBox::critical(NULL, "Nyx", Core::String8(message).c_str());
    }
    else
    {
        Ptr<Universe::Node> pNodeInst = getUniverseNode();
        if (pNodeInst->getNodeType() == Universe::NODE_SKELETON)
        {
            Ptr<Universe::NodeSkeleton> pNodeS = LM_DEBUG_PTR_CAST<Universe::NodeSkeleton>(pNodeInst);
            Ptr<Assets::Model> pModel = _pRessourcePool->getModel(pNodeS->getModelName());

            for (int32 iMesh=0; iMesh<pModel->getMeshCount(); ++iMesh)
            {
                Assets::ModelMesh & mesh = pModel->getMesh(iMesh);
                Assets::Material & meshMat = mesh.getMaterial();

                for(int32 iPtyMat = 0; iPtyMat<_ptiesMaterial.size(); ++iPtyMat)
                {
                    if (_ptiesMaterial[iPtyMat]->getName() == mesh.getName())
                    {
                        _ptiesMaterial[iPtyMat]->copy(meshMat);
                    }
                }
            }

            emit save(pModel);
        }
    }
}
//-----------------------------------------------------------------------------
void PtyModel::animate(EAnimPlayType type)
{
    Ptr<Universe::Node> pNodeInst = getUniverseNode();
    if(pNodeInst->getNodeType() == Universe::NODE_SKELETON)
    {
        Ptr<Universe::NodeSkeleton> pNodeSkeleton = LM_DEBUG_PTR_CAST<Universe::NodeSkeleton>(pNodeInst);

        if(type == ANIM_STOP)
        {
            pNodeSkeleton->setDefaultPose();
        }
        else if(_indexSelectedAnim < pNodeSkeleton->getAnimCount() && _indexSelectedAnim != ANIM_NO_SELECTION)
        {
            if(type == ANIM_PLAY)
                pNodeSkeleton->animate(_indexSelectedAnim, _animDuration);
            else if(type == ANIM_PLAYLOOP)
                pNodeSkeleton->animate(_indexSelectedAnim, _animDuration, 0.0, 1.0, true);
        }
    }
}
//-----------------------------------------------------------------------------
void PtyModel::renameAnim(const Core::String & newName)
{
    Ptr<Universe::Node> pNodeInst = getUniverseNode();

    if( _indexSelectedAnim != ANIM_NO_SELECTION && _indexSelectedAnim < _animations.size())
    {
        if (pNodeInst->getNodeType() == Universe::NODE_SKELETON)
        {
            Ptr<Universe::NodeSkeleton> pNodeSkeleton = LM_DEBUG_PTR_CAST<Universe::NodeSkeleton>(pNodeInst);
            Ptr<Assets::Model> pModel = _pRessourcePool->getModel(pNodeSkeleton->getModelName());

            pModel->renameAnimation(_indexSelectedAnim, newName);
            Core::String animationName = pModel->getAnimationName(_indexSelectedAnim);
            _animations[_indexSelectedAnim] = animationName;
        }
    }
}
//-----------------------------------------------------------------------------
void PtyModel::deleteAnim()
{
    Ptr<Universe::Node> pNodeInst = getUniverseNode();

    if( _indexSelectedAnim != ANIM_NO_SELECTION && _indexSelectedAnim < _animations.size())
    {
        if (pNodeInst->getNodeType() == Universe::NODE_SKELETON)
        {
            Ptr<Universe::NodeSkeleton> pNodeSkeleton = LM_DEBUG_PTR_CAST<Universe::NodeSkeleton>(pNodeInst);
            pNodeSkeleton->setDefaultPose();

            Ptr<Assets::Model> pModel = _pRessourcePool->getModel(pNodeSkeleton->getModelName());

            pModel->deleteAnimation(_indexSelectedAnim);
            _animations.erase(_animations.begin() + _indexSelectedAnim);

            _indexSelectedAnim = ANIM_NO_SELECTION;
        }
    }
}
void PtyModel::mergeAnim(const Core::String & modelName)
{
    Ptr<Universe::Node> pNodeInst = getUniverseNode();

    if (pNodeInst->getNodeType() == Universe::NODE_SKELETON)
    {
        Ptr<Universe::NodeSkeleton> pNodeSkeleton = LM_DEBUG_PTR_CAST<Universe::NodeSkeleton>(pNodeInst);
        Ptr<Assets::Model> pModel = _pRessourcePool->getModel(pNodeSkeleton->getModelName());

        Ptr<Assets::Model> pModelToMerge = _pRessourcePool->getModel(modelName);

        _animations.clear();
        pModel->mergeAnimations(*pModelToMerge);
        for(int32 iAnim = 0; iAnim < pModel->getAnimationCount(); ++iAnim)
            _animations.push_back(pModel->getAnimationName(iAnim));

        _indexSelectedAnim = ANIM_NO_SELECTION;
    }
}
//-----------------------------------------------------------------------------
void PtyModel::getSelectedPtiesMaterials(Core::List<Ptr<Property> >& selectedPties)
{
    Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection>(getPtySelection());
    selectedPties.clear();
    selectedPties = pPtySelection->getSelectedPties();
/*
    for (int32 ii=0; ii<_selectedPtiesMaterials.size(); ++ii)
    {
        for (int32 iMat=0; iMat<_ptiesMaterial.size(); ++iMat)
        {
            if (_ptiesMaterial[iMat]->getName() == _selectedPtiesMaterials[ii])
            {
                selectedPties.push_back(LM_DEBUG_PTR_CAST<Property>(_ptiesMaterial[iMat]));
            }
        }
    }*/
}
//-----------------------------------------------------------------------------
void PtyModel::setSelectedBoundingBoxVisible(bool flag)
{
    for(int32 ii=0; ii < _ptiesMaterial.size(); ++ii)
    {
        _ptiesMaterial[ii]->setBoundingBoxVisible(false);
    }

    if (flag == true)
    {
        Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection>(getPtySelection());
        Core::List<Ptr<Property> > selected = pPtySelection->getSelectedPties();

        for(int32 ii=0; ii < selected.size(); ++ii)
        {
             Ptr<PtyMaterial> pPtyMat = LM_DEBUG_PTR_CAST<PtyMaterial>(selected[ii]);
             pPtyMat->setBoundingBoxVisible(true);
        }
    }
}
//-----------------------------------------------------------------------------
void PtyModel::editingSelectionChanged()
{
    setSelectedBoundingBoxVisible(true);
    emit propertyChanged();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetModel::PtyWidgetModel( const Ptr<PropertyWidgetDataProxy>& data,
                                QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetModel::~PtyWidgetModel()
{
 //   delete _verticalSpacer2;
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::readProperty()
{
    Ptr<PtyModel> pPtyModel = LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty());

    //_listMeshes
    Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyModel->getPtySelection());
    _listMeshes->clear();
    for(int32 ii=0; ii < pPtyModel->_ptiesMaterial.size(); ++ii)
    {
        QListWidgetItem* item  = new QListWidgetItem (Core::String8(pPtyModel->_ptiesMaterial[ii]->getName()).c_str(), _listMeshes);
        item->setData(Qt::WhatsThisRole, QVariant(pPtyModel->_ptiesMaterial[ii]->getId()));
        item->setSelected(false);
        _listMeshes->addItem(item);


        for (int32 iMat=0; iMat<pPtySelection->getSelectedPtiesCount(); ++iMat)
        {
            if (pPtyModel->_ptiesMaterial[ii]->getName() == pPtySelection->getSelectedPty(iMat)->getName() && pPtyModel->_ptiesMaterial[ii]->getId() == pPtySelection->getSelectedPty(iMat)->getId())
            {
                item->setSelected(true);
                break;
            }
        }
    }

    //_listAnims
    disconnect( _listAnims, SIGNAL(itemChanged(QListWidgetItem  *)), 
                this,       SLOT(renameAnim(QListWidgetItem  *)));
    _listAnims->clear();
    for(int32 ii=0; ii < pPtyModel->_animations.size(); ++ii)
    {
        QListWidgetItem* item  = new QListWidgetItem (Core::String8(pPtyModel->_animations[ii]).c_str(), _listAnims);
        item->setSelected(false);
        item->setFlags(item->flags()|Qt::ItemIsEditable);
        _listAnims->addItem(item);
    }

    if (pPtyModel->_indexSelectedAnim != ANIM_NO_SELECTION)
    {
        _listAnims->item(pPtyModel->_indexSelectedAnim)->setSelected(true);
    }
    connect(_listAnims, SIGNAL(itemChanged(QListWidgetItem  *)), 
            this,       SLOT(renameAnim(QListWidgetItem  *)));

    //_duration
    _duration->setText(String8(Core::toString(pPtyModel->_animDuration, 3)).c_str());

    updatePtyWidgetMaterial();
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::writeProperty(QWidget* pWidget)
{
    Ptr<PtyModel> pPtyModel = LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty());
    Ptr<PropertySelection>  pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyModel->getPtySelection());

    disconnect (pPtySelection.get(), SIGNAL(selectionChanged()),
        pPtyModel.get(), SLOT(editingSelectionChanged()));

    if (pWidget == NULL || pWidget == _listMeshes)
    {
        //pPtyModel->_selectedPtiesMaterials.clear();
        pPtySelection->deselectAll();
        
        for(int32 iItemSelected=0; iItemSelected<_listMeshes->selectedItems().size(); ++iItemSelected)
        {
            Core::String itemName(Core::String8(_listMeshes->selectedItems()[iItemSelected]->text().toStdString().c_str()));

            int32 itemPtyId = _listMeshes->selectedItems()[iItemSelected]->data(Qt::WhatsThisRole).toInt();

            for (int32 ii=0; ii<pPtyModel->_ptiesMaterial.size(); ++ii)
            {
                if (pPtyModel->_ptiesMaterial[ii]->getName() == itemName && pPtyModel->_ptiesMaterial[ii]->getId() == itemPtyId)
                {
                    pPtySelection->addToSelection(pPtyModel->_ptiesMaterial[ii]);
                    break;
                }
            }
            //pPtyModel->_selectedPtiesMaterials.push_back(itemName);
        }
           
    }

    if (pWidget == NULL || pWidget == _listAnims)
    {
        pPtyModel->_indexSelectedAnim = _listAnims->currentRow();

        for(int32 iAnimation = 0; iAnimation < pPtyModel->_animations.size(); ++iAnimation)
        {
            pPtyModel->_animations[iAnimation] = Core::String(_listAnims->item(iAnimation)->text().toStdString().c_str());
        }
    }

    if (pWidget == NULL || pWidget == _duration)
    {
        Core::String duration(Core::String8(_duration->text().toStdString().c_str()));
        pPtyModel->_animDuration = Core::toFloat(duration);
    }

    pPtyModel->setSelectedBoundingBoxVisible(true);
    updatePtyWidgetMaterial();

     connect (pPtySelection.get(), SIGNAL(selectionChanged()),
        pPtyModel.get(), SLOT(editingSelectionChanged()));
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::updatePtyWidgetMaterial()
{
    Ptr<PtyModel> pPtyModel = LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty());
    Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyModel->getPtySelection());

    if (pPtySelection->getSelectedPtiesCount() > 0)
    {
        Core::List<Ptr<Property> > selectedPties;
        pPtyModel->getSelectedPtiesMaterials(selectedPties);
        _pPtyWMaterial->getDataProxy()->setProperties(selectedPties);


        _pPtyWMaterial->updatePropertyWidget();
        _pPtyWMaterial->setShown(true);
    }
    else
    {
        _pPtyWMaterial->getDataProxy()->setProperty(null);
        _pPtyWMaterial->setShown(false);
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupAnims = new QtToolbox::CollapsibleWidget(this, "Animations");
    _listAnims = new QListWidget(this);
    _listAnims->setFixedSize(300, 150);

    _buttonBar = new QWidget(this);
    _buttonLayout = new QHBoxLayout(this);
    _buttonLayout->setSpacing(1);
    
    _buttonLayout->setContentsMargins(0, 0, 0, 0);
    _buttonBar->setFixedHeight(20);
    _buttonBar->setFixedWidth(_listAnims->width());
    _play = new QPushButton(QIcon(":/icons/play.png"), "", this);
    _playLoop = new QPushButton(QIcon(":/icons/playLoop.png"), "", this);
    _stop = new QPushButton(QIcon(":/icons/stop.png"), "", this);
    _buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _del = new QPushButton(QIcon(":/icons/clear.png"), "", this);

    _mergeAnim = new QtToolbox::LineEdit(this, "Merge", false, "Drop model here", 100);
    _mergeAnim->setReadOnly(true);
    _mergeAnim->pushAuthorizedDropMimeData("asset/model");
    QPalette palette = _mergeAnim->palette();
    palette.setColor(QPalette::Text, Qt::red);
    _mergeAnim->setPalette(palette);

    _duration = new QtToolbox::LineEdit(this, "Duration", false, "0.0", 70);
    _duration->setValidator(0.0, 3600.0, 3);

    _buttonLayout->addWidget(_play);
    _buttonLayout->addWidget(_playLoop);
    _buttonLayout->addWidget(_stop);
    _buttonLayout->addItem(_buttonSpacer);
    _buttonLayout->addWidget(_del);

    _buttonBar->setLayout(_buttonLayout);

    _groupAnims->getLayout()->addWidget(_listAnims);
    _groupAnims->getLayout()->addWidget(_mergeAnim);
    _groupAnims->getLayout()->addWidget(_duration);
    _groupAnims->getLayout()->addWidget(_buttonBar);

    _groupMeshes  = new QtToolbox::CollapsibleWidget(this, "Meshes");
    _listMeshes = new QListWidget(this);
    _listMeshes->setFixedSize(300, 200);
    _listMeshes->setSelectionMode(QAbstractItemView::ExtendedSelection);

    _groupMeshes->getLayout()->addWidget(_listMeshes);

    //
    Ptr<PtyModel> pPtyModel = LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPWMat (new PropertyWidgetDataProxy(null, pPtyModel->getName(), getDataProxy()->getStateRecorder()));
    _pPtyWMaterial  =   Ptr<PtyWidgetMaterial>  (new PtyWidgetMaterial(pDataPWMat, this));
    //

    _layout->addWidget(_groupAnims);
    _layout->addWidget(_groupMeshes);
    _layout->addWidget(_pPtyWMaterial.get());
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    _layout->setAlignment(_groupAnims, Qt::AlignTop);
    _layout->setAlignment(_groupMeshes, Qt::AlignTop);
    _layout->setAlignment(_pPtyWMaterial.get(), Qt::AlignTop);

    int32 width = Core::max(_groupAnims->width(), _groupMeshes->width(), _pPtyWMaterial->width());

    connect(_play, SIGNAL(clicked()), this, SLOT(play()));
    connect(_playLoop, SIGNAL(clicked()), this, SLOT(playLoop()));
    connect(_stop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteAnim()));

    connect(_listAnims, SIGNAL(itemChanged(QListWidgetItem  *)), 
            this,       SLOT(renameAnim(QListWidgetItem  *)));

    connect(_mergeAnim, SIGNAL(valueUpdated()), this, SLOT(mergeAnim()));

    getWidgetsForUndoRedo().push_back(_duration);
    getWidgetsForUndoRedo().push_back(_listAnims);
    getWidgetsForUndoRedo().push_back(_listMeshes);

    _pPtyWMaterial->setShown(false);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::play()
{
    LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty())->animate(ANIM_PLAY);
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::playLoop()
{
    LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty())->animate(ANIM_PLAYLOOP);
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::stop()
{
    LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty())->animate(ANIM_STOP);
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::renameAnim(QListWidgetItem  * item)
{
    Ptr<PtyModel> pPtyModel = LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty());
    Core::String newName(item->text().toStdString().c_str());
    pPtyModel->renameAnim(newName);

    updatePropertyWidget();
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::deleteAnim()
{
    Ptr<PtyModel> pPtyModel = LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty());
    pPtyModel->deleteAnim();

    updatePropertyWidget();
}
//-----------------------------------------------------------------------------
void PtyWidgetModel::mergeAnim()
{
    Core::String modelName(_mergeAnim->text().toStdString().c_str());
    if(modelName.startsWith(L"/"))
    {
        _mergeAnim->setText("Drop model here");
        
        Ptr<PtyModel> pPtyModel = LM_DEBUG_PTR_CAST<PtyModel>(getDataProxy()->getProperty());
        pPtyModel->mergeAnim(modelName);

        updatePropertyWidget();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}//namespace EPI