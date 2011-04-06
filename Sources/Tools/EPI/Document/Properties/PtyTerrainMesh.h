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
#ifndef EPI_PROPERTY_TERRAIN_MESH_H_
#define EPI_PROPERTY_TERRAIN_MESH_H_

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

namespace Universe
{
class World;
}

namespace Workflow
{
struct TMGenInput;
}

namespace QtToolbox
{
class CollapsibleWidget;
}//namespace QtToolbox

class QGridLayout;

namespace EPI
{
class PtyWidgetTerrainMesh;
struct ImportTerrainInfo;
class TerrainGenerationOptions;
class GuiDocument;

//-----------------------------------------------------------------------------
class LM_API_EPI PtyTerrainMesh : public PropertyNode
{
    friend class PtyWidgetTerrainMesh;

public:
    PtyTerrainMesh(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorld, const Ptr<ImportTerrainInfo> & pImportTerrainInfo, const Core::String& title = L"Terrain");
    virtual ~PtyTerrainMesh();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    virtual void updateData();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    const Ptr<Property>& getPtySun();

    virtual String getIconFileName() {return L":/icons/ptyTerrainMesh.png";}

private:
    void generateTerrain(const Ptr<Workflow::TMGenInput> & pTerrainOptions);

    Ptr<Universe::World>    _pWorld;
    Ptr<ImportTerrainInfo>  _pImportTerrainInfo;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetTerrainMesh : public PropertyWidget
{
    friend class PtyTerrainMesh;

public:
    PtyWidgetTerrainMesh(const Ptr<PropertyWidgetDataProxy>& data, const Ptr<ImportTerrainInfo> & pImportTerrainInfo, QWidget * parent = 0);
    virtual ~PtyWidgetTerrainMesh();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

protected Q_SLOTS:
    virtual void launchGeneration();

private:
    QGridLayout * _layout;

    QtToolbox::CollapsibleWidget *  _groupBox;
    TerrainGenerationOptions *  _terrainGenerationOptions;
    Ptr<PropertyWidget>         _pPtyWidgetSun;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif/*EPI_PROPERTY_TERRAIN_MESH_H_*/