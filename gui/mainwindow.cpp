
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


#include "mainwindow.h"
#include "ui_mainWindow.h"
#include "fileviewer.h"
#include "listhandler.h"
#include "searchhandler.h"
#include "langtable.h"
#include "aboutdialog.h"
#include "swver.h"

#include <QInputDialog>
#include <QMessageBox>

mainwindow::mainwindow(QMainWindow *parent, QApplication *app)
        :QMainWindow(parent)
        ,m_app(app)
        ,ui(new Ui::MainWindow)
        ,m_translator(this)
        ,m_fileviewer(NULL)
        ,m_listhandler(NULL)
        ,m_searchhandler(NULL)
        ,m_currentLanguage("English")
{
    ui->setupUi(this);
    init();
}

mainwindow::~mainwindow()
{
    disconnect();
    delete m_fileviewer;
    delete m_listhandler;
    delete m_searchhandler;
    delete ui;
}

void mainwindow::init(void)
{
    m_fileviewer = new fileviewer(this);
    m_listhandler = new listhandler(this);
    m_searchhandler = new searchhandler(this);
    setup_fileviewer();
    setup_listhandler();
    setup_searchhandler();
    connect(m_searchhandler, SIGNAL(searchresults(sqlqueryresultlist, int)),
            m_listhandler, SLOT(populateList(sqlqueryresultlist, int)));
    connect(m_searchhandler, SIGNAL(DBreset()),
            m_fileviewer, SLOT(clearList()));
    connect(m_searchhandler, SIGNAL(sendDBtimestamp(QDateTime)),
            m_fileviewer, SLOT(recvDBtimestamp(QDateTime)));
    connect(m_searchhandler, SIGNAL(DBreset()),
            m_listhandler, SLOT(clearList()));
    connect(m_listhandler, SIGNAL(openFile(QString, QString)),
            m_fileviewer, SLOT(fileToBeOpened(QString, QString)));
    connect(m_listhandler, SIGNAL(listRowNumUpdated(int)),
            m_searchhandler, SLOT(updateListItemRowNum(int)));
    connect(m_fileviewer, SIGNAL(searchCopiedText()),
            m_searchhandler, SLOT(newSearchText()));
    // HX
    connect(m_fileviewer, SIGNAL(searchCopiedTextXRefTo()),
            m_searchhandler, SLOT(newSearchTextXRefTo()));
    connect(m_fileviewer, SIGNAL(searchCopiedTextXRefFrom()),
            m_searchhandler, SLOT(newSearchTextXRefFrom()));

    connect(m_fileviewer, SIGNAL(searchCopiedTextSymbolOnly()),
            m_searchhandler, SLOT(newSearchTextSymbolOnly()));
    connect(m_searchhandler, SIGNAL(updateStatus(const QString&, int)),
            ui->statusBar, SLOT(showMessage(const QString&, int)));
    connect(m_searchhandler, SIGNAL(getResultCurrentListItemSymbolName()),
            m_listhandler, SLOT(requestToProvideResultCurrentListItemSymbolName()));
    connect(m_listhandler, SIGNAL(sendResultCurrentListItemSymbolName(QString)),
            m_searchhandler, SLOT(resultCurrentListItemSymbolName(QString)));
    connect(ui->actionExit, SIGNAL(triggered(bool)),
            this, SLOT(ExitTriggered(bool)));
    connect(ui->actionAbout, SIGNAL(triggered(bool)),
            this, SLOT(AboutTriggered(bool)));
    connect(ui->actionAbout_Qt, SIGNAL(triggered(bool)),
            this, SLOT(AboutQtTriggered(bool)));
    connect(ui->actionLanguage, SIGNAL(triggered(bool)),
            this, SLOT(LanguageTriggered(bool)));
    connect(ui->actionOpenDB, SIGNAL(triggered(bool)),
            m_searchhandler, SLOT(OpenDB_ButtonClick(bool)));
    connect(ui->actionOptionsExtEditor, SIGNAL(triggered(bool)),
            m_fileviewer, SLOT(OptionsExtEditor_Triggered(bool)));
    connect(ui->actionFileViewSettings, SIGNAL(triggered(bool)),
            m_fileviewer, SLOT(fileViewSettings_Triggered(bool)));
    m_app->setQuitOnLastWindowClosed(false);
    connect(m_app, SIGNAL(lastWindowClosed()),
            this, SLOT(prepareToExit()));
    readSettings();
}

void mainwindow::setup_fileviewer(void)
{
    m_fileviewer->m_pushButtonPrev = ui->pushButtonPrev;
    m_fileviewer->m_pushButtonNext = ui->pushButtonNext;
    m_fileviewer->m_pushButtonTextShrink = ui->pushButtonTextShrink;
    m_fileviewer->m_pushButtonTextEnlarge = ui->pushButtonTextEnlarge;
    m_fileviewer->m_pushButtonOpenInEditor = ui->pushButtonOpenInEditor;
    m_fileviewer->m_pushButtonPaste = ui->pushButtonPaste;
    m_fileviewer->m_pushButtonXRefTo = ui->pushButtonXRefTo;
    m_fileviewer->m_pushButtonXRefFrom = ui->pushButtonXRefFrom;
    m_fileviewer->m_pushButtonGoToLine = ui->pushButtonGoToLine;
    m_fileviewer->m_comboBoxSearchPath = ui->comboBoxSearchPath;
    m_fileviewer->m_labelFilePath = ui->labelFilePath;
    m_fileviewer->m_textEditSource = ui->textEditSource;
    m_fileviewer->m_checkBoxSymbolOnly = ui->checkBoxSymbolOnly;
    //m_fileviewer->m_statusbar = ui->statusBar;

    /* So that we can do searches with the textEditSource events */
    m_fileviewer->m_textEditSource->m_fileviewer = m_fileviewer;
    m_fileviewer->init();
}

void mainwindow::setup_listhandler(void)
{
    m_listhandler->m_treeWidgetSearchResults = ui->treeWidgetSearchResults;
    m_listhandler->m_pushButtonUp = ui->pushButtonUp;
    m_listhandler->m_pushButtonDown = ui->pushButtonDown;
    m_listhandler->init();
}

void mainwindow::setup_searchhandler(void)
{
    m_searchhandler->m_pushButtonOpenDB = ui->pushButtonOpenDB;
    m_searchhandler->m_comboBoxDB = ui->comboBoxDB;
    m_searchhandler->m_checkBoxAutoComplete = ui->checkBoxAutoComplete;
    m_searchhandler->m_checkBoxExactMatch = ui->checkBoxExactMatch;
    m_searchhandler->m_statusbar = ui->statusBar;
    // HX
    m_searchhandler->m_pushButtonModelOpenDB = ui->pushButtonModelOpenDB;
    m_searchhandler->m_pushButtonSources = ui->pushButtonSources;
    m_searchhandler->m_pushButtonSinks = ui->pushButtonSinks;
    m_searchhandler->m_comboBoxModelDB = ui->comboBoxModelDB;
    m_searchhandler->m_apiSlider= ui->apiSlider;
    m_searchhandler->m_apiCoverage= ui->apiCoverage;
    m_searchhandler->m_pushButtonFindMW = ui->pushButtonFindMW;
    //
    m_searchhandler->m_pushButtonSearch = ui->pushButtonSearch;
    m_searchhandler->m_pushButtonClipSearch = ui->pushButtonClipSearch;
    m_searchhandler->m_comboBoxSearch = ui->comboBoxSearch;
    m_searchhandler->m_comboBoxSearchPath = ui->comboBoxSearchPath;
    m_searchhandler->m_comboBoxQueryType = ui->comboBoxQueryType;
    m_searchhandler->m_pushButtonSearchPrev = ui->pushButtonSearchPrev;
    m_searchhandler->m_pushButtonSearchNext = ui->pushButtonSearchNext;
    m_searchhandler->m_pushButtonGraph = ui->pushButtonGraph;
    m_searchhandler->m_pushButtonGraphTo = ui->pushButtonGraphTo;
    m_searchhandler->m_pushButtonGraphFrom = ui->pushButtonGraphFrom;
    m_searchhandler->m_checkBoxFilter = ui->checkBoxFilter;
    m_searchhandler->m_comboBoxFilter = ui->comboBoxFilter;
    // HX - More interlinked now
    m_searchhandler->m_listhandler =  m_listhandler;
    m_searchhandler->m_fileviewer = m_fileviewer;
    //
    m_searchhandler->init();
}

void mainwindow::LanguageTriggered(bool checked)
{
    QStringList items(langtable::getLangNameList());
    bool ok;
    int curLangIdx = items.indexOf(m_currentLanguage);
    curLangIdx = curLangIdx != -1 ? curLangIdx : 0;

    QInputDialog qinp(this);
    qinp.setCancelButtonText(tr("Cancel"));
    qinp.setOkButtonText(tr("OK"));
    qinp.setInputMode(QInputDialog::TextInput);
    qinp.setWindowTitle(tr("Language"));
    qinp.setLabelText(tr("Select language:"));
    qinp.setComboBoxEditable(false);
    qinp.setComboBoxItems(items);
    qinp.setTextValue(m_currentLanguage);
    qinp.exec();
    ok = (qinp.result() == QDialog::Accepted);
    QString item = qinp.textValue();
    if (ok && (item.isEmpty() == false))
    {
        m_currentLanguage = item;
        retranslateUi();
    }
}

void mainwindow::retranslateUi(void)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    m_translator.load(langtable::getLangFilePath(m_currentLanguage));
    m_app->installTranslator(&m_translator);
    ui->retranslateUi(this);
    QString langtxt = ui->actionLanguage->text();
    if (m_currentLanguage.compare("English") != 0) langtxt += " (Language)";
    ui->actionLanguage->setText(langtxt);
    m_searchhandler->retranslateUi();
    m_listhandler->retranslateUi();
    QApplication::restoreOverrideCursor();
}

void mainwindow::ExitTriggered(bool checked)
{
    prepareToExit();
}

void mainwindow::AboutQtTriggered(bool checked)
{
    QMessageBox::aboutQt(this);
}

void mainwindow::AboutTriggered(bool checked)
{
    cqDialogAbout cqdg(this);
    cqdg.setModal(true);
    cqdg.exec();
}

void mainwindow::prepareToExit()
{
    writeSettings();
    m_listhandler->prepareToExit();
    m_app->quit();
}

void mainwindow::writeSettings()
{
    QSettings settings("HxView", "XStatic");

    settings.beginGroup("MainWindow");
    settings.setValue("Size", size());
    settings.setValue("Pos", pos());
    settings.setValue("Maximized", isMaximized());
    settings.setValue("AutoComplete", ui->checkBoxAutoComplete->isChecked());
    settings.setValue("ExactMatch", ui->checkBoxExactMatch->isChecked());
    settings.setValue("SymbolOnly", ui->checkBoxSymbolOnly->isChecked());
    settings.setValue("FilterCheckBox", ui->checkBoxFilter->isChecked());
    settings.setValue("QueryType", ui->comboBoxQueryType->currentIndex());
    settings.setValue("LastOpenDB", ui->comboBoxDB->currentIndex());
    settings.setValue("Language", m_currentLanguage);
    settings.setValue("ExtEditorPath", m_fileviewer->m_externalEditorPath);
    settings.setValue("FileViewerFontSize", m_fileviewer->m_textEditSourceFont.pixelSize());
    settings.setValue("FileViewerFontType", m_fileviewer->m_textEditSourceFont.family());
    settings.setValue("FileViewerTabWidth", m_fileviewer->m_textEditSource->tabStopWidth() /
                      m_fileviewer->m_textEditSource->fontMetrics().width(' '));
    settings.endGroup();

    settings.beginWriteArray("OpenDBHistory");
    for (int i=0; i < ui->comboBoxDB->count(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("db", ui->comboBoxDB->itemText(i));
    }
    settings.endArray();

    settings.beginWriteArray("FilterHistory");
    for (int i=0; i < ui->comboBoxFilter->count(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("filter", ui->comboBoxFilter->itemText(i));
    }
    settings.endArray();

    /*settings.beginWriteArray("SearchHistory");
    for (int i=0; i < ui->comboBoxSearch->count(); i++)
    {
    	settings.setArrayIndex(i);
    	settings.setValue("phrase", ui->comboBoxSearch->itemText(i));
    }
    settings.endArray();*/
}

void mainwindow::readSettings()
{
    QSettings settings("HxView", "XStatic");

    int sizee = settings.beginReadArray("OpenDBHistory");
    QStringList dbhist;
    for (int i=0; i < sizee; i++)
    {
        settings.setArrayIndex(i);
        dbhist << settings.value("db").toString();
    }
    settings.endArray();
    if (dbhist.isEmpty() == false) ui->comboBoxDB->addItems(dbhist);

    int sizef = settings.beginReadArray("FilterHistory");
    QStringList filterhist;
    for (int i=0; i < sizef; i++)
    {
        settings.setArrayIndex(i);
        filterhist << settings.value("filter").toString();
    }
    settings.endArray();
    if (filterhist.isEmpty()) filterhist << "*.h" << "*.c" << "src";
    ui->comboBoxFilter->addItems(filterhist);
    ui->comboBoxFilter->setCurrentIndex(0);


    settings.beginGroup("MainWindow");
    resize(settings.value("Size", size()).toSize());
    move(settings.value("Pos", pos()).toPoint());
    if (settings.value("Maximized", false).toBool()) showMaximized();
    else showNormal();
    ui->checkBoxAutoComplete->setChecked(settings.value("AutoComplete", true).toBool());
    ui->checkBoxExactMatch->setChecked(settings.value("ExactMatch", false).toBool());
    ui->checkBoxSymbolOnly->setChecked(settings.value("SymbolOnly", false).toBool());
    ui->checkBoxFilter->setChecked(settings.value("FilterCheckBox", false).toBool());
    ui->comboBoxQueryType->setCurrentIndex(settings.value("QueryType", 0).toInt());
    ui->comboBoxDB->setCurrentIndex(settings.value("LastOpenDB", ui->comboBoxDB->currentIndex()).toInt());
    m_currentLanguage = settings.value("Language", QString("English")).toString();
    retranslateUi();
    m_fileviewer->m_externalEditorPath =
        settings.value("ExtEditorPath", m_fileviewer->m_externalEditorPath).toString();
    m_fileviewer->m_textEditSourceFont.setPixelSize(settings.value("FileViewerFontSize", 12).toInt());
    m_fileviewer->m_textEditSourceFont.setFamily(settings.value("FileViewerFontType", "Courier New").toString());
    m_fileviewer->m_textEditSource->setFont(m_fileviewer->m_textEditSourceFont);
    m_fileviewer->m_textEditSource->setTabStopWidth(settings.value("FileViewerTabWidth", 4).toInt() *
            m_fileviewer->m_textEditSource->fontMetrics().width(' '));
    settings.endGroup();

    /*sizee = settings.beginReadArray("SearchHistory");
    QStringList srchhist;
    for (int i=0; i < sizee; i++)
    {
    	settings.setArrayIndex(i);
    	srchhist << settings.value("phrase").toString();
    }
    settings.endArray();
    if (srchhist.isEmpty() == false)
    {
    	ui->comboBoxSearch->addItems(srchhist);
    	ui->comboBoxSearch->setCurrentIndex(0);
    }*/

}


