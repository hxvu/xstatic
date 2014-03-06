
/*
 * XStatic
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


#include <QProcess>
#include <QInputDialog>
#include <QFontDatabase>

#include "fileviewer.h"
#include "mainwindow.h"
#include "CodeEditor.h"
#include "highlighter.h"
#include "fileviewsettingsdialog.h"

#ifdef _WIN32
#define EXT_EDITOR_DEFAULT_PATH "notepad %f"
#else
#define EXT_EDITOR_DEFAULT_PATH "gvim +%n %f"
#endif

filedata::filedata()
{
    linenum = "1";
}
filedata::filedata(const QString& fn, const QString& ln)
{
    linenum = ln;
    filename = fn;
}

bool filedata::compare(const filedata& fd)
{
    return ((linenum.compare(fd.linenum) == 0)&&
            (filename.compare(fd.filename) == 0));
}

bool filedata::compareFilenameOnly(const filedata& fd)
{
    return (filename.compare(fd.filename) == 0);
}

filedata::filedata(const filedata& fd)
{
    linenum = fd.linenum;
    filename = fd.filename;
}

filedata& filedata::operator=(const filedata& fd)
{
    if (&fd != this)
    {
        linenum = fd.linenum;
        filename = fd.filename;
    }
    return *this;
}

fileviewer::fileviewer(mainwindow* pmw)
        :mw(pmw)
        ,m_pushButtonPrev(NULL)
        ,m_pushButtonNext(NULL)
        ,m_pushButtonOpenInEditor(NULL)
        ,m_pushButtonPaste(NULL)
        ,m_pushButtonXRefTo(NULL)
        ,m_pushButtonXRefFrom(NULL)
        ,m_pushButtonGoToLine(NULL)
        ,m_comboBoxSearchPath(NULL)
        ,m_labelFilePath(NULL)
        ,m_textEditSource(NULL)
        ,m_highlighter(NULL)
        ,m_textEditSourceFont("Courier New", 12)
        ,m_externalEditorPath(EXT_EDITOR_DEFAULT_PATH)
        ,m_timestampMismatchWarned(false)
        ,m_file_idx(0)
        //,m_statusbar(NULL)
{
    m_iter = m_fileDataList.begin();
    m_textEditSourceFont.setStyleHint(QFont::TypeWriter);
}

fileviewer::~fileviewer()
{
    if (m_highlighter != NULL) delete m_highlighter;
}

void fileviewer::createFontList(void)
{
    QFontDatabase fontdb;
    QStringList fontlst = fontdb.families(QFontDatabase::Latin);
    QStringList fixedpitch;
    QStringList::iterator it;
    for (it = fontlst.begin(); it != fontlst.end(); it++)
    {
        if (fontdb.isFixedPitch(*it))
        {
            fixedpitch << (*it);
        }
    }
    fixedpitch.sort();
    m_fontlist = fixedpitch;
}

void fileviewer::init(void)
{
    m_pushButtonPaste->setEnabled(false);
    m_pushButtonXRefTo->setEnabled(false);
    m_pushButtonXRefFrom->setEnabled(false);
    m_pushButtonPrev->setEnabled(false);
    m_pushButtonNext->setEnabled(false);
    m_pushButtonTextShrink->setEnabled(false);
    m_pushButtonTextEnlarge->setEnabled(false);
    m_pushButtonGoToLine->setEnabled(false);
    m_pushButtonOpenInEditor->setEnabled(false);
    m_comboBoxSearchPath->clear();
    m_labelFilePath->clear();
    m_textEditSource->clear();
    m_textEditSource->setFont(m_textEditSourceFont);
    m_textEditSource->setWordWrapMode(QTextOption::NoWrap);
    m_textEditSource->setReadOnly(true);
    m_textEditSource->setCenterOnScroll(true);
    createFontList();
    m_highlighter = new Highlighter(m_textEditSource->document());
    connect(m_textEditSource, SIGNAL(copyAvailable(bool)),
            this, SLOT(AbleToCopy(bool)));
    connect(m_pushButtonGoToLine, SIGNAL(clicked(bool)),
            this, SLOT(GoToLine_ButtonClick(bool)));
    connect(m_pushButtonPaste, SIGNAL(clicked(bool)),
            this, SLOT(Paste_ButtonClick(bool)));
    connect(m_pushButtonXRefTo, SIGNAL(clicked(bool)),
            this, SLOT(XRefTo_ButtonClick(bool)));
    connect(m_pushButtonXRefFrom, SIGNAL(clicked(bool)),
            this, SLOT(XRefFrom_ButtonClick(bool)));
    connect(m_pushButtonPrev, SIGNAL(clicked(bool)),
            this, SLOT(Prev_ButtonClick(bool)));
    connect(m_pushButtonNext, SIGNAL(clicked(bool)),
            this, SLOT(Next_ButtonClick(bool)));
    connect(m_pushButtonOpenInEditor, SIGNAL(clicked(bool)),
            this, SLOT(OpenInEditor_ButtonClick(bool)));
    connect(m_pushButtonTextShrink, SIGNAL(clicked(bool)),
            this, SLOT(TextShrink_ButtonClick(bool)));
    connect(m_pushButtonTextEnlarge, SIGNAL(clicked(bool)),
            this, SLOT(TextEnlarge_ButtonClick(bool)));
    // HX
    connect(m_comboBoxSearchPath, SIGNAL(currentIndexChanged(int)),
            this, SLOT(FilePath_indexChanged(int)));
    //
    m_fileDataList.clear();
}

void fileviewer::clearList()
{
    m_pushButtonPaste->setEnabled(false);
    m_pushButtonXRefTo->setEnabled(false);
    m_pushButtonXRefFrom->setEnabled(false);
    m_pushButtonPrev->setEnabled(false);
    m_pushButtonNext->setEnabled(false);
    m_pushButtonTextShrink->setEnabled(false);
    m_pushButtonTextEnlarge->setEnabled(false);
    m_pushButtonGoToLine->setEnabled(false);
    m_pushButtonOpenInEditor->setEnabled(false);
    m_comboBoxSearchPath->clear();
    m_labelFilePath->clear();
    //m_statusBar->showMessage(tr("Ready"), 2000);
    m_textEditSource->clear();
    m_fileDataList.clear();
    m_iter = m_fileDataList.begin();
    m_timestampMismatchWarned = false;
}
void fileviewer::FilePath_indexChanged(const int& idx)
{
    m_iter = m_fileDataList.begin();
    for (int i=0; i < idx; i++)
        m_iter++;
    updateTextEdit();
    checkFileStackButtons();
}
  
void fileviewer::recvDBtimestamp(QDateTime dt)
{
    m_DBtimestamp = dt;
}

void fileviewer::PushFileLocation(filedata &fd)
{
    //printf("List Sz=%d\n",m_fileDataList.size());
    // If my pointer is not at the TOP
    while (!m_fileDataList.isEmpty())
    { 
        if (!m_iter->compare(*m_fileDataList.begin()))
        {
            m_fileDataList.pop_front();
            m_comboBoxSearchPath->removeItem(0);
            //printf("POP\n");
        }
        else
        {
            //printf("Match\n");
            break;
        }
    }
    //printf("PUSH Top\n");
    m_fileDataList.push_front(fd);
    m_iter = m_fileDataList.begin();

    QFile file(m_iter->filename);
    QString labeltext(file.fileName());
    QStringList parts = labeltext.split("/");
    QString lastBit = parts.at(parts.size()-1);

    lastBit += ":";
    lastBit += m_iter->linenum;
    checkFileStackButtons();
    m_comboBoxSearchPath->insertItem(0,lastBit);
    m_file_idx=0;
    m_comboBoxSearchPath->setCurrentIndex(m_file_idx);
    //printf("List Sz=%d\n",m_fileDataList.size());
}

filedata *fileviewer::PopFileLocation()
{
    // If my pointer is not at the TOP
    while (m_fileDataList.isEmpty() && m_iter != m_fileDataList.begin())
    {
        printf("POP\n");
        m_fileDataList.pop_front();
    }
    if  (m_fileDataList.isEmpty())
        m_fileDataList.pop_front();
    m_iter = m_fileDataList.begin();
    checkFileStackButtons();
    return m_iter;
}

void  fileviewer::checkFileStackButtons()
{
    if (m_fileDataList.size() <= 1)
    {   
        m_pushButtonPrev->setEnabled(false);
        m_pushButtonNext->setEnabled(false);
    }
    else if (m_iter == (m_fileDataList.end()-1))
    {
        m_pushButtonPrev->setEnabled(false);
        m_pushButtonNext->setEnabled(true);
    }
    else if (m_iter == (m_fileDataList.begin()))
    {
        m_pushButtonPrev->setEnabled(true);
        m_pushButtonNext->setEnabled(false);
    }
    else
    {
        m_pushButtonPrev->setEnabled(true);
        m_pushButtonNext->setEnabled(true);
    }
}

void fileviewer::fileToBeOpened(QString filename, QString linenum)
{
    filename.replace(QString("$HOME"), QDir::homePath());
    if (!(QFile::exists(filename)))
    {
        //m_comboBoxSearchPath->lineEdit()->setText(tr("File not found"));
        m_labelFilePath->setText(tr("File not found"));
        handleFileCannotBeOpenedCase();
        return;
    }
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //m_comboBoxSearchPath->lineEdit()->setText(tr("File could not be opened"));
        m_labelFilePath->setText(tr("File could not be opened"));
        handleFileCannotBeOpenedCase();
        return;
    }
    file.close();

    QFileInfo fi(filename);
    if ((m_DBtimestamp < fi.lastModified())&&(m_timestampMismatchWarned == false))
    {

        m_timestampMismatchWarned = true;
        QMessageBox msgBox((QWidget*)mw);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(tr("The source file to be viewed is newer than the XStatic database file. You are recommended to manually regenerate the XStatic database file."));
        msgBox.exec();
    }

    filedata fd(filename, linenum);
    if (m_fileDataList.isEmpty())
    {
        PushFileLocation(fd);
        /*
        m_fileDataList.push_back(fd);
        m_iter = m_fileDataList.end() - 1;
        */
        updateTextEdit();
        return;
    }
    else if (m_iter->compare(fd))
    {
        // the same filename and line number
        updateFilePathLabel();
        return;
    }
    else if (m_iter->compareFilenameOnly(fd))
    {
        // same file, different line number
        PushFileLocation(fd);

        /*
        m_fileDataList.push_back(fd);
        m_iter = m_fileDataList.end() - 1;
        */
        m_textEditSource->highlightLine(fd.linenum.toInt());
        updateFilePathLabel();
    }
    else
    {
        // different file
        PushFileLocation(fd);
        /*
        m_fileDataList.push_back(fd);
        m_iter = m_fileDataList.end() - 1;
        */
        updateTextEdit();
    }
/*
    QVector<filedata>::iterator it = m_fileDataList.begin();
    while ((it != m_fileDataList.end() - 1)&&(it != m_fileDataList.end()))
    {
        if (it->compare(fd)) m_fileDataList.erase(it);
        else it++;
    }
    if (m_fileDataList.size() > 20) m_fileDataList.erase(m_fileDataList.begin());
*/
}

void fileviewer::updateTextEdit(void)
{
    if (!m_iter) 
        return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_textEditSource->clear();
    m_highlighter->m_intAddlRulesMode = 0; //reset additional rules mode

    QFile file(m_iter->filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QApplication::restoreOverrideCursor();
        return;
    }
    QTextStream in(&file);

    int lang = enHighlightCPP;

    QRegExp rx1("\\.py$", Qt::CaseInsensitive);
    int pos = rx1.indexIn(m_iter->filename);
    if (pos != -1) lang = enHighlightPython;

    QRegExp rx2("\\.java$", Qt::CaseInsensitive);
    pos = rx2.indexIn(m_iter->filename);
    if (pos != -1) lang = enHighlightJava;

    m_highlighter->m_intLanguage = lang;

    while (!in.atEnd())
    {
        m_textEditSource->insertPlainText(in.readAll());
    }
    m_textEditSource->highlightLine(m_iter->linenum.toInt());
    updateFilePathLabel();
    m_pushButtonGoToLine->setEnabled(true);
    m_pushButtonOpenInEditor->setEnabled(true);
    m_pushButtonTextShrink->setEnabled(true);
    m_pushButtonTextEnlarge->setEnabled(true);
    QApplication::restoreOverrideCursor();
}

void fileviewer::updateFilePathLabel(void)
{
    QString labeltext = m_iter->filename;
    labeltext += ":";
    labeltext += m_iter->linenum;
    // TODO: Update Combo Box
    //m_comboBoxSearchPath->lineEdit()->setText(labeltext);
    m_labelFilePath->setText(labeltext);
    m_pushButtonGoToLine->setEnabled(true);
}

void fileviewer::AbleToCopy(bool copy)
{
    m_pushButtonPaste->setEnabled(copy);
    m_pushButtonXRefTo->setEnabled(copy);
    m_pushButtonXRefFrom->setEnabled(copy);
}

void fileviewer::GoToLine_ButtonClick(bool checked)
{
    if (!checked) m_textEditSource->highlightLine();
}

void fileviewer::Prev_ButtonClick(bool checked)
{
    if (!checked)
    {
        if (m_iter != m_fileDataList.end())
        {
            QVector<filedata>::iterator it = m_iter;
            m_iter++;
            m_file_idx++;
            if (m_iter->compareFilenameOnly(*it))
            {
                // Same file different line
                m_textEditSource->highlightLine(m_iter->linenum.toInt());
                updateFilePathLabel();
            }
            else
            {
                // Different File
                updateTextEdit();
            }
            m_comboBoxSearchPath->setCurrentIndex(m_file_idx);
        }
        checkFileStackButtons();
    }
}

void fileviewer::Next_ButtonClick(bool checked)
{
    if (!checked)
    {
        if (m_iter != m_fileDataList.begin())
        {
            QVector<filedata>::iterator it = m_iter;
            m_iter--;
            m_file_idx--;
            if (m_iter->compareFilenameOnly(*it))
            {
                // Same file different line
                m_textEditSource->highlightLine(m_iter->linenum.toInt());
                updateFilePathLabel();
            }
            else
            {
                // Different File
                updateTextEdit();
            }
            m_comboBoxSearchPath->setCurrentIndex(m_file_idx);
        }
        checkFileStackButtons();
    }
}

void fileviewer::handleFileCannotBeOpenedCase(void)
{
    m_textEditSource->clear();
    m_pushButtonTextShrink->setEnabled(false);
    m_pushButtonTextEnlarge->setEnabled(false);
    m_highlighter->m_intAddlRulesMode = 0; //reset additional rules mode
    m_pushButtonGoToLine->setEnabled(false);
    m_pushButtonNext->setEnabled(false);
    m_pushButtonPrev->setEnabled(m_fileDataList.isEmpty() == false);
    m_iter = m_fileDataList.end();
}


void fileviewer::Paste_ButtonClick(bool checked)
{
    if (!checked)
    {
        m_textEditSource->copy();
        if (m_checkBoxSymbolOnly->isChecked())
            emit searchCopiedTextSymbolOnly();
        else emit searchCopiedText();
    }
}

void fileviewer::XRefTo_ButtonClick(bool checked)
{
    if (!checked)
    {
        m_textEditSource->copy();
        emit searchCopiedTextXRefTo();
    }
}

void fileviewer::XRefFrom_ButtonClick(bool checked)
{
    if (!checked)
    {
        m_textEditSource->copy();
        emit searchCopiedTextXRefFrom();
    }
}

void fileviewer::fileViewSettings_Triggered(bool checked)
{
    cqDialogFileViewSettings cqdg((QWidget*)mw, this, m_fontlist);
    m_fonttemp = m_textEditSourceFont.family();
    m_fontwidthtemp = (m_textEditSource->tabStopWidth() /
                       m_textEditSource->fontMetrics().width(' '));
    cqdg.setCurrentFontType(m_fonttemp);
    cqdg.setTabWidth(m_fontwidthtemp);
    cqdg.setModal(true);
    cqdg.exec();
    if (cqdg.result() == QDialog::Accepted)
    {
        m_textEditSourceFont.setFamily(m_fonttemp);
        m_textEditSource->setFont(m_textEditSourceFont);
        m_textEditSource->setTabStopWidth(m_fontwidthtemp *
                                          m_textEditSource->fontMetrics().width(' '));
        updateTextEdit();
    }
}

void fileviewer::OptionsExtEditor_Triggered(bool checked)
{
    bool ok;
    QString inptext;
    QInputDialog qinp(mw);
    qinp.setCancelButtonText(tr("Cancel"));
    qinp.setOkButtonText(tr("OK"));
    qinp.setInputMode(QInputDialog::TextInput);
    qinp.setWindowTitle(tr("External Editor Configuration"));
    QString exted = tr("Please enter the path and arguments for the external editor. Replace as follows:");
    exted += "\n%f - ";
    exted += tr("for file path");
    exted += "\n%n - ";
    exted += tr("for line number");
    exted += "\n";
    exted += tr("For example:");
#ifdef _WIN32
    exted += "\n\"C:\\Program Files\\Notepad++\\notepad++.exe\" -n%n %f";
#else
    exted += "\ngedit %f +%n";
#endif
    qinp.setLabelText(exted);
    qinp.setTextEchoMode(QLineEdit::Normal);
    qinp.setTextValue(m_externalEditorPath);
    qinp.exec();
    ok = (qinp.result() == QDialog::Accepted);
    inptext = qinp.textValue();
    if (ok && (inptext.isEmpty() == false)) m_externalEditorPath = inptext.trimmed();
}

void fileviewer::OpenInEditor_ButtonClick(bool checked)
{
    QMessageBox msgBox(mw);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    if (!checked)
    {
        QFile file(m_iter->filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            msgBox.setText(tr("File could not be opened!"));
            msgBox.exec();
            return;
        }
        file.close();

        QStringList arguments;
        QString program;
        QRegExp rx("^\"([^\"]+)\" (.*)");
        int pos = rx.indexIn(m_externalEditorPath);
        if (pos != -1)
        {
            program = rx.cap(1);
            arguments = (rx.cap(2)).split(QRegExp("[ ]+"));
        }
        else
        {
            arguments = m_externalEditorPath.split(QRegExp("[ ]+"));
            program = arguments.takeFirst();
        }
        arguments.replaceInStrings(QRegExp("%f"), m_iter->filename);
        arguments.replaceInStrings(QRegExp("%n"), m_iter->linenum);

        if (QProcess::startDetached(program, arguments) == false)
        {
            msgBox.setText(tr("External editor could not be started. Please check Options!"));
            msgBox.exec();
            return;
        }
    }
}

void fileviewer::TextShrink_ButtonClick(bool checked)
{
    if (!checked)
    {
        textSizeChange(0-2);
    }
}

void fileviewer::TextEnlarge_ButtonClick(bool checked)
{
    if (!checked)
    {
        textSizeChange(2);
    }
}

void fileviewer::textSizeChange(int n)
{
    m_fontwidthtemp = (m_textEditSource->tabStopWidth() /
                       m_textEditSource->fontMetrics().width(' '));
    m_textEditSourceFont.setPixelSize(m_textEditSourceFont.pixelSize() + n);
    m_textEditSource->setFont(m_textEditSourceFont);
    m_textEditSource->setTabStopWidth(m_fontwidthtemp *
                                      m_textEditSource->fontMetrics().width(' '));
}

void fileviewer::fontSelectionTemporary(const QString &fonttxt)
{
    m_fonttemp = fonttxt;
}

void fileviewer::tabWidthSelectionTemporary(const QString &width)
{
    m_fontwidthtemp = width.toInt();
}



