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
  * @file  ColumnShadows.h
  * @brief Defines the class AScrollView and the enumeration PointerType.
  */

#ifndef _COLUMNSHADOWS_H
#define _COLUMNSHADOWS_H

// AGT
#include "agt_syntax.h"

// QT
#include <qfile.h>
#include <q3textstream.h>
#include <qcolor.h>
#include <qmessagebox.h>

#define SHD_MAGIC_NUMBER 0xfedefed0

class ColumnShadows
{
    public:

        /**
          * Constructor
          */
        ColumnShadows (
                      int numColumns         ///< Number of columns that can be shaded
                    );
        /**
          * Destructor
          */
        virtual
        ~ColumnShadows();

        virtual void resize(int newsize);

        virtual void shadeColumn(INT32 col,bool shade);
        virtual bool getShadeFlag(INT32 col);

        virtual void clear();
        virtual INT32 countShadowedColumns();

        virtual void saveShadows(QFile* file);
        virtual void loadShadows(QFile* file);

        virtual int getNumCols() { return cols; }
        
    protected:
        int    cols;
        BYTE*  shadeFlagVector;

};
#endif


