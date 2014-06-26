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
  * @file  snapDialogImpl.cpp
  */

#include "snapDialogImpl.h"

/*
 *  Constructs a snapDialogImpl which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
snapDialogImpl::snapDialogImpl(QWidget* parent,  const char* name, bool modal, Qt::WFlags fl)
    : Form1( parent, name, modal, fl )
{
    // defaults
    doSnap = true;
    snapSize = 10;

    CheckBoxSnap->setChecked(doSnap);
    LineEditDst->setText(QString::number((int)snapSize));
    SliderDst->setValue(snapSize);

    connect(PushButton1,SIGNAL(clicked()),this,SLOT(acceptClicked()));
    connect(PushButton2,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    connect(LineEditDst,SIGNAL(textChanged(const QString &)),this,SLOT(textChanged(const QString &)));
}

/*
 *  Destroys the object and frees any allocated resources
 */
snapDialogImpl::~snapDialogImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 * public slot
 */
void
snapDialogImpl::Slider1_sliderMoved(int value)
{
    LineEditDst->setText(QString::number((int)value));
}


QSize
snapDialogImpl::sizeHint() const
{
    return QSize(400,150); 
}


bool
snapDialogImpl::snap()
{
    return doSnap;
}

int
snapDialogImpl::getSnapSize()
{
    return snapSize; 
}


void
snapDialogImpl::setSnap(bool v)
{
    doSnap = v;
    CheckBoxSnap->setChecked(doSnap);
}

void
snapDialogImpl::setSnapSize(int v)
{
    snapSize = v;
    LineEditDst->setText(QString::number((int)v));
}

void
snapDialogImpl::acceptClicked()
{
    bool atoi;
    int tmp = LineEditDst->text().toInt(&atoi);
    if ( (!atoi) || (tmp<2) || (tmp>100) )
    {
        // deny ...
        QMessageBox::warning (this,"The introduced text is not a valid",
        "Input error, valid range between 2 and 100",
        Qt::NoButton,Qt::NoButton,Qt::NoButton);

        return;
    }

    doSnap = CheckBoxSnap->isChecked();
    snapSize = tmp;

    // end modal dialog:
    this->done(1);
}

void
snapDialogImpl::cancelClicked()
{
    this->done(0); 
}

void
snapDialogImpl::textChanged ( const QString & str)
{
    //printf ("textChanged called\n");
    bool atoi;
    int tmp = LineEditDst->text().toInt(&atoi);
    if ( (atoi) && (tmp<=100) && (tmp>=2))
    {
        SliderDst->setValue(tmp);
    }
}


