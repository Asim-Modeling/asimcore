/*
 * Copyright (c) 2014, Intel Corporation
 *
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright 
 *   notice, this list of conditions and the following disclaimer in the 
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the Intel Corporation nor the names of its 
 *   contributors may be used to endorse or promote products derived from 
 *   this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
   @file BookmarkMgr.h
  */
#include "BookmarkMgr.h"

#include <qmessagebox.h>
#include <q3filedialog.h>
//Added by qt3to4:
#include <Q3TextStream>
#include <Q3PopupMenu>

BookmarkMgr::BookmarkMgr(QWidget* parent,Q3PopupMenu* pmenu,AScrollView* asv)
{
    myMenu = pmenu;
    myParentWidget = parent;
    myAsv = asv;

    bmd = new bookmarkImpl(&bookmarks,parent,"bmd",true);Q_ASSERT(bmd!=NULL);

    connect( pmenu, SIGNAL( activated( int ) ),
         this, SLOT( bookmChosen( int ) ) );

}

BookmarkMgr::~BookmarkMgr()
{
    if (bmd != NULL) delete bmd;
}

void
BookmarkMgr::setAScrollView (AScrollView* asv)
{ myAsv=asv;}

AScrollView*
BookmarkMgr::getAScrollView ()
{ return myAsv; }

void
BookmarkMgr::addBookmark(double sx,double sy, int x, int y)
{
    // get a description for the bookmark
    bool ok = FALSE;
    QString text = QInputDialog::getText(
    "Bookmark management",
    "Please enter a description for the new bookmark:",
    QLineEdit::Normal,
    QString::null, &ok, myParentWidget );

    if (!ok || text.isEmpty())
    {
        return;
    }
    addBookmark(sx,sy,x,y,text);
}

void
BookmarkMgr::addBookmark(double sx,double sy, int x, int y, QString text)
{
    int cnt = (int)bookmarks.contains(text);
    if (cnt>0)
    {
        QMessageBox::critical (NULL,"Bookmark Error",
        "Repeated descriptor not allowed.",
        QMessageBox::Ok,Qt::NoButton,Qt::NoButton);
        return;
    }

    // create the needed struct
    BookContents nb = new BookContentsClass(x,y,sx,sy,text);
    Q_ASSERT(nb!=NULL);

    mBookmarks[myMenu->insertItem(text)]=nb;
    bookmarks.append(text);
}

void
BookmarkMgr::bookmChosen( int i )
{
    if ( mBookmarks.contains( i ) )
    {
        BookContents nb = mBookmarks[i];
        if (myAsv)
            myAsv->putView(nb->getX(),nb->getY(),nb->getSX(),nb->getSY());
    }

}

void BookmarkMgr::runDialog()
{
    bmd->refresh();
    bmd->exec();
    QStringList strListCpy(bookmarks);
    QMap<int, BookContents> mapCpy(mBookmarks);
    reset();

    QStringList::Iterator it;
    for ( it = strListCpy.begin(); it != strListCpy.end(); ++it )
    {
        BookContents bc = findOnMap(*it,&mapCpy);
        if (bc!=NULL)
        {
            mBookmarks[myMenu->insertItem(*it)]=bc;
            bookmarks.append(*it);
        }
    }
}

BookContents
BookmarkMgr::findOnMap(QString desc,QMap<int, BookContents>* map)
{
    bool fnd=false;
    QMap<int, BookContents>::Iterator it = map->begin();
    while (!fnd && (it != map->end()))
    {
        QString str(it.data()->getDesc());
        fnd = (str==desc);
        ++it;
    }
    if (fnd)
    {
        --it;
        return it.data();
    }
    else
    {
        return NULL;
    }
}

void
BookmarkMgr::reset()
{
    bookmarks.clear();

    QMap<int, BookContents>::Iterator it;
    for ( it = mBookmarks.begin(); it != mBookmarks.end(); ++it )
    {
        int menuid = it.key();
        myMenu->removeItem(menuid);
    }

    mBookmarks.clear();
}

void
BookmarkMgr::importBookmarks()
{
    // open file dialog
    QString fileName = Q3FileDialog::getOpenFileName(QString::null,"AGT Bookmarks (*.abk)");
    if (fileName == QString::null)
    {
        return;
    }

    QFile* file = new QFile(fileName);
    bool ok = file->open(QIODevice::ReadOnly);
    if (!ok)
    {
        QMessageBox::critical (NULL,"Import Error",
        "IO Error reading "+fileName,
        QMessageBox::Ok,Qt::NoButton,Qt::NoButton);
        return;
    }
    importBookmarks(file);
    file->close();
    delete file;
}

void
BookmarkMgr::importBookmarks(QFile* file)
{

    reset();
    int cnt;
    unsigned mgc;
    {
    Q3TextStream istream (file);
    istream >> mgc;
    istream >> cnt;
    }

    if ((mgc!=BKM_MAGIC_NUMBER) || (cnt<0) )
    {
        QMessageBox::critical (NULL,"Bookmark Error",
        "Unrecognized format.",
        QMessageBox::Ok,Qt::NoButton,Qt::NoButton);
        return;
    }

    for (int i=0;i<cnt;i++)
    {
        BookContents bc = new BookContentsClass();
        bc->load(file);
        mBookmarks[myMenu->insertItem(bc->getDesc())]=bc;
        bookmarks.append(bc->getDesc());
    }
}

void
BookmarkMgr::exportBookmarks()
{
    // save file dialog
    QString fileName = Q3FileDialog::getSaveFileName(QString::null,"AGT Bookmarks (*.abk)");
    if (fileName == QString::null)
    {
        return;
    }

    QFile* file = new QFile(fileName);
    bool ok = file->open(QIODevice::WriteOnly);
    if (!ok)
    {
        QMessageBox::critical (NULL,"Export Error",
        "IO Error writing "+fileName,
        QMessageBox::Ok,Qt::NoButton,Qt::NoButton);
        return;
    }
    exportBookmarks(file);
    file->close();
    delete file;
}

void
BookmarkMgr::exportBookmarks(QFile* file)
{
    int cnt = bookmarks.count();
    /*
    if (cnt<1)
    {
        QMessageBox::critical (NULL,"Export Error",
        "No bookmarks to export!",
        QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
        return;
    }
    */
    unsigned mgc = BKM_MAGIC_NUMBER;
    {
    Q3TextStream ostream (file);
    ostream << mgc << "\n";
    ostream << cnt << "\n";
    }

    QStringList::Iterator it;
    for ( it = bookmarks.begin(); it != bookmarks.end(); ++it )
    {
        BookContents bc = findOnMap(*it,&mBookmarks);
        Q_ASSERT(bc!=NULL);
        bc->save(file);
    }
}


