/*
 *Copyright (C) 2003-2006 Intel Corporation
 *
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/****************************************************************************
** Form interface generated from reading ui file 'bookmark.ui'
**
** Created: Thu Jan 9 11:13:18 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef BOOKMARKFORM_H
#define BOOKMARKFORM_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
class Q3VBoxLayout; 
class Q3HBoxLayout; 
class Q3GridLayout; 
class Q3Frame;
class Q3GroupBox;
class Q3ListBox;
class Q3ListBoxItem;
class QPushButton;

class BookMarkForm : public QDialog
{ 
    Q_OBJECT

public:
    BookMarkForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~BookMarkForm();

    Q3ListBox* BookMarkList;
    Q3GroupBox* GroupBox1;
    QPushButton* UpButton;
    QPushButton* DownButton;
    QPushButton* RemoveButton;
    QPushButton* CloseButton;
    Q3Frame* Line1;


protected:
    Q3GridLayout* BookMarkFormLayout;
    Q3VBoxLayout* GroupBox1Layout;
    Q3VBoxLayout* Layout8;
    Q3VBoxLayout* Layout2;
};

#endif // BOOKMARKFORM_H
