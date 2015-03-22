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
#ifndef EPI_PROPERTY_DOCUMENT_SPLINE_H_
#define EPI_PROPERTY_DOCUMENT_SPLINE_H_

#include <QWidget>
#include <QGridLayout>

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

namespace QtToolbox
{
    class SmartGroupBox;
    class ComboBox;
    class LineEdit;
}//namespace QtToolbox

namespace EPI
{
class PtyWidgetSpline;
class PtyWidgetDocSpline;
class GuiDocument;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyDocSpline : public PropertyNode
{
    friend class PtyWidgetDocSpline;

public:
    PtyDocSpline(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorldForRepresentation, const Ptr<Universe::World>& pWorld, const Ptr<StateRecorder>& pStateRecorder, const Core::String& title = L"DocSpline");
    virtual ~PtyDocSpline();

    virtual Ptr<PropertyWidget> internalCreatePropertyWidget    (const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void                internalCopy                    (const Ptr<Property>& pSrc);
    virtual Ptr<Property>       clone                           () const;
    virtual void                updateData();
    virtual void                updateProperty();
    
    const Ptr<Property>& getPtySpline();

    const Core::String& getWorldDecoPath();
    void setWorldDecoPath(const Core::String& path) {_worldDeco = path;}
private:
    String                  _worldDeco;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetDocSpline : public PropertyWidget
{
    friend class PtyDocSpline;

public:
    PtyWidgetDocSpline(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetDocSpline();

    void readProperty   ();
    void writeProperty  (QWidget* pWidget);
    void setupUi        ();

private:
    QGridLayout *               _layout;
    QtToolbox::SmartGroupBox *  _groupBox;
    QtToolbox::LineEdit *       _worldDeco;
    Ptr<PropertyWidget>         _pPtyWidgetSpline;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif /*EPI_PROPERTY_DOCUMENT_SPLINE_H_*/