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
#include "EpiQtToolbox.h"

#include <QCheckBox>
#include <QRadioButton>
#include <QListWidget>

#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>
#include <QtToolbox/SpinBox.moc.h>
#include <QtToolbox/SpinBoxVector.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <QtToolbox/Slider2D/Slider2D.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>
#include <QtToolbox/NumberEdit.moc.h>
#include <QtToolbox/NumberEditVector3.moc.h>

#include <EPI/IWidgetManageUndoRedo.moc.h>
#include <EPI/GUI/Widget/CustomLine.moc.h>

#include <algorithm>

namespace EPI
{
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QPushButton* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(clicked(bool)),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(clicked(bool)),
            mapperCommitPropertyChange, SLOT (map()));

        mapperCommitPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::LineEdit* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

        mapperCommitPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QListWidget* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(itemSelectionChanged()),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(itemSelectionChanged()),
            mapperCommitPropertyChange, SLOT (map()));

        mapperCommitPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::ComboBox* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

        mapperCommitPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QCheckBox* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(stateChanged(int)),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(stateChanged(int)),
            mapperCommitPropertyChange, SLOT (map()));

        mapperCommitPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QRadioButton* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(clicked(bool)),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(clicked(bool)),
            mapperCommitPropertyChange, SLOT (map()));

        mapperCommitPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    CustomLine* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

        mapperCommitPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::SingleSlidingDouble * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalSliderHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalSliderReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));
    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalSliderHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalSliderReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }

}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::NumberEditVector3 * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalWidgetUpdated()),
            mapperCommitPropertyChange, SLOT (map()));
    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::NumberEdit * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalWidgetUpdated()),
            mapperCommitPropertyChange, SLOT (map()));
    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::SingleSlidingHDR * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalSliderHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalSliderReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));
    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalSliderHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalSliderReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::SingleSlidingInt * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalSliderHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalSliderReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));
    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalSliderHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalSliderReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(valueUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::RangedSlidingDouble * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalRangeHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalRangeReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(rangeUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalRangeHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalRangeReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(rangeUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::RangedSlidingHDR * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalRangeHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalRangeReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(rangeUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalRangeHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalRangeReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(rangeUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::RangedSlidingInt * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalRangeHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalRangeReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(rangeUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalRangeHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalRangeReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(rangeUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::SpinBoxVector3* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));

        QObject::connect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalWidgetUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::SpinBox* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));

        QObject::connect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalWidgetUpdated()),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetUpdated()),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::QuickColorPicker* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(colorUpdated(float, float, float, float)),
            mapperCommitPropertyChange, SLOT (map()));

    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(colorUpdated(float, float, float, float)),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                    QtToolbox::Slider2D* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        mapperBeginPropertyChange->setMapping (pWidget, pWidget);
        mapperFinishPropertyChange->setMapping (pWidget, pWidget);
        mapperCommitPropertyChange->setMapping (pWidget, pWidget);

        QObject::connect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::connect(pWidget, SIGNAL(valueUpdated(double, double)),
            mapperCommitPropertyChange, SLOT (map()));
    }
    else
    {
        QObject::disconnect(pWidget, SIGNAL(signalWidgetHeld()),
            mapperBeginPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(signalWidgetReleased()),
            mapperFinishPropertyChange, SLOT (map()));
        QObject::disconnect(pWidget, SIGNAL(valueUpdated(double, double)),
            mapperCommitPropertyChange, SLOT (map()));;

        mapperCommitPropertyChange->removeMappings (pWidget);
        mapperFinishPropertyChange->removeMappings (pWidget);
        mapperBeginPropertyChange->removeMappings (pWidget);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------















//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QCheckBox* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget, SIGNAL(stateChanged  (int)), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget, SIGNAL(stateChanged  (int)), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QRadioButton* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget, SIGNAL(clicked  (bool)), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget, SIGNAL(clicked  (bool)), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const CustomLine* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget, SIGNAL(valueUpdated ()), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget, SIGNAL(valueUpdated ()), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QtToolbox::SingleSlidingDouble* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget,     SIGNAL(signalSliderHeld()), 
                pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(signalSliderReleased()), 
                pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(valueUpdated()), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalSliderHeld()), 
                   pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalSliderReleased()), 
                   pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(valueUpdated()), 
                   pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QtToolbox::SingleSlidingHDR* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget,     SIGNAL(signalSliderHeld()), 
                pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(signalSliderReleased()), 
                pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(valueUpdated()), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalSliderHeld()), 
                   pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalSliderReleased()), 
                   pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(valueUpdated()), 
                   pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QtToolbox::SingleSlidingInt * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget,     SIGNAL(signalSliderHeld()), 
                pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(signalSliderReleased()), 
                pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(valueUpdated()), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalSliderHeld()), 
                   pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalSliderReleased()), 
                   pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(valueUpdated()), 
                   pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QtToolbox::RangedSlidingDouble * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget,     SIGNAL(signalRangeHeld()), 
                pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(signalRangeReleased()), 
                pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(rangeUpdated()), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalRangeHeld()), 
                   pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalRangeReleased()), 
                   pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(rangeUpdated()), 
                   pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QtToolbox::RangedSlidingHDR * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget,     SIGNAL(signalRangeHeld()), 
                pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(signalRangeReleased()), 
                pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(rangeUpdated()), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalRangeHeld()), 
                   pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalRangeReleased()), 
                   pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(rangeUpdated()), 
                   pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QtToolbox::RangedSlidingInt * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget,     SIGNAL(signalRangeHeld()), 
                pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(signalRangeReleased()), 
                pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(rangeUpdated()), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalRangeHeld()), 
                   pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalRangeReleased()), 
                   pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(rangeUpdated()), 
                   pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QtToolbox::SpinBoxVector3* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
    /*    pPtyWidget->connect(pWidget,     SIGNAL(signalRangeHeld()), 
                pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(signalRangeReleased()), 
                pPtyWidget, SLOT(finishPropertyChange()));*/
        pPtyWidget->connect(pWidget,     SIGNAL(signalWidgetUpdated()), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
    /*    pPtyWidget->disconnect(pWidget,     SIGNAL(signalRangeHeld()), 
                   pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalRangeReleased()), 
                   pPtyWidget, SLOT(finishPropertyChange()));*/
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalWidgetUpdated()), 
                   pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapper, const QtToolbox::QuickColorPicker* pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget,     SIGNAL(signalWidgetHeld()), 
                pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(signalWidgetReleased()), 
                pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(colorUpdated(float, float, float, float)), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalWidgetHeld()), 
                   pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalWidgetReleased()), 
                   pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(colorUpdated(float, float, float, float)), 
                   pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//-----------------------------------------------------------------------------
void setConnectToPropertyChangeSlot(const IWidgetManageUndoRedo* pPtyWidget,  QSignalMapper* mapper, const QtToolbox::Slider2D * pWidget, bool makeConnection)
{
    if(makeConnection == true)
    {
        pPtyWidget->connect(pWidget,     SIGNAL(signalWidgetHeld()), 
                pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(signalWidgetReleased()), 
                pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->connect(pWidget,     SIGNAL(valueUpdated(double, double)), 
                pPtyWidget, SLOT(commitPropertyChange()));
    }
    else
    {
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalWidgetHeld()), 
                   pPtyWidget, SLOT(beginPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(signalWidgetReleased()), 
                   pPtyWidget, SLOT(finishPropertyChange()));
        pPtyWidget->disconnect(pWidget,     SIGNAL(valueUpdated(double, double)), 
                   pPtyWidget, SLOT(commitPropertyChange()));
    }
}
//----------------------------------------------------------------------------- 
//-----------------------------------------------------------------------------
/*template<class T>
class SetConnectionFunctor
{
public:
    SetConnectionFunctor(const PropertyWidget* pPtyWidget, bool makeConnection) : _pPtyWidget(pPtyWidget), _makeConnection(makeConnection) {;}

    void operator() (T* p)
    {
        setConnectToPropertyChangeSlot(_pPtyWidget, p, _makeConnection);
    }

private:
    const PropertyWidget*   _pPtyWidget;
    bool                    _makeConnection;
};
*/


LM_API_EPI void setQtToolboxConnection(const IWidgetManageUndoRedo* pPtyWidget, QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange, bool makeConnection)
{
    if(makeConnection == true)
    {
        QObject::connect(mapperBeginPropertyChange, SIGNAL(mapped(QWidget *)),
                 pPtyWidget, SLOT(beginPropertyChange(QWidget *)));
        QObject::connect(mapperFinishPropertyChange, SIGNAL(mapped(QWidget *)),
                 pPtyWidget, SLOT(finishPropertyChange(QWidget *)));
        QObject::connect(mapperCommitPropertyChange, SIGNAL(mapped(QWidget *)),
                 pPtyWidget, SLOT(commitPropertyChange(QWidget *)));
    }
    else
    {
        QObject::disconnect(mapperBeginPropertyChange, SIGNAL(mapped(QWidget *)),
                 pPtyWidget, SLOT(beginPropertyChange(QWidget *)));
        QObject::disconnect(mapperFinishPropertyChange, SIGNAL(mapped(QWidget *)),
                 pPtyWidget, SLOT(finishPropertyChange(QWidget *)));
        QObject::disconnect(mapperCommitPropertyChange, SIGNAL(mapped(QWidget *)),
                 pPtyWidget, SLOT(commitPropertyChange(QWidget *)));
    }
}
//-----------------------------------------------------------------------------
LM_API_EPI void setConnectUndoRedoWidgetToMapper(QSignalMapper* mapperBeginPropertyChange, QSignalMapper* mapperFinishPropertyChange, QSignalMapper* mapperCommitPropertyChange,
                                                 QList<QWidget*>& widgets)
{
    for (int32 iW=0; iW<widgets.size(); ++iW)
    {
        QtToolbox::SingleSlidingDouble * pSsd = qobject_cast<QtToolbox::SingleSlidingDouble *>(widgets[iW]);
        if (pSsd != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pSsd, true);

        QtToolbox::SingleSlidingHDR * pSshdr = qobject_cast<QtToolbox::SingleSlidingHDR *>(widgets[iW]);
        if (pSshdr != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pSshdr, true);

        QtToolbox::SingleSlidingInt * pSsi = qobject_cast<QtToolbox::SingleSlidingInt *>(widgets[iW]);
        if (pSsi != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pSsi, true);

        QtToolbox::RangedSlidingDouble * pRsd = qobject_cast<QtToolbox::RangedSlidingDouble *>(widgets[iW]);
        if (pRsd != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pRsd, true);

        QtToolbox::RangedSlidingHDR * pRshdr = qobject_cast<QtToolbox::RangedSlidingHDR *>(widgets[iW]);
        if (pRshdr != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pRshdr, true);

        QtToolbox::RangedSlidingInt * pRsi = qobject_cast<QtToolbox::RangedSlidingInt *>(widgets[iW]);
        if (pRsi != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pRsi, true);

        QtToolbox::SpinBoxVector3 * PSbv = qobject_cast<QtToolbox::SpinBoxVector3 *>(widgets[iW]);
        if (PSbv != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, PSbv, true);

        QtToolbox::SpinBox * PSb = qobject_cast<QtToolbox::SpinBox *>(widgets[iW]);
        if (PSb != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, PSb, true);

        QtToolbox::QuickColorPicker * PQcp = qobject_cast<QtToolbox::QuickColorPicker *>(widgets[iW]);
        if (PQcp != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, PQcp, true);

        QtToolbox::Slider2D * pSlider2D = qobject_cast<QtToolbox::Slider2D *>(widgets[iW]);
        if (pSlider2D != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pSlider2D, true);

        QtToolbox::ComboBox * pCBox = qobject_cast<QtToolbox::ComboBox *>(widgets[iW]);
        if (pCBox != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pCBox, true);

        QCheckBox * pCb = qobject_cast<QCheckBox *>(widgets[iW]);
        if (pCb != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pCb, true);

        QRadioButton * pRb = qobject_cast<QRadioButton *>(widgets[iW]);
        if (pRb != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pRb, true);

        CustomLine * pCl = qobject_cast<CustomLine *>(widgets[iW]);
        if (pCl != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pCl, true);

        QListWidget * pLW = qobject_cast<QListWidget *>(widgets[iW]);
        if (pLW != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pLW, true);

        QtToolbox::LineEdit * pLE = qobject_cast<QtToolbox::LineEdit *>(widgets[iW]);
        if (pLE != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pLE, true);

        QPushButton * pPB = qobject_cast<QPushButton *>(widgets[iW]);
        if (pPB != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pPB, true);

        QtToolbox::NumberEdit * pNe = qobject_cast<QtToolbox::NumberEdit *>(widgets[iW]);
        if (pNe != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pNe, true);
    
        QtToolbox::NumberEditVector3 * pNe3 = qobject_cast<QtToolbox::NumberEditVector3 *>(widgets[iW]);
        if (pNe3 != NULL) setConnectToPropertyChangeSlot(mapperBeginPropertyChange, mapperFinishPropertyChange, mapperCommitPropertyChange, pNe3, true);
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace EPI
