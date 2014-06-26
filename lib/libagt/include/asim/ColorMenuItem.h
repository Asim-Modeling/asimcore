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
  * @file   ColorMenuItem.h
  * @brief  Helper class to color selection/apply widgets.
  */
  
#ifndef _COLORMENUITEM_H
#define _COLORMENUITEM_H

#include <qcolor.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qmenudata.h>
#include <q3action.h>

#include "agt_syntax.h"

/**
  * Helper class to color selection/apply widgets.
  *
  * This is a specialization of QCustomMenuItem class.
  * @see http://doc.trolltech.com/3.0/qcustommenuitem.html
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class ColorMenuItem : public Q3Action
{
    public:

    /**
      * Constructor
      */
        ColorMenuItem (
                        QObject *,
                        QColor                ///< Initial color
                      );

    /**
      * Destructor
      */
        virtual
        ~ColorMenuItem(){};

    /**
      * Function description
      */
        virtual void
        paint (
              QPainter * p,                 ///< Painter where to draw on
              const QColorGroup & cg,       ///< Used by QT for skins...
              bool act,                     ///< TRUE if active item
              bool enabled,                 ///< TRUE if enabled item
              int x,                        ///< x coord. for drawing the item     
              int y,                        ///< y coord. for drawing the item
              int w,                        ///< width for drawing the item
              int h                         ///< height for drawing the item
              );

    /**
      * Inherited redefined QT method for managing widget size.
      * @see http://doc.trolltech.com/3.0/qwidget.html
      */
        virtual QSize
        sizeHint ();

    /**
      * Sets the item color
      */
        inline void
        setColor (
                 QColor c                             ///< New item color
                 );

    /**
      * Gets back item color
      */
        inline QColor       /// @return item color
        getColor();

    protected:
        QColor myColor;
};


void
ColorMenuItem::setColor (QColor c)
{
    myColor = c;
}

QColor
ColorMenuItem::getColor()
{
    return myColor;
}

#endif
