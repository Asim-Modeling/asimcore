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
  * @file  HelpWindow.h
  * @brief This is a small HTML-based help viewer with basic navigation functionality.
  */
  
#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <q3mainwindow.h>
#include <q3textbrowser.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>
//Added by qt3to4:
#include <Q3PopupMenu>

#include "agt_syntax.h"

class QComboBox;
class Q3PopupMenu;

/**
  * This is a small HTML-based help viewer with basic navigation functionality.
  * We are able to keep the history and explicit added bookmarks.
  *
  * @todo This is based on a QT-example source, review copyright implications of this !?
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class HelpWindow : public Q3MainWindow
{
    Q_OBJECT
public:
    /**
      * Constructor
      */
    HelpWindow (
               const QString& home_,    ///< Starting "home page"
               const QStringList& path,
               QWidget* parent = 0,
               const char *name=0
               );

    /**
      * Destructor
      */
    ~HelpWindow();

public:
    inline Q3TextBrowser* getTextBrowser() { return browser; }

private slots:

    /**
      * Switches on and off the back arrow
      */
    void
    setBackwardAvailable (
                         bool
                         );

    /**
      *  Switches on and off the forward arrow
      */
    void
    setForwardAvailable (
                        bool
                        );

    /**
      * This method is triggered when for some reason the path
      * text field changes.
      */
    void
    textChanged();

    /**
      * You can keep multiple windows with different view.
      * This method is triggered when a new view must be
      * created.
      */
    void
    newWindow();

    /**
      * This method is triggered when the current view should be printed out.
      */
    void
    print();

    /**
      * This method is triggered when some "old" path is selected from
      * the related combo box widget.
      */
    void
    pathSelected (
                 const QString &
                 );

    /**
      * This method is triggered when some "old" path is selected from
      * the history list.
      */
    void
    histChosen (
               int
               );

    /**
      * This method is triggered when some "old" path is selected from
      * the bookmark list.
      */
    void
    bookmChosen (
                int
                );

    /**
      * Makes current position gets saved as a (help) bookmark.
      */
    void
    addBookmark();

private:

    /**
      * Try to read History from current directory
      */
    void
    readHistory();

    /**
      * Try to read Bookmakrs from current directory
      */
    void
    readBookmarks();

private:
    Q3TextBrowser* browser;
    QComboBox *pathCombo;
    int backwardId, forwardId;
    QString selectedURL;
    QStringList history, bookmarks;
    QMap<int, QString> mHistory, mBookmarks;
    Q3PopupMenu *hist, *bookm;
    QWidget* myParent;

};

#endif

