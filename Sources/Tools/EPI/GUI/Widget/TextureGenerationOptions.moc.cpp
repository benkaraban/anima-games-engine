/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#include "TextureGenerationOptions.moc.h"

#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidget>

#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>

#include <Core/System/Files.h>

#include <EPI/Constants.h>
#include <EPI/ImportInfo/ImportTextureInfo.h>

#include <EPI/Gui/Widget/FileDialog.moc.h>

namespace EPI
{

TextureGenerationOptions::TextureGenerationOptions(QWidget * parent, bool import)
:   QWidget(parent),
    _layout(NULL),
    _fileNameLabel(NULL),
    _fileName(NULL),
    _fileOpen(NULL),
    _textureEdit(NULL),
    _format(NULL),
    _mipmapFilter(NULL),
    _textureType(NULL),
    _kaiserWidth(NULL),
    _kaiserAlpha(NULL),
    _kaiserStretch(NULL),
    _generateMipmap(NULL),
    _buttonBox(NULL),
    _generate(NULL),
    _listWidget(NULL),
    _selectAll(NULL),
    _textureName(L""),
    _path(IMPORT_FROM_SHARED_FOLDER),
    _pTextureOptions(null),
    _multipleImports(false)
{
    setupUi(import);
}

TextureGenerationOptions::~TextureGenerationOptions()
{
    if(_layout != NULL)
        delete _layout;
    if(_fileNameLabel != NULL)
        delete _fileNameLabel;
    if(_fileName != NULL)
        delete _fileName;
    if(_fileOpen != NULL)
        delete _fileOpen;
    if(_textureEdit != NULL)
        delete _textureEdit;
    if(_format != NULL)
        delete _format;
    if(_mipmapFilter != NULL)
        delete _mipmapFilter;
    if(_textureType != NULL)
        delete _textureType;
    if(_kaiserWidth != NULL)
        delete _kaiserWidth;
    if(_kaiserAlpha != NULL)
        delete _kaiserAlpha;
    if(_kaiserStretch != NULL)
        delete _kaiserStretch;
    if(_generateMipmap != NULL)
        delete _generateMipmap;
    if(_buttonBox != NULL)
        delete _buttonBox;
    if(_generate != NULL)
        delete _generate;
    if(_listWidget != NULL)
        delete _listWidget;
    if(_selectAll != NULL)
        delete _selectAll;

    _layout         = NULL;
    _fileNameLabel  = NULL;
    _fileName       = NULL;
    _fileOpen       = NULL;
    _textureEdit    = NULL;
    _format         = NULL;
    _mipmapFilter   = NULL;
    _textureType    = NULL;
    _kaiserWidth    = NULL;
    _kaiserAlpha    = NULL;
    _kaiserStretch  = NULL;
    _generateMipmap = NULL;
    _buttonBox      = NULL;
    _generate       = NULL;
    _listWidget     = NULL;
    _selectAll      = NULL;
}

void TextureGenerationOptions::setupUi(bool import)
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _fileNameLabel = new QLabel("File :", this);
    _fileNameLabel->setFixedHeight(16);
    _layout->addWidget(_fileNameLabel, 0, 0, 1, 1);

    _fileName = new QLineEdit(this);
    _fileName->setFixedHeight(16);
    _fileName->setReadOnly(true);
    _layout->addWidget(_fileName, 0, 1, 1, 1);
    
    _fileOpen = new QPushButton(QIcon(":/icons/folder.png"), "", this);
    _fileOpen->setFixedWidth(35);
    _fileOpen->setFixedHeight(16);
    _layout->addWidget(_fileOpen, 0, 2, 1, 1);

    _textureEdit = new QtToolbox::LineEdit(this, "Texture name");
    _textureEdit->setValidator("[A-Za-z0-9_]+");
    _layout->addWidget(_textureEdit, 1, 0, 1, 3);

    _format = new QtToolbox::ComboBox(this, "Format :");
    _layout->addWidget(_format, 2, 0, 1, 3);

    _format->addItem( Core::String8(textureFormatToString(Assets::TEX_FORMAT_ARGB8)).c_str());
    _format->addItem( Core::String8(textureFormatToString(Assets::TEX_FORMAT_DXTC1)).c_str());
    _format->addItem( Core::String8(textureFormatToString(Assets::TEX_FORMAT_DXTC3)).c_str());
    _format->addItem( Core::String8(textureFormatToString(Assets::TEX_FORMAT_DXTC5)).c_str());

    _mipmapFilter = new QtToolbox::ComboBox(this, "Mipmap filter :");
    _layout->addWidget(_mipmapFilter, 3, 0, 1, 3);

    _mipmapFilter->addItem( Core::String8(mipmapFilterToString(Workflow::MIPMAP_FILTER_BOX)).c_str());
    _mipmapFilter->addItem( Core::String8(mipmapFilterToString(Workflow::MIPMAP_FILTER_KAISER)).c_str());
    _mipmapFilter->addItem( Core::String8(mipmapFilterToString(Workflow::MIPMAP_FILTER_TRIANGLE)).c_str());
    //KAISER par dÈfaut
    _mipmapFilter->selectItem( Core::String8(mipmapFilterToString(Workflow::MIPMAP_FILTER_KAISER)).c_str());
    
    _textureType = new QtToolbox::ComboBox(this, "Texture type :");
    _layout->addWidget(_textureType, 4, 0, 1, 3);

    _textureType->addItem( Core::String8(textureTypeToString(Workflow::TEXTURE_COLORMAP)).c_str());
    _textureType->addItem( Core::String8(textureTypeToString(Workflow::TEXTURE_HEIGHTMAP)).c_str());
    _textureType->addItem( Core::String8(textureTypeToString(Workflow::TEXTURE_LIGHTMAP)).c_str());
    _textureType->addItem( Core::String8(textureTypeToString(Workflow::TEXTURE_NORMALMAP)).c_str());
    
    _kaiserWidth = new QtToolbox::LineEdit(this, "Kaiser width :", false, "3.0");
    _kaiserWidth->setValidator(0.0, 5.0, 6);
    _layout->addWidget(_kaiserWidth, 5, 0, 1, 3);

    _kaiserAlpha = new QtToolbox::LineEdit(this, "Kaiser alpha :", false, "4.0");
    _kaiserAlpha->setValidator(0.0, 5.0, 6);
    _layout->addWidget(_kaiserAlpha, 6, 0, 1, 3);

    _kaiserStretch = new QtToolbox::LineEdit(this, "Kaiser stretch :", false, "1.0");
    _kaiserStretch->setValidator(0.0, 5.0, 6);
    _layout->addWidget(_kaiserStretch, 7, 0, 1, 3);

    _generateMipmap = new QCheckBox("Generate mipmap", this);
    _generateMipmap->setChecked(true);
    _layout->addWidget(_generateMipmap, 8, 0, 1, 3, Qt::AlignRight);

    if(import)
    {
        _buttonBox = new QDialogButtonBox(this);
        _buttonBox->setOrientation(Qt::Horizontal);
        _buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(false);
        
        connect(_buttonBox, SIGNAL(rejected()), this, SLOT(rejected()));
        connect(_buttonBox, SIGNAL(accepted()), this, SLOT(checkBeforeValidation()));

        _layout->addWidget(_buttonBox, 9, 0, 1, 3);
    }
    else
    {
        _generate = new QPushButton(QIcon(":/icons/gears.png"), "Generate", this);
        //_generate->setEnabled(false);

        _fileName->setReadOnly(true);
        _fileOpen->setEnabled(false);
        _textureEdit->setEnabled(false);

        connect(_generate, SIGNAL(clicked()), this, SLOT(checkBeforeValidation()));
        _layout->addWidget(_generate, 9, 0, 1, 3);
    }
    

    setLayout(_layout);
    this->setFixedWidth(267);
    this->setFixedHeight(253);

    connect(_fileOpen, SIGNAL(clicked()), this, SLOT(fileOpen()));
    
    if(!import)
    {
        connect(_format,        SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_mipmapFilter,  SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_textureType,   SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_kaiserWidth,   SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_kaiserAlpha,   SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_kaiserStretch, SIGNAL(valueUpdated()),     this, SLOT(optionsChanged()));
        connect(_generateMipmap,SIGNAL(stateChanged(int)),  this, SLOT(optionsChanged()));
    }
}

void TextureGenerationOptions::setupUiMultipleImports()
{
    delete _layout;
    _layout = NULL;

    delete _fileNameLabel;
    delete _fileName;
    delete _fileOpen;
    delete _textureEdit;
    _fileNameLabel  = NULL;
    _fileName       = NULL;
    _fileOpen       = NULL;
    _textureEdit    = NULL;

    _multipleImports = true;

    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _listWidget = new QListWidget(this);
    _listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _selectAll = new QCheckBox("Select all", this);
    _selectAll->setChecked(true);

    //1Ëre colonne
    _layout->addWidget(_listWidget, 0, 0, 7, 1);
    _layout->addWidget(_selectAll, 7, 0, 1, 1, Qt::AlignLeft);

    //2Ëme colonne
    _layout->addWidget(_format, 0, 1, 1, 1);
    _layout->addWidget(_mipmapFilter, 1, 1, 1, 1);
    _layout->addWidget(_textureType, 2, 1, 1, 1);
    _layout->addWidget(_kaiserWidth, 3, 1, 1, 1);
    _layout->addWidget(_kaiserAlpha, 4, 1, 1, 1);
    _layout->addWidget(_kaiserStretch, 5, 1, 1, 1);
    _layout->addWidget(_generateMipmap, 6, 1, 1, 1, Qt::AlignRight);
    _layout->addWidget(_buttonBox, 7, 1, 1, 1);

    setLayout(_layout);

    this->setFixedWidth(500);
    this->setFixedHeight(206);

    emit sizeChanged(width(), height());

    connect(_selectAll,  SIGNAL(stateChanged (int)), this, SLOT(onSelectAll(int)));
}

bool TextureGenerationOptions::fileOpen()
{
    //QStringList files = QFileDialog::getOpenFileNames(NULL, tr("Open File"), String8(_path).c_str(), String8(AUTHORISED_IMAGE_EXT).c_str());

    Core::List<String> files = FileDialog::getOpenFileNames(MiscConst::memoPathShared, IMPORT_FROM_SHARED_FOLDER,  MiscConst::sharedFolderShortcuts, NULL, L"Open File", AUTHORISED_IMAGE_EXT);

    if(files.size() > 0 )
    {
        if(files.size() == 1)
        {
            Core::String fileName(files[0]);
            if(Core::System::getPath(fileName).startsWith(IMPORT_FROM_SHARED_FOLDER))
            {
                String path = Core::System::getPath(fileName);
                MiscConst::addPathToMemoPath(MiscConst::memoPathShared, path);

                Core::String textureName = Core::System::getFileBaseName(fileName);
                Core::String heightString = L"-height";
                Core::String normalString = L"-normal";
                Core::String::size_type pos;
                
                //-normal
                if(fileName.find(normalString) != Core::String::npos)
                    _textureType->selectItem( Core::String8(textureTypeToString(Workflow::TEXTURE_NORMALMAP)).c_str());
                //-height
                else if( (pos = textureName.find(heightString)) != Core::String::npos)
                {
                    _textureType->selectItem( Core::String8(textureTypeToString(Workflow::TEXTURE_HEIGHTMAP)).c_str());
                    textureName.replace(pos, heightString.length(), normalString);
                }
                else
                    _textureType->selectItem( Core::String8(textureTypeToString(Workflow::TEXTURE_COLORMAP)).c_str());

                _fileName->setText(String8(fileName).c_str());
                _textureEdit->setText(String8(textureName).c_str());

                _imageNames.push_back(fileName);
                _path = Core::System::getPath(fileName);

                _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(true);
            }
            else
            {
                Core::String message = L"Your file doesn't seem to be in the right folder : ";
                message << IMPORT_FROM_SHARED_FOLDER;
                QMessageBox::critical(this, "Nyx", Core::String8(message).c_str());
            }
        }
        else
        {
            setupUiMultipleImports();
            for(int32 ii = 0; ii<files.size(); ii++)
            {
                Core::String fileName(files[ii]);
                if(Core::System::getPath(fileName).startsWith(IMPORT_FROM_SHARED_FOLDER))
                {
                    QListWidgetItem * item = new QListWidgetItem(StrToQStr(files[ii]), _listWidget);
                    item->setCheckState(Qt::Checked);
                    _listWidget->insertItem(ii, item);
                }
            }

            if(_listWidget->count() == 0)
            {
                Core::String message = L"Your files don't seem to be in the right folder : ";
                message << IMPORT_FROM_SHARED_FOLDER;
                QMessageBox::critical(this, "Nyx", Core::String8(message).c_str());
                emit reject();
            }
            else
            {
                _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(true);
            }
        }
    }
    return (files.size() > 0);
}

void TextureGenerationOptions::checkBeforeValidation()
{
    double kaiserWidth = 0.0;
    double kaiserAlpha = 0.0;
    double kaiserStretch = 0.0;

    Core::String userWidth(_kaiserWidth->text().toStdString().c_str());
    Core::String userAlpha(_kaiserAlpha->text().toStdString().c_str());
    Core::String userStretch(_kaiserStretch->text().toStdString().c_str());

    kaiserWidth = Core::toFloat(userWidth);
    kaiserAlpha = Core::toFloat(userAlpha);
    kaiserStretch = Core::toFloat(userStretch);

    bool errorDetected = false;

    QPalette error = _kaiserWidth->palette();
    QPalette noError = _kaiserWidth->palette();
    error.setColor(QPalette::Text, Qt::red);
    noError.setColor(QPalette::Text, Qt::black);

    if(kaiserWidth < 0.0 || kaiserWidth > 5.0)
    {
        _kaiserWidth->setPalette(error);
        errorDetected = true;
    }
    else
        _kaiserWidth->setPalette(noError);

    if(kaiserAlpha < 0.0 || kaiserAlpha > 5.0)
    {
        _kaiserAlpha->setPalette(error);
        errorDetected = true;
    }
    else
        _kaiserAlpha->setPalette(noError);

    if(kaiserStretch < 0.0 || kaiserStretch > 5.0)
    {    
        _kaiserStretch->setPalette(error);
        errorDetected = true;
    }
    else
        _kaiserStretch->setPalette(noError);

    if(!errorDetected)
    {
        //Import fichier unique
        if(_textureEdit != NULL)
        {
            _textureName = Core::String(_textureEdit->text().toStdString().c_str());
            
            if(_generate != NULL)
                _generate->setEnabled(false);

            emit accept();
        }
        else
        {
            for(int32 ii =0; ii<_listWidget->count(); ii++)
            {
                if(_listWidget->item(ii)->checkState() == Qt::Checked)
                {    
                    Core::String imageName(_listWidget->item(ii)->text().toStdString().c_str());
                    _imageNames.push_back(imageName);
                }
            }

            if(_imageNames.size() > 0)
                emit accept();
            else
                QMessageBox::critical(this, "Nyx", "You must select files before hitting 'OK'.");
        }
    }
}

void TextureGenerationOptions::rejected()
{
    emit reject();
}

const Core::String & TextureGenerationOptions::getTextureName() const
{ 
    if(_multipleImports && _imageNames.size() > 0)
        return _imageNames[0];
    else
        return _textureName; 
}

Ptr<Workflow::TextureOptions> TextureGenerationOptions::getTextureOptions() const
{
    Ptr<Workflow::TextureOptions> pOptions;
    if(_pTextureOptions == null)
        pOptions = Ptr<Workflow::TextureOptions>(new Workflow::TextureOptions());
    else
        pOptions = _pTextureOptions;

    double kaiserWidth = 0.0;
    double kaiserAlpha = 0.0;
    double kaiserStretch = 0.0;

    Core::String userWidth(_kaiserWidth->text().toStdString().c_str());
    Core::String userAlpha(_kaiserAlpha->text().toStdString().c_str());
    Core::String userStretch(_kaiserStretch->text().toStdString().c_str());

    kaiserWidth = Core::toFloat(userWidth);
    kaiserAlpha = Core::toFloat(userAlpha);
    kaiserStretch = Core::toFloat(userStretch);

    pOptions->kaiserWidth    = kaiserWidth;
    pOptions->kaiserAlpha    = kaiserAlpha;
    pOptions->kaiserStretch  = kaiserStretch;
    
    pOptions->format         = stringToTextureFormat(_format->selectedText());
    pOptions->mipmapFilter   = stringToMipmapFilter(_mipmapFilter->selectedText());
    pOptions->texType        = stringToTextureType(_textureType->selectedText());

    pOptions->generateMipmap = (_generateMipmap->checkState() == Qt::Checked);

    return pOptions;
}

void TextureGenerationOptions::onSelectAll(int checked)
{
    Qt::CheckState checkState =(Qt::CheckState)checked;

    for(int ii =0; ii<_listWidget->count(); ii++)
    {
        _listWidget->item(ii)->setCheckState(checkState);
    }
}

void TextureGenerationOptions::setTextureInfo(const Ptr<ImportTextureInfo> & pImportTextureInfo)
{
    _fileName->setText(String8(pImportTextureInfo->imageName).c_str());
    _textureEdit->setText(String8(Core::System::getFileName(pImportTextureInfo->textureName)).c_str());

    _pTextureOptions = pImportTextureInfo->pTextureOptions;

    _format->selectItem(String8(textureFormatToString(_pTextureOptions->format)).c_str());
    _mipmapFilter->selectItem(String8(mipmapFilterToString(_pTextureOptions->mipmapFilter)).c_str());
    _textureType->selectItem(String8(textureTypeToString(_pTextureOptions->texType)).c_str());
    _kaiserWidth->setText(String8(Core::toString(_pTextureOptions->kaiserWidth)).c_str());
    _kaiserAlpha->setText(String8(Core::toString(_pTextureOptions->kaiserAlpha)).c_str());
    _kaiserStretch->setText(String8(Core::toString(_pTextureOptions->kaiserStretch)).c_str());
    _generateMipmap->setChecked(_pTextureOptions->generateMipmap);

    //_generate->setEnabled(false);
}

void TextureGenerationOptions::optionsChanged()
{
    if(_generate != NULL)
    {
        _generate->setEnabled(true);
    }
}

}//namespace EPI