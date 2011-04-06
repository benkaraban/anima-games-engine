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
#ifndef EPI_TEXTURE_GENERATION_OPTIONS_H_
#define EPI_TEXTURE_GENERATION_OPTIONS_H_

#include <QWidget>

#include <Core/String.h>
#include <Workflow/TexCompression.h>

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

struct ImportTextureInfo;

class TextureGenerationOptions : public QWidget
{
    Q_OBJECT
public:
    TextureGenerationOptions(QWidget * parent = 0, bool import = true);
    ~TextureGenerationOptions();

    void setupUi(bool import);
    void setupUiMultipleImports();

    inline const Core::List<Core::String> & getImageNames() const { return _imageNames; }
    const Core::String & getTextureName() const;
    Ptr<Workflow::TextureOptions> getTextureOptions() const;

    void setTextureInfo(const Ptr<ImportTextureInfo> & pImportTextureInfo);

public Q_SLOTS:
    bool fileOpen();
    void optionsChanged();

protected Q_SLOTS:
    void checkBeforeValidation();
    void rejected();
    void onSelectAll(int);
    

Q_SIGNALS:
    void reject();
    void accept();
    void sizeChanged(int, int);
    
private:
    QGridLayout *           _layout;
    QLabel *                _fileNameLabel;
    QLineEdit *             _fileName;
    QPushButton *           _fileOpen;
    QtToolbox::LineEdit *   _textureEdit;
    QtToolbox::ComboBox *   _format;
    QtToolbox::ComboBox *   _mipmapFilter;
    QtToolbox::ComboBox *   _textureType;
    QtToolbox::LineEdit *   _kaiserWidth;
    QtToolbox::LineEdit *   _kaiserAlpha;
    QtToolbox::LineEdit *   _kaiserStretch;
    QCheckBox *             _generateMipmap;
    QDialogButtonBox *      _buttonBox;
    QPushButton *           _generate;

    QListWidget *           _listWidget;
    QCheckBox *             _selectAll;

    Core::List<Core::String>    _imageNames;
    Core::String                _textureName;
    Core::String                _path;

    Ptr<Workflow::TextureOptions>   _pTextureOptions;

    bool                    _multipleImports;
};

}//namespace EPI

#endif/*EPI_TEXTURE_GENERATION_OPTIONS_H_*/