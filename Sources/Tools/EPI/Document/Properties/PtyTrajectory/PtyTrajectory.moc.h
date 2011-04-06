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
#ifndef EPI_PROPERTY_TRAJECTORY_H_
#define EPI_PROPERTY_TRAJECTORY_H_

#include <QWidget>
#include <QGridLayout>
#include <QTableView>
#include <QItemDelegate>

#include <Core/Math/Splines/Trajectory.h>

#include <EPI/Document/Properties/PtyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>
#include <EPI/Document/ContentInfo/IContentRepresentation.h>
#include <EPI/Document/GuiDescription.h>
#include <EPI/Document/StateRecorder.h>
#include <EPI/Gui/GuiDocument.moc.h>

namespace QtToolbox
{
    class Slider2D;
    class QuickColorPicker;
    class SingleSlidingHDR;
    class SmartGroupBox;
    class ComboBox;
    class CollapsibleWidget;
}//namespace QtToolbox

namespace EPI
{
class GuiDocument;
class CustomLine;


class PtyWidgetTrajectory;
class TableCheckPointView;
class TableCheckPointModel;

//-----------------------------------------------------------------------------
LM_ENUM_2 (ETrajTaskMode,
           TRAJ_MODEL,
           TRAJ_CAMERA);
class LM_API_EPI PtyTrajectory : public PtyNodeEditableContent, public ICallBackTask
{
    Q_OBJECT

    friend class PtyWidgetTrajectory;

public:
    PtyTrajectory(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorldForRepresentation, const Ptr<Universe::World>& pWorld, const Ptr<StateRecorder>& pStateRecorder, const Ptr<Universe::Node>& pNode, const Core::String& title = L"Trajectory");
    virtual ~PtyTrajectory();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateData();
    virtual void updateProperty();
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    virtual bool            isLeaf () const {return true;}
    
    void runTask();
    void stopTask();
    void runStopTask();

    void deleteSelectedCheckPoint();

    const Core::Trajectory& getTrajectory() const;
    Core::Trajectory& getTrajectory();
    void setTrajectory(const Core::Trajectory& tra);

    const Ptr<Universe::NodeCamera>& getCamera() const;
    const Ptr<Universe::Node>& getModel() const;
    void insertCheckPoint(int32 index, const Core::Vector3f& pos, const Core::Quaternionf& orient, float time=-1, bool mustUpdateData = true);
    void createCheckPoint(int32 index = -1);
    void insertLocalisationCamera(int32 index);
    void deleteCheckPoint(int32 start, int32 end);
    void updateModel(const String& newModelName);
    void setEditMode(bool isEditMode);
    bool    isInEditMode() {return _editMode;}
    virtual void signalEndTask(const Ptr<Universe::ITask>& pTask);

    Ptr<StateRecorder>  getStateRecorder() {return _pStateRecorder;}

    void startCameraCtrlMode();
    void endCameraCtrlMode();

    void cameraCtrlStartAction  (int32 checkPointId);
    void cameraCtrlEndAction    ();

    bool isInCameraCtrl() {return _isInCameraCtrl;}

    void moveTrajectory();
    void setNodePivotTrajectory(const Ptr<Universe::Node>& pivot);

    virtual String getIconFileName() {return L":/icons/ptyTrajectory.png";}

    virtual bool beforeUndo();
    virtual bool beforeRedo();
    virtual bool isRepresented() const  {return true;}
    virtual void setRepresentationVisible(bool flag) {_pRepresentation->setVisible(flag);}
private Q_SLOTS:
    virtual void editingSelectionChanged();
    virtual void propertyContentChanged(const Property& pty);
    void ptyTransformChanged();

private:
    void initPtyChildren();
    void showRepresentation(bool flag);
    void cameraCtrlSelectNextCP();
    void cameraCtrlSelectPrevCP();
    void cameraCtrlSelectInsertCP();

private:
    Ptr<Universe::World>         _pWorldForRepresentation;
    Core::Trajectory             _traject;
    Ptr<IContentRepresentation>  _pRepresentation;
    Ptr<StateRecorder>           _pStateRecorder;
    Ptr<Universe::NodeCamera>    _pNodeCamTraj;
    Ptr<Universe::Node>          _pNodeModel;
    GuiDocument&                 _guiDoc;
    Core::String                 _modelName;
    double                        _factor;
    ETrajTaskMode                _mode;
    Ptr<Universe::ITask>         _pTask;
    EViewportCameraView          _viewportCamera;
    bool                         _editMode;
    bool                         _isInCameraCtrl;

    EViewportCameraView          _memoViewportCameraBeforeCamMode;
    int32                        _camModeCheckPointId;

    Ptr<Universe::Node>          _nodePivot;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetTrajectory : public PropertyWidget
{
    Q_OBJECT

    friend class PtyTrajectory;

public:
    PtyWidgetTrajectory(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetTrajectory();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

private:
    void swapCameraCtrlMode(bool checked);


private Q_SLOTS:
    void ptySelectSelectionChanged();
 //   void ptyTrajectoryChanged();
    void viewCPselectionChanged ( const QItemSelection  & selected, const QItemSelection  & deselected ) ;
    void insertLocalisationCamera();
    void swapEditMode(bool checked);
    
    void cameraCtrlMode(bool checked);
    void moveTrajectory();

    void cameraCtrlSelectNextCP();
    void cameraCtrlSelectPrevCP();
    void cameraCtrlSelectInsertCP();

private:
    QGridLayout * _layout;
    QtToolbox::SmartGroupBox *  _groupBox;

    QPushButton*                _editTraj;
    QPushButton*                _moveTraj;

    QtToolbox::CollapsibleWidget*   _cameraModeGroup;
    QPushButton*                    _cameraCtrl;
    QPushButton*                    _cameraCtrlNextCP;
    QPushButton*                    _cameraCtrlPrevCP;
    QPushButton*                    _cameraCtrlInsertCP;
    QPushButton*                    _insertLocalisationCamera;
    TableCheckPointModel*           _modelCP;
    TableCheckPointView*            _viewCP;

    QtToolbox::SingleSlidingHDR *    _factor;

    CustomLine *                _modelName;
    QtToolbox::ComboBox*        _taskMode;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif /*EPI_PROPERTY_TRAJECTORY_H_*/
