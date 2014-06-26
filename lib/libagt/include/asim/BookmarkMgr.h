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
  * @file   BookmarkMgr.h
  * @brief  BookmarkMgr class wraps all the functionality related with event matrix bookmarks
  */

#ifndef BookMgr_H
#define BookMgr_H

#include <stdio.h>

#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>
#include <qwidget.h>
#include <q3popupmenu.h>
#include <qinputdialog.h>

#include "agt_syntax.h"
#include "AScrollView.h"
#include "bookmarkImpl.h"

/**
  * This class wraps all the functionality related with event matrix bookmarks.
  *
  * We keep just position and scalling factors (and a description) on each
  * entry. How to save/restore bookmark information is not managed by this
  * class (depends on AGT concrete application).
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class BookmarkMgr : public QObject
{
    Q_OBJECT

    public:

        /**
          * Constructor
          */
        BookmarkMgr (
                        QWidget* parent,          ///< Parent widget
                        Q3PopupMenu* pmenu,        ///< Reference to the popup menu where to add bookmark entries
                        AScrollView* asv = NULL
                     );

        /**
          * Destructor
          */
        ~BookmarkMgr();

        /**
          * Method to add a bookmark in a given point and scalling factor.
          */
        void
        addBookmark (
                        double sx,   ///< horizontal scalling factor
                        double sy,   ///< vertical scalling factor
                        int x,       ///< x axis
                        int y        ///< y axis
                        );
        /**
          * Method to add a bookmark in a given point a scalling factor and a string
          */
        void
        addBookmark (
                      double sx,       ///< horizontal scalling factor
                      double sy,       ///< vertical scalling factor
                      int x,           ///< x axis
                      int y,           ///< y axis
                      QString text     ///< bookmark description
                      );

        /**
          * Import bookmarks from a file
          */
        void importBookmarks();
        void importBookmarks(QFile* file);

        /**
          * Export bookmarks to a file
          */
        void exportBookmarks();
        void exportBookmarks(QFile* file);

        /**
          * Show modal manager dialog
          */
        void
        runDialog();

        /**
          * clean up method
          */
        void reset();

        void setAScrollView (AScrollView* asv);
        AScrollView* getAScrollView();
         
    protected:
        BookContents findOnMap(QString desc,QMap<int, BookContents>* map);
        
    private slots:

        /**
          * This method is triggered when a bookmark is choosen.
          */
        void 
        bookmChosen (
                     int          ///< bookmark index ('vector' position)
                    );

    private:

        QWidget* myParentWidget;
        Q3PopupMenu* myMenu;
        AScrollView* myAsv;

        QStringList bookmarks;
        QMap<int, BookContents> mBookmarks;

        // management dialog
        bookmarkImpl* bmd;


};


#endif
