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
  * @file  AColorDialog.cpp
  */


#include "AColorDialog.h"

// icons
#include "xpm/pdown.xpm"
//Added by qt3to4:
#include <QtGui/QPixmap>
#include <Q3PopupMenu>

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// AColorRect methods
// ------------------------------------------------------------------
// ------------------------------------------------------------------

QSize
AColorRect::sizeHint() const
{
    return QSize(20,5);
}

void
AColorRect::drawContents ( QPainter * p )
{
    if (!noColorFlag)
    {
        p->setBrush(QBrush(myColor));
        p->setPen(QPen(QColor(0,0,0),1));
    }
    else
    {
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(QColor(150,150,150),1));
    }
    p->drawRect(1,0,19,5);
}



// ------------------------------------------------------------------
// ------------------------------------------------------------------
// AColorDialog methods
// ------------------------------------------------------------------
// ------------------------------------------------------------------
QColor
AColorDialog::stdColors[] =
{
        Qt::black,
        Qt::white,
        Qt::darkGray,
        Qt::gray,
        Qt::lightGray,
        Qt::red,
        Qt::green,
        Qt::blue,
        Qt::cyan,
        Qt::magenta,
        Qt::yellow,
        Qt::darkRed,
        Qt::darkGreen,
        Qt::darkBlue,
        Qt::darkCyan,
        Qt::darkMagenta,
        Qt::darkYellow
};

int 
AColorDialog::static_id=0;

AColorDialog::AColorDialog (QColor defColor, QWidget *parent, const char *name, QPixmap* icon) :
Q3HBox( parent, name )
{
    myId = static_id;
    static_id++;

    setMargin(0);
    setSpacing(0);

    // defaults
    lastActivatedItem = -1;
    noColorFlag    = false;
    currentColor = defColor;
    myInterfaceObj = NULL;

    // create std color array
    int i;
    for (i=0;i<NUM_STD_COLORS;i++)
    {
        stdColorItems[i] = new ColorMenuItem(this, stdColors[i]);
        Q_ASSERT(stdColorItems[i]!=NULL);
    }

    // create the buttom for color apply
    Q3VBox* vbox = new Q3VBox(this,"color_v_box");
    Q_ASSERT(vbox!=NULL);
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->setMaximumWidth(20);
    vbox->setMaximumHeight(28);
    bt_app = new AToolButton(vbox);
    Q_ASSERT(bt_app!=NULL);
    bt_app->setAutoRaise(true);

    if (icon!=NULL)
    {
        bt_app->setPixmap(*icon);
    }
    acr = new AColorRect(vbox,currentColor);Q_ASSERT(acr!=NULL);
    connect( bt_app, SIGNAL( clicked() ), this, SLOT( colorApply() ) );

    // create the buttom for menu popup
    downIcon = QPixmap( pdown );
    bt_pop = new AToolButton(this);Q_ASSERT(bt_pop!=NULL);
    bt_pop->setMaximumWidth (16);
    bt_pop->setPixmap(downIcon);
    bt_pop->setAutoRaise(true);
    connect(bt_pop,SIGNAL( pressed() ), this, SLOT( popupShow() ) );

    // and the popup menu
    pop = new Q3PopupMenu(parent,"qt_color_menu");Q_ASSERT(pop!=NULL);
    connect(pop,SIGNAL(activated(int)),this, SLOT( popMenuActivated(int) ) );

    // now put in all the "standard" colors
    noColorId = pop->insertItem("No color");

    for (i=0;i<NUM_STD_COLORS;i++)
    {
        pop->addAction((QAction*)stdColorItems[i]);
        pop->connect(stdColorItems[i], SIGNAL(triggered), this, SLOT(stdColorActivated()));
    }

    moreColorsId = pop->insertItem("More colors...");
}

AColorDialog::~AColorDialog ()
{
    int i;
    for (i=0;i<NUM_STD_COLORS;i++)
    {
        delete stdColorItems[i];
    }

    delete acr;
    delete bt_app;
    delete bt_pop;
    delete pop;
}


void 
AColorDialog::colorApply()
{
    // invoke registered call
    if (myInterfaceObj!=NULL)
    {
        myInterfaceObj->colorApply(currentColor,noColorFlag,myId);
    }
}

void
AColorDialog::popupShow()
{
    pop->exec(mapToGlobal(bt_pop->geometry().bottomLeft()));
    bt_pop->setDown(false);
}

void 
AColorDialog::popMenuActivated (int id)
{
    lastActivatedItem = id;

    // check for "more colors dialog"
    if (id==moreColorsId)
    {
        QColor selColor = QColorDialog::getColor();
        if (selColor.isValid())
        {
            currentColor = selColor;
            noColorFlag = false;
            acr->setNoColor(false);
            acr->setColor(currentColor);
            if (myInterfaceObj!=NULL)
            {
                myInterfaceObj->colorChanged(currentColor,noColorFlag,myId);
            }    
        }
    }
    else if (id==noColorId)
    {
        noColorFlag = true;
        acr->setNoColor(true);
        acr->setColor(currentColor);
        if (myInterfaceObj!=NULL)
        {
            myInterfaceObj->colorChanged(currentColor,noColorFlag,myId);
        }    
    }
}

void 
AColorDialog::stdColorActivated()
{
    int pos = pop->indexOf(lastActivatedItem);
    currentColor = stdColorItems[pos-1]->getColor();
    noColorFlag = false;
    acr->setNoColor(false);
    acr->setColor(currentColor);
    if (myInterfaceObj!=NULL)
        myInterfaceObj->colorChanged(currentColor,noColorFlag,myId);
}

