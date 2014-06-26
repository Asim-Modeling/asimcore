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
  * @file   snapDialogImpl.h
  * @brief  QT dialog for configuring snap-to-grid functionality.
  */
  
#ifndef SNAPDIALOGIMPL_H
#define SNAPDIALOGIMPL_H

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
#include <qmessagebox.h>

#include "agt_syntax.h"
#include "snapDialog.h"


/**
  * This is a QT dialog for configuring snap-to-grid functionality.
  *
  * Basically you can switch on and off the "snap to grid"
  * behaviour of annotation tools and change the gap 
  * between grid lines.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class snapDialogImpl : public Form1
{
    Q_OBJECT

public:
    /**
      * Constructor
      */
    snapDialogImpl (
                   QWidget* parent = 0,      ///< Widget parent
                   const char* name = 0,     ///< Internal name
                   bool modal = FALSE,       ///< modal dialog? (stay on top and block other windows)
                   Qt::WFlags fl = 0             ///< QT Widget flags (stay on top, etc.)
                   );

    /**
      * Destructor
      */
    ~snapDialogImpl();

    /**
      * Query whether snap to grip is enabled or not.
      */
    bool         /// @return TRUE is snap to grid is enabled.
    snap();

    /**
      * Switch on and off snap to grid. 
      */
    void
    setSnap (
            bool v        ///< TRUE is on, FALSE if off
            );

    /**
      * Get back the "gap" between grid lines.
      */
    int              /// @return the "gap" between grid lines.
    getSnapSize();

    /**
      * Set the "gap" between grid lines.
      */
    void
    setSnapSize (
                int size      ///< the "gap" between grid lines (in pixels).
                );

    /**
      * Inherited method redefinition related with QT widget size management.
      * @see http://doc.trolltech.com/3.0/qwidget.html
      */
    virtual QSize
    sizeHint () const;



public slots:
    /**
      * This method is invoked when the Size slider is moved.
      */
    void
    Slider1_sliderMoved (
                        int    ///< New slider value
                        );

    /**
      * This method is invoked when the accept button is clicked.
      */
    void
    acceptClicked();

    /**
      * This method is invoked when cancel button is clicked
      */
    void
    cancelClicked();

    /**
      * This method is invoked when user change the text edit field (for size)
      */
    void
    textChanged (
                const QString &     ///< Current text value
                );

private:
    bool doSnap;
    int  snapSize;

};

#endif // SNAPDIALOGIMPL_H

