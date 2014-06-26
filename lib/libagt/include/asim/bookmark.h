/*
// Copyright (c) 2014, Intel Corporation
//
// All rights reserved.
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met:
// 
// - Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright 
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the distribution.
// - Neither the name of the Intel Corporation nor the names of its 
//   contributors may be used to endorse or promote products derived from 
//   this software without specific prior written permission.
//  
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

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
