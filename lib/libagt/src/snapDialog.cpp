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
** Form implementation generated from reading ui file 'snapDialog.ui'
**
** Created: Wed May 29 22:09:13 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "snapDialog.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <q3groupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

/* 
 *  Constructs a Form1 which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Form1::Form1( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Form1" );
    resize( 390, 172 ); 
    setCaption( trUtf8( "Snap to grid" ) );
    Form1Layout = new Q3GridLayout( this, 1, 1, 11, 6, "Form1Layout");

    Layout14 = new Q3VBoxLayout( 0, 0, 6, "Layout14"); 

    GroupBox1 = new Q3GroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "Snap to grid options:" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new Q3GridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout13 = new Q3VBoxLayout( 0, 0, 6, "Layout13"); 
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout13->addItem( spacer );

    Layout12 = new Q3HBoxLayout( 0, 0, 6, "Layout12"); 

    Layout8 = new Q3VBoxLayout( 0, 0, 6, "Layout8"); 

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setText( trUtf8( "Snap to grid:" ) );
    Layout8->addWidget( TextLabel3 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setText( trUtf8( "Grid lines distance: " ) );
    Layout8->addWidget( TextLabel1 );
    Layout12->addLayout( Layout8 );
    QSpacerItem* spacer_2 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout12->addItem( spacer_2 );

    Layout11 = new Q3VBoxLayout( 0, 0, 6, "Layout11"); 

    CheckBoxSnap = new QCheckBox( GroupBox1, "CheckBoxSnap" );
    CheckBoxSnap->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    CheckBoxSnap->setPaletteBackgroundColor( QColor( 192, 192, 192 ) );
    QFont CheckBoxSnap_font(  CheckBoxSnap->font() );
    CheckBoxSnap_font.setPointSize( 1 );
    CheckBoxSnap->setFont( CheckBoxSnap_font ); 
    CheckBoxSnap->setFocusPolicy( Qt::NoFocus );
    CheckBoxSnap->setText( trUtf8( "." ) );
    Layout11->addWidget( CheckBoxSnap );

    Layout10 = new Q3HBoxLayout( 0, 0, 6, "Layout10"); 

    LineEditDst = new QLineEdit( GroupBox1, "LineEditDst" );
    LineEditDst->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, LineEditDst->sizePolicy().hasHeightForWidth() ) );
    LineEditDst->setMaximumSize( QSize( 50, 22 ) );
    LineEditDst->setFrameShape( QLineEdit::LineEditPanel );
    LineEditDst->setFrameShadow( QLineEdit::Sunken );
    Layout10->addWidget( LineEditDst );

    SliderDst = new QSlider( GroupBox1, "SliderDst" );
    SliderDst->setMinValue( 2 );
    SliderDst->setMaxValue( 100 );
    SliderDst->setValue( 10 );
    SliderDst->setOrientation( Qt::Horizontal );
    SliderDst->setTickmarks( QSlider::NoTicks );
    Layout10->addWidget( SliderDst );
    Layout11->addLayout( Layout10 );
    Layout12->addLayout( Layout11 );
    Layout13->addLayout( Layout12 );

    GroupBox1Layout->addLayout( Layout13, 0, 0 );
    Layout14->addWidget( GroupBox1 );

    Layout4 = new Q3HBoxLayout( 0, 0, 6, "Layout4"); 

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setText( trUtf8( "&Accept" ) );
    Layout4->addWidget( PushButton1 );

    PushButton2 = new QPushButton( this, "PushButton2" );
    PushButton2->setText( trUtf8( "&Cancel" ) );
    Layout4->addWidget( PushButton2 );
    QSpacerItem* spacer_3 = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout4->addItem( spacer_3 );
    Layout14->addLayout( Layout4 );

    Form1Layout->addLayout( Layout14, 0, 0 );

    // signals and slots connections
    connect( SliderDst, SIGNAL( sliderMoved(int) ), this, SLOT( Slider1_sliderMoved(int) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Form1::~Form1()
{
    // no need to delete child widgets, Qt does it all for us
}

void Form1::Slider1_sliderMoved(int)
{
    qWarning( "Form1::Slider1_sliderMoved(int): Not implemented yet!" );
}

