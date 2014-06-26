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
  * @file   bookmarkImpl.h
  * @brief  A QT dialog for bookmark management dialog.
  */

#ifndef BOOKMARKIMPL_H
#define BOOKMARKIMPL_H

#include <qstringlist.h>
#include <qfile.h>

#include "agt_syntax.h"
#include "bookmark.h"

#define BKM_MAGIC_NUMBER 0xfedeedef

/**
  * Just a place holder to bookmark entry information.
  */
class BookContentsClass
{
    public:
        BookContentsClass() {};
        BookContentsClass(int _x, int _y, double _sx, double _sy, QString _desc);
        ~BookContentsClass();

        inline int getX() { return x; }
        inline int getY() { return y; }
        inline double getSX() { return sx; }
        inline double getSY() { return sy; }
        inline QString getDesc() { return desc; }

        void save(QFile* file);
        void load(QFile* file);
        
    protected:
        QString repSpaces(QString str);
        
    private:
        int x;                             ///< bookmark X axis
        int y;                             ///< bookmark Y axis
        double sx;                         ///< bookmark X scalling factor
        double sy;                         ///< bookmark Y scalling factor
        QString desc;                      ///< bookmark description
};


/**
  * @typedef BookContents
  * @brief Pointer to BookContentsClass
  */
typedef BookContentsClass* BookContents;



/**
  * A QT dialog for bookmark management dialog.
  *
  * This is intended basically to reorder, remove and rename bookmarks
  *
  * @warning Not fully operative
  * @todo Implement this dialog!
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class bookmarkImpl : public BookMarkForm
{
    Q_OBJECT

    public:
    
        /**
          * Constructor
          */
        bookmarkImpl (
                     QStringList *mBookmarks, 
                     QWidget* parent = 0,   ///< Parent widget
                     const char* name = 0,  ///< Internal name
                     bool modal = FALSE,    ///< Modal if TRUE (block other windows)
                     Qt::WFlags fl = 0          ///< QT Flags (stay on top, etc.)
                     );
    
        /**
          * Destructor
          */
        ~bookmarkImpl();
    
        /**
          * Refresh dialog contents before run
          */
        void refresh();
        
    public slots:
    
        /**
          * Method triggered when "bookmark to up" button is pressed.
          */
        void
        UpButton_clicked();
    
        /**
          * Method triggered when "bookmark to up" button is pressed.
          */
        void
        DownButton_clicked();
    
        /**
          * Method triggered when "remove bookmark" button is pressed.
          */
        void
        RemoveButton_clicked();
    
        /**
          * Method triggered when "close" button is pressed.
          */
        void
        CloseButton_clicked();
    
    private:
        QStringList* booklist;
};

#endif // BOOKMARKIMPL_H
