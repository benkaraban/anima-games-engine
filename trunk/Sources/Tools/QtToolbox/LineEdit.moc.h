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
#ifndef LINE_EDIT_H_
#define LINE_EDIT_H_

#include <QWidget>
#include <QStringList>

class QGridLayout;
class QLabel;
class QLineEdit;
class QMimeData;

namespace QtToolbox
{

class LM_API_QTB LineEdit : public QWidget
{
    Q_OBJECT
public:
    LineEdit(   QWidget *parent = 0,
                const QString & name = "",
                bool showClearButton = false,
                const QString & defaultValue = "",
                int lineEditWidth = 170);
    virtual ~LineEdit();

    virtual void setupUi();

    void setValidator(int min, int max);
    void setValidator(double min, double max, int decimals);
    void setValidator(QString regExp);

    QString text() const;
    void setText(const QString & text);

    void setReadOnly(bool readOnly);

    void setLabelColor(const QColor & color);

    void pushAuthorizedDropMimeData(const QString & mimeData);

    void dragEnterEvent(QDragEnterEvent * dragEnterEvent);
    void dragMoveEvent(QDragMoveEvent * dragMoveEvent);
    void dragLeaveEvent(QDragLeaveEvent * dragLeaveEvent);
    void dropEvent(QDropEvent * dropEvent);

protected:
	virtual void paintEvent(QPaintEvent * paintEvent);
    virtual void mousePressEvent(QMouseEvent * mouseEvent);
	virtual void mouseReleaseEvent(QMouseEvent * mouseEvent);

protected Q_SLOTS:
    virtual void editingFinished();
    virtual void textChanged();

Q_SIGNALS:
    void valueUpdated();

private:
    bool isAuthorizedMimeData(const QMimeData * mimeData, QString & format);
    void createPixMaps();
   
protected:
    QGridLayout *   _gridLayout;
    QLabel *        _nameLabel;
    QLineEdit *     _value;
    QString         _name;
    bool            _showClearButton;
    QString         _defaultValue;
    bool            _isValueChanged;
    int             _lineEditWidth;

    QStringList     _authorizedMimeData;
    QPixmap         _pixClear;
    QPoint          _pointClicked;
};

}//namespace QtToolbox

#endif/*LINE_EDIT_H_*/
