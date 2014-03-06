
/*
 * CodeQuery
 * Copyright (C) 2013 ruben2020 https://github.com/ruben2020/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef _FILEVIEWER_H
#define _FILEVIEWER_H

#include <QtGui>

class mainwindow;
class CodeEditor;
class Highlighter;

class filedata
{
public:
    QString filename;
    QString linenum;

    filedata();
    filedata(const QString& fn, const QString& ln);
    filedata(const filedata& fd);
    bool compare(const filedata& fd);
    bool compareFilenameOnly(const filedata& fd);
    filedata& operator=(const filedata& fd);
};

class fileviewer : public QObject
{
    Q_OBJECT

public:
    QPushButton *m_pushButtonPrev;
    QPushButton *m_pushButtonNext;
    QPushButton *m_pushButtonOpenInEditor;
    QPushButton *m_pushButtonPaste;
    QPushButton *m_pushButtonXRefTo;
    QPushButton *m_pushButtonXRefFrom;
    QPushButton *m_pushButtonGoToLine;
    QPushButton *m_pushButtonTextShrink;
    QPushButton *m_pushButtonTextEnlarge;
    QCheckBox   *m_checkBoxSymbolOnly;
    QComboBox   *m_comboBoxSearchPath;
    CodeEditor *m_textEditSource;
    QString m_externalEditorPath;
    QFont m_textEditSourceFont;
    //QStatuBar   *m_statusbar;
    QLabel     *m_labelFilePath;

    fileviewer(mainwindow* pmw);
    ~fileviewer();
    void init(void);
    void updateTextEdit(void);
    void updateFilePathLabel(void);
    void handleFileCannotBeOpenedCase(void);

public slots:
    void fileToBeOpened(QString filename, QString linenum);
    void AbleToCopy(bool copy);
    void GoToLine_ButtonClick(bool checked);
    void Paste_ButtonClick(bool checked);
    void XRefTo_ButtonClick(bool checked);
    void XRefFrom_ButtonClick(bool checked);
    void Prev_ButtonClick(bool checked);
    void Next_ButtonClick(bool checked);
    void OpenInEditor_ButtonClick(bool checked);
    void TextShrink_ButtonClick(bool checked);
    void TextEnlarge_ButtonClick(bool checked);
    void OptionsExtEditor_Triggered(bool checked);
    void fileViewSettings_Triggered(bool checked);
    void clearList();
    void recvDBtimestamp(QDateTime dt);
    void fontSelectionTemporary(const QString &fonttxt);
    void tabWidthSelectionTemporary(const QString &width);
    void FilePath_indexChanged(const int& idx);
    // * 
    void PushFileLocation(filedata &fd);
    filedata *PopFileLocation();
    void  checkFileStackButtons();

signals:
    void searchCopiedText();
    void searchCopiedTextXRefTo();
    void searchCopiedTextXRefFrom();
    void searchCopiedTextSymbolOnly();

private:
    mainwindow *mw;
    Highlighter *m_highlighter;
    QVector<filedata> m_fileDataList;
    QVector<filedata>::iterator m_iter;
    QDateTime m_DBtimestamp;
    bool m_timestampMismatchWarned;
    QStringList m_fontlist;
    QString m_fonttemp;
    int m_fontwidthtemp;
    int  m_file_idx;

    void createFontList(void);
    void textSizeChange(int n);

};


#endif
