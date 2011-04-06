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
#include "PtyDocMaterial.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>

#include <EPI/Document/Properties/PtyMaterial.moc.h>

#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>

namespace EPI
{
const String SPHERE_MESH_NAME = L"$MoveToolDocMaterialSphereMesh";
const String SPHERE_MODEL_NAME = L"$MoveToolDocMaterialSphere";

const String PLAN_MESH_NAME = L"$MoveToolDocMaterialPlanMesh";
const String PLAN_MODEL_NAME = L"$MoveToolDocMaterialPlan";

const float widthPlan = 20.f;
void createModels(const Ptr<Universe::RessourcePool>& pPool)
{
    //sphere
    const float radiusSphere = 15.f;

    Ptr<Assets::ModelMesh> sphere (new Assets::ModelMesh());
    Assets::VerticeSet & vsetSphere = sphere->getLODByID(0);
    Assets::createGeoSphere(vsetSphere, radiusSphere, 5, true);
    //Assets::makeTBNBasis(vsetSphere, false);
    sphere->cleanup();

    Ptr<Assets::Model> pSphereModel (new Assets::Model());
    pSphereModel->addMesh(SPHERE_MESH_NAME, *sphere);

    pPool->addModel(SPHERE_MODEL_NAME, pSphereModel);

    //plan
    
    const float heightPlan = widthPlan;
    Ptr<Assets::ModelMesh> plan (new Assets::ModelMesh());
    Assets::VerticeSet & vsetPlan = plan->getLODByID(0);
    Assets::createPlane(vsetPlan, widthPlan, heightPlan, 10, 10);
    //Assets::makeTBNBasis(vsetPlan, false);
    plan->cleanup();

    Ptr<Assets::Model> pPlanModel (new Assets::Model());
    pPlanModel->addMesh(PLAN_MESH_NAME, *plan);

    pPool->addModel(PLAN_MODEL_NAME, pPlanModel);
}

String EDocMaterialSupportToString(EDocMaterialSupport type)
{
    String ret;

    switch(type)
    {
    case DOC_MAT_SUPPORT_PLAN:
        ret = L"Plan";
        break;
    case DOC_MAT_SUPPORT_SPHERE:
        ret = L"Sphere";
        break;
    }

    return ret;
}

EDocMaterialSupport StringToEDocMaterialSupport(const String& str)
{
    EDocMaterialSupport ret;

    if      (str==L"Plan")   ret = DOC_MAT_SUPPORT_PLAN;
    else if (str==L"Sphere") ret = DOC_MAT_SUPPORT_SPHERE;
    else                     LM_ASSERT(false);

    return ret;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyDocMaterial::PtyDocMaterial(const Ptr<Universe::World>& pWorld, const Core::String& title):
    _pWorld(pWorld),
    PropertyNode(title, true, false, CONTENT),
    _support(DOC_MAT_SUPPORT_PLAN)
{
    createModels(_pWorld->getRessourcesPool());

    if      (_support==DOC_MAT_SUPPORT_PLAN)    _pNodeM = _pWorld->createMesh(PLAN_MODEL_NAME, PLAN_MESH_NAME);
    else if (_support==DOC_MAT_SUPPORT_SPHERE)  _pNodeM = _pWorld->createMesh(SPHERE_MODEL_NAME, SPHERE_MESH_NAME);

    Core::List<Ptr<Universe::NodeMesh> >    pNodes;
    pNodes.push_back(_pNodeM);
    Ptr<PtyMaterial>    pPtyMat (new PtyMaterial(_pWorld->getRessourcesPool(), pNodes, L"Material", false));

    addChild(pPtyMat);

    if (_support==DOC_MAT_SUPPORT_PLAN)         _pNodeM->setMesh (PLAN_MODEL_NAME, PLAN_MESH_NAME);
    else if (_support==DOC_MAT_SUPPORT_SPHERE)  _pNodeM->setMesh (SPHERE_MODEL_NAME, SPHERE_MESH_NAME);

    pPtyMat->setNodeMesh(_pNodeM, true);

     
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyDocMaterial::~PtyDocMaterial()
{
    if (_pNodeM!=null)
    {
        _pNodeM->kill();
    }
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyDocMaterial::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetDocMaterial> pPW (new PtyWidgetDocMaterial(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyDocMaterial::updateData()
{
    if (_support==DOC_MAT_SUPPORT_PLAN)         _pNodeM->setMesh (PLAN_MODEL_NAME, PLAN_MESH_NAME);
    else if (_support==DOC_MAT_SUPPORT_SPHERE)  _pNodeM->setMesh (SPHERE_MODEL_NAME, SPHERE_MESH_NAME);

    Ptr<PtyMaterial> pPtyMat = LM_DEBUG_PTR_CAST<PtyMaterial>(getPtyMaterial());
    pPtyMat->setNodeMesh(_pNodeM, true);

    if (_support==DOC_MAT_SUPPORT_PLAN)
    {
        _pNodeM->beginMatrixUpdate();
            _pNodeM->setWorldPosition(Core::Vector3f(-widthPlan/2.f,0,-widthPlan/2.f));
        _pNodeM->endMatrixUpdate();
    }
    else
    {
        _pNodeM->beginMatrixUpdate();
            _pNodeM->setWorldPosition(Core::Vector3f(0,0,0));
        _pNodeM->endMatrixUpdate();
    }
}
//-----------------------------------------------------------------------------
void PtyDocMaterial::updateProperty()
{
     
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyDocMaterial::clone() const
{
    return Ptr<Property>(new PtyDocMaterial( *this ));
}
//-----------------------------------------------------------------------------
void PtyDocMaterial::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyDocMaterial> pPty = LM_DEBUG_PTR_CAST<PtyDocMaterial>(pSrc);

    _support = pPty->_support;

    updateData();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyDocMaterial::getPtyMaterial()
{
    return getChild(0);
}
//-----------------------------------------------------------------------------
const Core::String& PtyDocMaterial::getWorldDecoPath()
{
    return _worldDeco;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetDocMaterial::PtyWidgetDocMaterial(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetDocMaterial::~PtyWidgetDocMaterial()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetDocMaterial::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::SmartGroupBox(this, "Material", true);

    _support = new QtToolbox::ComboBox(this, "Support");
        for (int32 ii=0; ii<EDocMaterialSupport_COUNT; ++ii)
        {
            _support->addItem(String8(EDocMaterialSupportToString((EDocMaterialSupport)ii)).c_str());
        }
    
    Ptr<PtyDocMaterial> pP = LM_DEBUG_PTR_CAST<PtyDocMaterial>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPWMat (new PropertyWidgetDataProxy(pP->getPtyMaterial(), pP->getName(), getDataProxy()->getStateRecorder()));
    _pPtyWidgetMat = pP->getPtyMaterial()->createPropertyWidget(pDataPWMat, this);
    


    _worldDeco = new QtToolbox::LineEdit(this, "Drop world here");
    _worldDeco->pushAuthorizedDropMimeData("asset/world");
    _worldDeco->setReadOnly(true);

    
    _groupBox->addWidget(_support);
    _groupBox->addWidget(_worldDeco);

    _layout->addWidget(_groupBox, 0, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);
    _layout->addWidget(_pPtyWidgetMat.get(), 1, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);

    setFixedWidth(Core::max(_groupBox->width(),_pPtyWidgetMat->width()));
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_support);
    getWidgetsForUndoRedo().push_back(_worldDeco);  

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocMaterial::readProperty()
{
    Ptr<PtyDocMaterial> pPtyDocMaterial = LM_DEBUG_PTR_CAST<PtyDocMaterial>(getDataProxy()->getProperty());

    _support->selectItem(String8(EDocMaterialSupportToString(pPtyDocMaterial->_support)).c_str());
    _worldDeco->setText(Core::String8(pPtyDocMaterial->_worldDeco).c_str());
    _pPtyWidgetMat->updatePropertyWidget();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocMaterial::writeProperty(QWidget* pWidget)
{
    Ptr<PtyDocMaterial> pPtyDocMaterial = LM_DEBUG_PTR_CAST<PtyDocMaterial>(getDataProxy()->getProperty());

    if (pWidget==NULL || pWidget==_support)
        pPtyDocMaterial->_support = StringToEDocMaterialSupport(_support->selectedText());

    if (pWidget==NULL || pWidget==_worldDeco)
        pPtyDocMaterial->_worldDeco = Core::String(_worldDeco->text().toStdString().c_str());
}
//-----------------------------------------------------------------------------
} // namespace EPI