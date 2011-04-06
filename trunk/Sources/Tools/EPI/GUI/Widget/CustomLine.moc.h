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
#ifndef EPI_CUSTOM_LINE_H
#define EPI_CUSTOM_LINE_H

#include <QWidget>


class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QMimeData;

namespace EPI
{

class LM_API_EPI CustomLine : public QWidget
{
    Q_OBJECT
public:
    CustomLine( QWidget *parent = 0,
                const QString & name = "");
    virtual ~CustomLine();

    virtual void setupUi();

    QString text() const;
    void setText(const QString & text);
    void getText(Core::String& text) const;
    void setText(const Core::String & text);
    
    void setReadOnly(bool readOnly);

    void pushAuthorizedDropMimeData(const QString & mimeData);

    void dragEnterEvent(QDragEnterEvent * dragEnterEvent);
    void dragMoveEvent(QDragMoveEvent * dragMoveEvent);
    void dragLeaveEvent(QDragLeaveEvent * dragLeaveEvent);
    void dropEvent(QDropEvent * dropEvent);

protected Q_SLOTS:
    virtual void editingFinished();
    virtual void textChanged();
    virtual void clearValue();
    virtual void openFile();
    virtual void refresh();

Q_SIGNALS:
    void valueUpdated();
    void refreshValue();

private:
    bool isAuthorizedMimeData(const QMimeData * mimeData, QString & format);
   
protected:
    QGridLayout *   _gridLayout;
    QLabel *        _nameLabel;
    QLineEdit *     _value;
    QPushButton *   _clear;
    QPushButton *   _localOpen;
    QPushButton *   _refresh;
    QString         _name;
    bool            _isValueChanged;

    Core::String    _path;

    QStringList     _authorizedMimeData;
};

}//namespace EPI

#endif/*EPI_CUSTOM_LINE_H*/
