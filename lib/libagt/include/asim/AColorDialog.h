// ==================================================
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

/**
  * @file AColorDialog.h 
  * @brief Defines AColorDialog and some helper classes for "Office-like" color selection/apply widgets.
  */
#ifndef _ACOLORDIALOG_H
#define _ACOLORDIALOG_H

#include <qwidget.h>
#include <qtoolbutton.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <q3popupmenu.h>
#include <qstring.h>
#include <qcolordialog.h>
#include <qpixmap.h>
#include <qlabel.h>

#include "UpdColorInterface.h"
#include "ColorMenuItem.h"

/**
  * @enum StdColors
  * Predefined QT colors for Color Dialog
  */
/*typedef enum
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
    Qt::darkYellow,
    NUM_STD_COLORS
} StdColors;
*/

#define NUM_STD_COLORS 18

/**
  * Helper class to keep the current (Color Dialog) color rectangle.
  */
class AColorRect : public QLabel
{
    public:

       /**
        * Function description
        */
        inline
        AColorRect (
                   QWidget* parent,
                   QColor
                   );

       /**
        * Function description
        */
        inline void
        setColor (
                 QColor
                 );

       /**
        * Function description
        */
        inline QColor
        getColor (
                 QColor
                 );

       /**
        * Function description
        */
        virtual QSize
        sizeHint() const;

       /**
        * Function description
        */
        inline void
        setNoColor (
                   bool
                   );

    protected:
       /**
        * Function description
        */
        virtual void
        drawContents (
                     QPainter * p
                     );

    private:
        QColor myColor;
        bool noColorFlag;

};

/**
  * Helper class which is a tool-like button used in the Color Dialog
  */
class AToolButton : public QToolButton
{
    public:
       /**
        * Function description
        */
        AToolButton ( QWidget * parent, const char * name = 0 ):
        QToolButton (parent,name){};

    protected:
       /**
        * Function description
        */
        inline bool uses3D () const { return false; }
};

/**
  * QT ad-hoc widget to mimic color selection/apply widget used in office applications.
  * Put long desc. here...
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  *
  * @see UpdColorInterface
  */
class AColorDialog : public Q3HBox
{
    Q_OBJECT

public:
    /**
     * Function description
     */
    AColorDialog (
                 QColor defColor = Qt::black,
                 QWidget *parent = 0,
                 const char *name = 0,
                 QPixmap* icon=NULL
                 );

    /**
     * Function description
     */
    ~AColorDialog ();

    /**
     * Function description
     */
    inline QColor
    getCurrentColor();

    /**
     * Function description
     */
    inline bool
    getNoColor();

    /**
     * Function description
     */
    inline void
    setEnabled (
               bool
               );

    /**
     * Function description
     */
    inline int
    registerApplyMethod (
                        UpdColorIterface*
                        );

public slots:
    /**
     * Function description
     */
    void
    colorApply ();

    /**
     * Function description
     */
    void
    popupShow  ();

    /**
     * Function description
     */
    void
    popMenuActivated (
                     int id
                     );

    /**
     * Function description
     */
    void
    stdColorActivated();

private:
    static QColor stdColors[];
    static int static_id;


protected:
    AToolButton    *bt_app;
    AToolButton    *bt_pop;
    Q3PopupMenu    *pop;

private:
    ColorMenuItem* stdColorItems[NUM_STD_COLORS];
    int lastActivatedItem;
    QColor currentColor;
    int noColorId;
    int moreColorsId;
    bool noColorFlag;
    UpdColorIterface* myInterfaceObj;
    QPixmap downIcon;
    AColorRect* acr;
    int myId;

};


// ---------------------------------------------------------
// ---------------------------------------------------------
// AColorDialog methods
// ---------------------------------------------------------
// ---------------------------------------------------------
void
AColorDialog::setEnabled(bool v)
{
    bt_app->setEnabled(v);
    //bt_pop->setEnabled(v);
}

bool 
AColorDialog::getNoColor()
{
    return noColorFlag;
}

QColor 
AColorDialog::getCurrentColor()
{
    return currentColor; 
}

int 
AColorDialog::registerApplyMethod(UpdColorIterface* obj)
{
    myInterfaceObj = obj;
    return (myId);
}


// ---------------------------------------------------------
// ---------------------------------------------------------
// AColorRect methods
// ---------------------------------------------------------
// ---------------------------------------------------------
AColorRect::AColorRect(QWidget* parent, QColor c) : QLabel(parent)
{
    myColor = c;
    noColorFlag = false;
}

void 
AColorRect::setColor(QColor c)
{
    myColor = c;
    repaint();
}

QColor
AColorRect::getColor(QColor)
{
    return myColor; 
}

void
AColorRect::setNoColor(bool v)
{
    noColorFlag = v; 
}

#endif


