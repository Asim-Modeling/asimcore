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
  * @file bookmarkImpl.cpp
  */


#include <string.h>

#include <qapplication.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <q3frame.h>
#include <q3listbox.h>
#include <qregexp.h>
#include <q3textstream.h>

#include "bookmarkImpl.h"

BookContentsClass::BookContentsClass(int _x, int _y, double _sx, double _sy, QString _desc)
{
    x = _x;
    y = _y;
    sx = _sx;
    sy = _sy;
    desc = _desc;
}

BookContentsClass::~BookContentsClass()
{
}

void
BookContentsClass::save(QFile* file)
{
    Q3TextStream ostream(file);
    ostream << repSpaces(desc) << "\n" << x << "\n" << y << "\n" << sx << "\n" << sy << "\n";
}

void
BookContentsClass::load(QFile* file)
{
    Q3TextStream istream(file);
    istream >> desc >> x >> y >> sx >> sy;
    desc.replace(QRegExp("_")," ");
}

QString
BookContentsClass::repSpaces(QString str)
{
    str.replace(QRegExp("\\s"),"_");
    return (str);
}

/*
 *  Constructs a bookmarkImpl which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
bookmarkImpl::bookmarkImpl(QStringList *mBookmarks, QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : BookMarkForm( parent, name, modal, fl )
{
    booklist=mBookmarks;
    // connect all the buttons
    connect ( UpButton, SIGNAL(clicked()), this, SLOT(UpButton_clicked()) );
    connect ( DownButton, SIGNAL(clicked()), this, SLOT(DownButton_clicked()) );
    connect ( RemoveButton, SIGNAL(clicked()), this, SLOT(RemoveButton_clicked()) );
    connect ( CloseButton, SIGNAL(clicked()), this, SLOT(CloseButton_clicked()) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
bookmarkImpl::~bookmarkImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void
bookmarkImpl::refresh()
{
    BookMarkList->clear();

    QStringList::Iterator it;
    for ( it = booklist->begin(); it != booklist->end(); ++it )
    {
        BookMarkList->insertItem(*it);
    }
}

void
bookmarkImpl::UpButton_clicked()
{
    int pos = BookMarkList->currentItem();
    if (pos<0)
    {
        qApp->beep();
        return;
    }

    Q3ListBoxItem* lastSelected = BookMarkList->item(pos);
    if (lastSelected!=NULL)
    {
        if (pos>0)
        {
            QString prevStr = (BookMarkList->item(pos-1))->text();
            BookMarkList->changeItem(lastSelected->text(),pos-1);
            BookMarkList->changeItem(prevStr,pos);
            BookMarkList->setSelected(pos-1,true);
            BookMarkList->setCurrentItem(pos-1);
            BookMarkList->ensureCurrentVisible();
        }
    }
    else
    {
        qApp->beep();
    }
}

void
bookmarkImpl::DownButton_clicked()
{
    int pos = BookMarkList->currentItem();
    if (pos<0)
    {
        qApp->beep();
        return;
    }

    Q3ListBoxItem* lastSelected = BookMarkList->item(pos);
    if (lastSelected!=NULL)
    {
        int pos = BookMarkList->currentItem();
        if (pos< ((int)BookMarkList->count() - 1))
        {
            QString prevStr = (BookMarkList->item(pos+1))->text();
            BookMarkList->changeItem(lastSelected->text(),pos+1);
            BookMarkList->changeItem(prevStr,pos);
            BookMarkList->setSelected(pos+1,true);
            BookMarkList->setCurrentItem(pos+1);
            BookMarkList->ensureCurrentVisible();
        }
    }
    else
    {
        qApp->beep();
    }
}

void
bookmarkImpl::RemoveButton_clicked()
{
    int ci = BookMarkList->currentItem();
    if (ci>=0)
    {
        BookMarkList->removeItem(ci);
    }
}

void
bookmarkImpl::CloseButton_clicked()
{
    // change the string list according to the elements on BookMarkList
    Q3ListBoxItem *current = BookMarkList->firstItem();
    booklist->clear();
    while (current!=NULL)
    {
        booklist->append(current->text());
        current = current->next();
    }    
    done(1);
}


