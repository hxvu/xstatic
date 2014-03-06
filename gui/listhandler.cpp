
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

#include "std2qt.h"
#include "listhandler.h"

listhandler::listhandler(mainwindow* pmw)
        :mw(pmw)
        ,m_treeWidgetSearchResults(NULL)
        ,m_pushButtonUp(NULL)
        ,m_pushButtonDown(NULL)
        ,m_noclick(false)
{
}

listhandler::~listhandler()
{
    disconnect();
}

void listhandler::prepareToExit(void)
{
    disconnect();
    m_noclick = true;
    for (int i=0; i<m_itemlist.size(); i++) delete m_itemlist[i];
    m_itemlist.clear();
    m_treeWidgetSearchResults->clear();
    m_sqlist.resultlist.clear();
}

void listhandler::init(void)
{
    connect(m_treeWidgetSearchResults, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(listItemClicked(QTreeWidgetItem *, QTreeWidgetItem *)));
    connect(m_pushButtonUp, SIGNAL(clicked(bool)),
            this, SLOT(Up_ButtonClick(bool)));
    connect(m_pushButtonDown, SIGNAL(clicked(bool)),
            this, SLOT(Down_ButtonClick(bool)));
    m_pushButtonUp->setEnabled(false);
    m_pushButtonDown->setEnabled(false);
    m_treeWidgetSearchResults->setSelectionMode(QAbstractItemView::SingleSelection);
    m_sqlist.resultlist.clear();
    m_sqlist.result_type = sqlqueryresultlist::sqlresultFULL;
    m_noclick = true;
    updateListHeaders();
    m_noclick = false;
    m_sqlist.result_type = sqlqueryresultlist::sqlresultERROR;
}

void listhandler::populateList(sqlqueryresultlist resultlist, int selectitem)
{
    bool headersChanged = (m_sqlist.result_type != resultlist.result_type);
    m_sqlist = resultlist;
    m_noclick = true;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    clearList();
    if (headersChanged) updateListHeaders();
    updateList();
    resizeColumns();
    QApplication::restoreOverrideCursor();
    m_noclick = false;
    m_treeWidgetSearchResults->setCurrentItem(m_treeWidgetSearchResults->topLevelItem(selectitem));
    emit listRowNumUpdated(selectitem);
    checkUpDown();
}

void listhandler::listItemClicked(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    if (current == NULL) return;
    if (m_noclick) return;
    emit listRowNumUpdated(m_treeWidgetSearchResults->indexOfTopLevelItem(current));
    emit openFile(str2qt(m_sqlist.resultlist[current->data(0,Qt::UserRole).toInt()].filepath),
                  str2qt(m_sqlist.resultlist[current->data(0,Qt::UserRole).toInt()].linenum));
    checkUpDown();
}

void listhandler::requestToProvideResultCurrentListItemSymbolName()
{
    QString symName("");
    if (m_treeWidgetSearchResults->topLevelItemCount() > 0)
    {
        QTreeWidgetItem* current = m_treeWidgetSearchResults->currentItem();
        symName = str2qt(m_sqlist.resultlist[current->data(0,Qt::UserRole).toInt()].symname);
    }
    emit sendResultCurrentListItemSymbolName(symName);
}

void listhandler::updateList(void)
{
    //if (m_noclick) return;
    if (m_sqlist.resultlist.empty()) return;

    unsigned int n = m_sqlist.resultlist.size();
    QStringList strList;
    //printf("Updating list N=%d\n",n);
    for (unsigned int i=0; i < n; i++)
    {
        strList.clear();
        if (m_sqlist.result_type == sqlqueryresultlist::sqlresultFULL)
        {
            strList += str2qt(m_sqlist.resultlist[i].symid);
            strList += str2qt(m_sqlist.resultlist[i].symname);
        }
        strList += str2qt(m_sqlist.resultlist[i].filename);
        if ((m_sqlist.result_type == sqlqueryresultlist::sqlresultFULL)||
                (m_sqlist.result_type == sqlqueryresultlist::sqlresultFILE_LINE))
            strList << str2qt(m_sqlist.resultlist[i].linenum) << str2qt(m_sqlist.resultlist[i].linetext);
        m_itemlist += new QTreeWidgetItem(m_treeWidgetSearchResults, strList);
        m_itemlist[i]->setData(0, Qt::UserRole, QVariant(i));
    }
    m_treeWidgetSearchResults->addTopLevelItems(m_itemlist);
}

void listhandler::updateListHeaders(void)
{
    QStringList headers;
    if (m_sqlist.result_type == sqlqueryresultlist::sqlresultFULL)
    {
        headers += tr("ID");
        headers += tr("Symbol");
    }
    headers += tr("File");
    if ((m_sqlist.result_type == sqlqueryresultlist::sqlresultFULL)||
            (m_sqlist.result_type == sqlqueryresultlist::sqlresultFILE_LINE))
        headers << tr("Line") << tr("Preview");
    m_treeWidgetSearchResults->setHeaderLabels(headers);
    if (m_sqlist.result_type == sqlqueryresultlist::sqlresultFULL)
    {
        m_treeWidgetSearchResults->setColumnCount(5);
    }
    else if (m_sqlist.result_type == sqlqueryresultlist::sqlresultFILE_LINE)
    {
        m_treeWidgetSearchResults->setColumnCount(3);
    }
    else if (m_sqlist.result_type == sqlqueryresultlist::sqlresultFILE_ONLY)
    {
        m_treeWidgetSearchResults->setColumnCount(1);
    }
}

void listhandler::resizeColumns(void)
{
    int n = m_treeWidgetSearchResults->columnCount();
    for (int i=0; i < n; i++)
    {
        m_treeWidgetSearchResults->resizeColumnToContents(i);
    }
}

void listhandler::clearList()
{
    bool noclick = m_noclick;
    m_pushButtonUp->setEnabled(false);
    m_pushButtonDown->setEnabled(false);
    m_noclick = true;
    int n = m_itemlist.size();
    for (int i=0; i<n; i++) delete m_itemlist[i];
    m_itemlist.clear();
    m_treeWidgetSearchResults->clear();
    m_noclick = noclick;
}

void listhandler::retranslateUi(void)
{
    m_noclick = true;
    if (m_treeWidgetSearchResults->topLevelItemCount() <= 0)
    {
        m_sqlist.result_type = sqlqueryresultlist::sqlresultFULL;
        updateListHeaders();
        m_noclick = false;
    }
    else
    {
        int curItemIdx = m_treeWidgetSearchResults->indexOfTopLevelItem(m_treeWidgetSearchResults->currentItem());
        clearList();
        updateListHeaders();
        updateList();
        resizeColumns();
        m_treeWidgetSearchResults->setCurrentItem(m_treeWidgetSearchResults->topLevelItem(curItemIdx));
        m_noclick = false;
        listItemClicked(m_treeWidgetSearchResults->topLevelItem(curItemIdx), NULL);
    }
}

void listhandler::Up_ButtonClick(bool checked)
{
    QTreeWidgetItem *item = NULL;
    tStr curFilepath;
    tStr itemFilepath;
    if ((!checked)&&(m_treeWidgetSearchResults->topLevelItemCount() > 0))
    {
        item = m_treeWidgetSearchResults->currentItem();
        curFilepath = m_sqlist.resultlist[item->data(0,Qt::UserRole).toInt()].filepath;
        while (item != NULL)
        {
            item = m_treeWidgetSearchResults->itemAbove(item);
            if (item == NULL) break;
            itemFilepath = m_sqlist.resultlist[item->data(0,Qt::UserRole).toInt()].filepath;
            if (strrevcmp(curFilepath, itemFilepath))
            {
                m_treeWidgetSearchResults->setCurrentItem(item);
                break;
            }
        }
    }
}

void listhandler::Down_ButtonClick(bool checked)
{
    QTreeWidgetItem *item = NULL;
    tStr curFilepath;
    tStr itemFilepath;
    if ((!checked)&&(m_treeWidgetSearchResults->topLevelItemCount() > 0))
    {
        item = m_treeWidgetSearchResults->currentItem();
        curFilepath = m_sqlist.resultlist[item->data(0,Qt::UserRole).toInt()].filepath;
        while (item != NULL)
        {
            item = m_treeWidgetSearchResults->itemBelow(item);
            if (item == NULL) break;
            itemFilepath = m_sqlist.resultlist[item->data(0,Qt::UserRole).toInt()].filepath;
            if (strrevcmp(curFilepath, itemFilepath))
            {
                m_treeWidgetSearchResults->setCurrentItem(item);
                break;
            }
        }
    }
}

void listhandler::checkUpDown(void)
{
    QTreeWidgetItem *item = NULL;
    QTreeWidgetItem *curItem = NULL;
    tStr curFilepath, itemFilepath;
    bool upArrow=false, downArrow=false;
    if (m_treeWidgetSearchResults->topLevelItemCount() <= 0) return;
    item = m_treeWidgetSearchResults->currentItem();
    curItem = item;
    curFilepath = m_sqlist.resultlist[item->data(0,Qt::UserRole).toInt()].filepath;

    while (item != NULL)
    {
        item = m_treeWidgetSearchResults->itemAbove(item);
        if (item == NULL) break;
        itemFilepath = m_sqlist.resultlist[item->data(0,Qt::UserRole).toInt()].filepath;
        if (strrevcmp(curFilepath, itemFilepath))
        {
            upArrow = true;
            break;
        }

        // for now, assume items with same file path are grouped together
        // This is to speed up the search
        else break;
    }

    item = curItem;
    while (item != NULL)
    {
        item = m_treeWidgetSearchResults->itemBelow(item);
        if (item == NULL) break;
        itemFilepath = m_sqlist.resultlist[item->data(0,Qt::UserRole).toInt()].filepath;
        if (strrevcmp(curFilepath, itemFilepath))
        {
            downArrow = true;
            break;
        }

        // for now, assume items with same file path are grouped together
        // This is to speed up the search
        else break;
    }

    m_pushButtonUp->setEnabled(upArrow);
    m_pushButtonDown->setEnabled(downArrow);
}

