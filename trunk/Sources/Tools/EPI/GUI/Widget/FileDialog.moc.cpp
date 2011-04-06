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
#include "FileDialog.moc.h"

#include <QVBoxLayout>

#include <EPI/Constants.h>
#include <QUrl>


namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FileDialog::FileDialog(QWidget* parent):
    QDialog(parent)
{
}
//-----------------------------------------------------------------------------
FileDialog::~FileDialog()
{
}
//-----------------------------------------------------------------------------
String FileDialog::getOpenFileName(Core::List<String>& histDir, const String& defaultDir, const Core::List<String>& shortcuts, QWidget *parent, const String &caption, const String &filter)
{
    Core::List<String> ret = FileDialog::getOpenFileNames(histDir, defaultDir, shortcuts, parent, caption, filter, QFileDialog::ExistingFile);

    if (ret.size()>0)   return ret[0];
    return L"";
}
//-----------------------------------------------------------------------------
Core::List<String> FileDialog::getOpenFileNames(Core::List<String>& histDir, const String& defaultDir, const Core::List<String>& shortcuts, QWidget *parent, const String &caption, const String &filter, const QFileDialog::FileMode& fileMode)
{
    Core::List<String> strings;

    //QStringList files = QFileDialog::getOpenFileNames(NULL, tr("Open File"), String8(dir).c_str(), String8(filter).c_str());

    QList<QUrl> urls;
    QUrl u = QUrl::fromLocalFile(StrToQStr(defaultDir));
    urls << u;
    for (int32 ii=0; ii<shortcuts.size(); ++ii)
    {
        if (Core::System::dirExists(shortcuts[ii]))
        {
            QUrl u = QUrl::fromLocalFile(StrToQStr(shortcuts[ii]));
            urls << u;
        }
    }
    
     

    String dir;

    if (histDir.size()>0)
        dir = histDir.back();
    else
        dir = defaultDir;

    QFileDialog qFileDlg (parent, StrToQStr(caption));
   /* QFileDialogArgs args;
        args.parent = parent;
        args.caption = StrToQStr(caption);
        args.directory = QFileDialogPrivate::workingDirectory(StrToQStr(defaultDir));
        args.selection = QFileDialogPrivate::initialSelection(StrToQStr(defaultDir));
        args.filter = filter;
        args.mode = QFileDialog::ExistingFiles;
        args.options = options;*/


        qFileDlg.setFilter(StrToQStr(filter));
        qFileDlg.setDirectory(StrToQStr(dir));
        qFileDlg.setOption(QFileDialog::ShowDirsOnly, false);
        qFileDlg.setOption(QFileDialog::DontResolveSymlinks, false);
        qFileDlg.setOption(QFileDialog::DontConfirmOverwrite, false);
        qFileDlg.setOption(QFileDialog::DontUseNativeDialog, false);
        qFileDlg.setOption(QFileDialog::ReadOnly, true);
        qFileDlg.setOption(QFileDialog::HideNameFilterDetails, false);
        qFileDlg.setFileMode(fileMode);
    QStringList memoPath = ListStrToQStringList(histDir);

    qFileDlg.setHistory(memoPath);

    qFileDlg.setSidebarUrls(urls);

    if (qFileDlg.exec())
    {
        strings = QStringListToListStr(qFileDlg.selectedFiles());
    }

    return strings;
}
//-----------------------------------------------------------------------------
String FileDialog::getExistingDirectory  (Core::List<String>& histDir, const String& defaultDir, const Core::List<String>& shortcuts, QWidget * parent, const String & caption)
{
    Core::List<String> strings;

    QList<QUrl> urls;
    QUrl u = QUrl::fromLocalFile(StrToQStr(defaultDir));
    urls << u;
    for (int32 ii=0; ii<shortcuts.size(); ++ii)
    {
        if (Core::System::dirExists(shortcuts[ii]))
        {
            QUrl u = QUrl::fromLocalFile(StrToQStr(shortcuts[ii]));
            urls << u;
        }
    }
    
     

    String dir;

    if (histDir.size()>0)
        dir = histDir.back();
    else
        dir = defaultDir;

    QFileDialog qFileDlg (parent);
        
        qFileDlg.setDirectory(StrToQStr(dir));
        qFileDlg.setFileMode(QFileDialog::Directory);
        qFileDlg.setOption(QFileDialog::ShowDirsOnly, true);
    /*    qFileDlg.setOption(QFileDialog::DontResolveSymlinks, false);
        qFileDlg.setOption(QFileDialog::DontConfirmOverwrite, false);
        qFileDlg.setOption(QFileDialog::DontUseNativeDialog, true);
        qFileDlg.setOption(QFileDialog::ReadOnly, true);
        qFileDlg.setOption(QFileDialog::HideNameFilterDetails, true);*/
        
        
    QStringList memoPath = ListStrToQStringList(histDir);
    qFileDlg.setHistory(memoPath);

    qFileDlg.setSidebarUrls(urls);

    if (qFileDlg.exec())
    {
        strings = QStringListToListStr(qFileDlg.selectedFiles());
    }

    if (strings.size()> 0)  return strings[0];

    return L"";
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

