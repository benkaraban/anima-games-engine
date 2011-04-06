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
#ifndef EPI_LIGHTMAP_GENERATION_OPTIONS_H_
#define EPI_LIGHTMAP_GENERATION_OPTIONS_H_

#include <QWidget>

#include <Core/String.h>

#include <EPI/ImportInfo/LightmapsGenerationInfo.h>

class QLabel;
class QGridLayout;
class QLineEdit;
class QPushButton;
class QDialogButtonBox;
class QCheckBox;
class QListWidget;

namespace QtToolbox
{
class ComboBox;
class LineEdit;
}

namespace EPI
{

struct LightmapsGenerationInfo;

class LightmapsGenerationOptions : public QWidget
{
    Q_OBJECT
public:
    LightmapsGenerationOptions(QWidget * parent = 0, bool import = true);
    ~LightmapsGenerationOptions();

    void setupUi(bool import);

    Ptr<LightmapsGenerationInfo> getInfo() const;

protected Q_SLOTS:
    void checkBeforeValidation();
    void rejected();

Q_SIGNALS:
    void reject();
    void accept();
    void sizeChanged(int, int);
    
private:
    QGridLayout *               _layout;

    QtToolbox::LineEdit *       _suffix;
    QtToolbox::LineEdit *       _tagFilter;
    QtToolbox::ComboBox *       _defaultRes;
    QtToolbox::ComboBox *       _textureAtlasSize;
    QtToolbox::LineEdit *       _batchSize;
    QtToolbox::LineEdit *       _border;

    QDialogButtonBox *          _buttonBox;
    QPushButton *               _generate;
};

}//namespace EPI

#endif/*EPI_TEXTURE_GENERATION_OPTIONS_H_*/