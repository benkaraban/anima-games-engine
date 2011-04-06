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
#ifndef DOCUMENT_SCRIPT_H_
#define DOCUMENT_SCRIPT_H_

#include <EPI/Document/DocumentBase.moc.h>
#include <EPI/Document/IDocumentWidget.h>

namespace EPI
{
class DocumentScript;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class DocumentScriptWidget : public IDocumentWidget
{
    Q_OBJECT

public:
    DocumentScriptWidget(const DocumentScript& doc, QWidget* parent);
    virtual ~DocumentScriptWidget();

private:
    void setupUi();


public Q_SLOTS:
    void enableWorldEdit();

private Q_SLOTS:
    void swapEditWorld();
    void saveEditWorld();
    
private:
    DocumentScript& _doc;
    QGridLayout * _layout;
    QPushButton * _editWorld;
    QPushButton * _saveEditWorld;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI DocumentScript : public DocumentBase
{
    Q_OBJECT

public:
    DocumentScript(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr);
    DocumentScript(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr, const Core::String & fileName);

    virtual ~DocumentScript();

    virtual Ptr<Property> getDefaultProperty() const;

    const Ptr<Property> getPropertyScript() const;
    Ptr<Property> getPropertyScript();

    virtual const Ptr<Universe::World>& getWorld() const;
    virtual const Ptr<Property>&    getPropertyDocumentContentStart() const;


    virtual void runStopScript();

    virtual Ptr<IDocumentWidget> createDocumentWidget(QWidget* parent) const;

    void swapEditWorld();
    void saveEditWorld();

    virtual const Ptr<StateRecorder>&                     getStateRecorder() const;
    virtual const Ptr<DocumentRenderer>&            getDocumentRenderer() const;
    virtual Ptr<Universe::World>                    getWorldDeco();
    virtual const Ptr<Universe::World>&             getWorldInfoContent() const;
    virtual void                                    setWorldDeco(const Ptr<Universe::World>& pWorld);
    virtual const Ptr<GuiDescription>&              getGuiDescription() const;

    virtual void                                deleteSelectedProperties();
    virtual void                                duplicateSelectedProperties();
    virtual void                                addChildrenToCurrentProperty(Core::List<Ptr<Property> > pties);
    virtual const Ptr<Property>&                getPropertyDocumentContent() const;

    virtual Ptr<Property>                       getPtyForEdition() const;

    virtual void                                selectProperty(Ptr<Property> p);
    virtual void                                selectProperty(int32 id);
    virtual void                                selectProperties(const Core::List<int32>& id);

    virtual const Core::List<Ptr<Property> >&   getSelectedProperties() const;
    virtual Ptr<Property>                       getProperty(int32 id);
    virtual void                                getProperty(const Core::List<int32>& ids, Core::List<Ptr<Property> >& pties);

    virtual bool isEditing();
    virtual void setEditing(bool edit);

    virtual int32               getPropertyIdFromUniverseNode(const Ptr<Universe::Node>& pNode);
    virtual QList<QAction*>&    getActionsCreateProperty();

    virtual const Core::List<Ptr<PtyNodeCamera> >& getAllCameraContent() const;


    virtual void                            setClosable(bool closable);
    virtual void                            setModified(bool modified);
    virtual bool                            canClose(bool userInteraction) const;


    virtual bool dropInViewportIsPossible() const;
    virtual bool dropThisFormatIsAuthorized(const QMimeData* mimeData, QString& format) const;
    virtual void dropThisFile(const QMimeData* mimeData, QString& format);

    virtual void escape();

    virtual Ptr<Universe::NodeCamera> getCameraEdition() const;

    virtual Ptr<Property>   getActivePlayerPosition();

    bool isInInEditWorld() {return _isInEditWorld;}

    virtual Ptr<IWorldScriptable>  getWorldScriptable() const;

private:
    void initialize();

public Q_SLOTS:
    virtual void activeProperty(int32 id);
    virtual void contentIsModified();
    virtual void undo();
    virtual void redo();
    virtual void save();
    virtual void saveAs();

private Q_SLOTS:
    void errorInScript();

Q_SIGNALS:
    void worldEditIsEnable();

private:
    void saveScript();
    void saveFile(const Core::String & fileName);
    void loadFile(const Core::String & fileName);

    bool _isInEditWorld;
};
//-----------------------------------------------------------------------------
}  // namespace EPI

#endif /*DOCUMENT_SCRIPT_H_*/