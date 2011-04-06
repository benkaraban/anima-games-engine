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
#include "../../Prelude.h"

#include <QWidget>
#include <QPushButton>

#include <Core/Standard.h>
#include <Core/Logger.h>
#include <Core/Math/ModuleMath.h>

#include <Universe/World.h>

//a commenter si on travaille sur QtToolbox
//#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>
//#include <QtToolbox/PushButtonMenu.moc.h>
#include <QtToolbox/RangedSlidingTimeValue.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/SpinBox.moc.h>
#include <QtToolbox/SpinBoxVector.moc.h>
#include <QtToolbox/WidgetRenderArea.moc.h>
#include <QtToolbox/Slider2D/Slider2D.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

//#define LOG_FCT_ENTRY

#ifdef LOG_FCT_ENTRY
#define LOG_FCT_NAME INF << LM_LOG_TAG_FUNCTION;
#else
#define LOG_FCT_NAME
#endif


