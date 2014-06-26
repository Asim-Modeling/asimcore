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
  * @file   UpdColorInterface.h
  * @brief  Helper interface for AColorDialog used to register target methods for internal events.
  */
  
#ifndef _UPDCOLORITERFACE_H
#define _UPDCOLORITERFACE_H

#include "agt_syntax.h"

/**
  * @interface UpdColorInterface
  * @brief Helper interface for AColorDialog used to register target methods for internal events.
  *
  * The idea is that you want to be informed when the "default" color changes or the
  * user pressed the "apply" button. Usually the apply means do something with currently
  * selected object.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  *
  * @see AColorDialog
  */
class UpdColorIterface
{
    public:

    /**
      * Triggered method when default color changes.
      */
        virtual void
        colorChanged (
                     QColor color,    ///< the new default color (or undefined if transparent)
                     bool nocolor,    ///< when transparent color is selected this parameter is true
                     int id           ///< Widget ID (pen, brush, text ? ...)
                     ) = 0;

    /**
      * Triggered method when apply button is pressed.
      */
        virtual void
        colorApply (
                   QColor color,           ///< The color to be applied (or undefined if transparent)
                   bool nocolor,           ///< when transparent color is selected this parameter is true
                   int id                  ///< Widget ID (pen, brush, text ? ...)
                   ) = 0;
};

#endif

