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

/****************************************************************************
** Form implementation generated from reading ui file 'bookmark.ui'
**
** Created: Thu Jan 9 11:13:39 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "bookmark.h"

#include <qvariant.h>
#include <q3frame.h>
#include <q3groupbox.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>

/* 
 *  Constructs a BookMarkForm which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
BookMarkForm::BookMarkForm( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "BookMarkForm" );
    resize( 576, 333 ); 
    setCaption( trUtf8( "Bookmark Manager" ) );
    BookMarkFormLayout = new Q3GridLayout( this, 1, 1, 11, 6, "BookMarkFormLayout"); 

    BookMarkList = new Q3ListBox( this, "BookMarkList" );
    BookMarkList->setMinimumSize( QSize( 0, 0 ) );

    BookMarkFormLayout->addWidget( BookMarkList, 0, 0 );

    GroupBox1 = new Q3GroupBox( this, "GroupBox1" );
    GroupBox1->setFrameShadow( Q3GroupBox::Sunken );
    GroupBox1->setTitle( trUtf8( "Operations:" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new Q3VBoxLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    QSpacerItem* spacer = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox1Layout->addItem( spacer );

    Layout8 = new Q3VBoxLayout( 0, 0, 6, "Layout8"); 

    Layout2 = new Q3VBoxLayout( 0, 0, 6, "Layout2"); 

    UpButton = new QPushButton( GroupBox1, "UpButton" );
    UpButton->setText( trUtf8( "&Up" ) );
    Layout2->addWidget( UpButton );

    DownButton = new QPushButton( GroupBox1, "DownButton" );
    DownButton->setText( trUtf8( "&Down" ) );
    Layout2->addWidget( DownButton );
    Layout8->addLayout( Layout2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
    Layout8->addItem( spacer_2 );

    RemoveButton = new QPushButton( GroupBox1, "RemoveButton" );
    RemoveButton->setText( trUtf8( "&Remove" ) );
    Layout8->addWidget( RemoveButton );
    GroupBox1Layout->addLayout( Layout8 );
    QSpacerItem* spacer_3 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox1Layout->addItem( spacer_3 );
    QSpacerItem* spacer_4 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox1Layout->addItem( spacer_4 );
    QSpacerItem* spacer_5 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox1Layout->addItem( spacer_5 );
    QSpacerItem* spacer_6 = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox1Layout->addItem( spacer_6 );

    CloseButton = new QPushButton( GroupBox1, "CloseButton" );
    CloseButton->setText( trUtf8( "&Close" ) );
    GroupBox1Layout->addWidget( CloseButton );

    BookMarkFormLayout->addWidget( GroupBox1, 0, 2 );

    Line1 = new Q3Frame( this, "Line1" );
    Line1->setFrameShape( Q3Frame::VLine );
    Line1->setFrameShadow( Q3Frame::Sunken );
    Line1->setFrameShape( Q3Frame::VLine );

    BookMarkFormLayout->addWidget( Line1, 0, 1 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
BookMarkForm::~BookMarkForm()
{
    // no need to delete child widgets, Qt does it all for us
}

