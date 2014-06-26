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
  * @file ColumnShadows.cpp
  * @brief
  */

#include "ColumnShadows.h"
//Added by qt3to4:
#include <Q3TextStream>

ColumnShadows::ColumnShadows (int numColumns)
{
    // create shading flag vectors
    int fsz = (numColumns+1)/sizeof(BYTE);
    cols = numColumns;
    shadeFlagVector = new BYTE[fsz];
    Q_ASSERT(shadeFlagVector!=NULL);
    bzero((void*)shadeFlagVector,(size_t)fsz);
}

ColumnShadows::~ColumnShadows()
{
    delete [] shadeFlagVector;
}

void
ColumnShadows::resize(int newsize)
{
    int i;
    BYTE* tmpVector;
    int fsz = (newsize+1)/sizeof(BYTE);
    tmpVector = new BYTE[fsz];
    Q_ASSERT(tmpVector!=NULL);
    bzero((void*)tmpVector,(size_t)fsz);
    int cmin = QMIN(cols,newsize);
    for (i=0;i<cmin;i++)
    {
        if (getShadeFlag(i))
            tmpVector[i/8] |= (1<<(7 - i%8));
    }
    delete [] shadeFlagVector;
    shadeFlagVector = tmpVector;
    cols = newsize;
}

void
ColumnShadows::shadeColumn(INT32 col,bool shade)
{
    if (col >= cols) { return; }

    int v = (int)shade;
    BYTE mask = ~(1<<(7 - col%8));
    shadeFlagVector[col/8] &= mask;
    shadeFlagVector[col/8] |= (v<<(7 - col%8));
}

bool
ColumnShadows::getShadeFlag(INT32 pos)
{ return (shadeFlagVector[pos/8]>>(7 - pos%8) & 1 ); }

void
ColumnShadows::clear()
{
    int fsz = cols/sizeof(BYTE);
    bzero((void*)shadeFlagVector,(size_t)fsz);
}

INT32
ColumnShadows::countShadowedColumns()
{
    INT32 i;
    INT32 acc = 0;
    for (i=0;i<cols;i++)
        if (getShadeFlag(i)) acc++;

    return acc;
}

void
ColumnShadows::saveShadows(QFile* file)
{
    int i;
    int cnt = countShadowedColumns();
    Q3TextStream ostream (file);

    unsigned mgc = SHD_MAGIC_NUMBER;
    ostream << mgc << "\n";
    ostream << cnt << "\n";

    for (i=0;i<cols;i++)
    {
        if (getShadeFlag(i))
        {
            ostream << i << "\n";
        }
    }
}

void
ColumnShadows::loadShadows(QFile* file)
{
    int i,cnt;
    unsigned mgc;
    {
        Q3TextStream istream (file);
        istream >> mgc;
        istream >> cnt;
    }

    if ((mgc!=SHD_MAGIC_NUMBER) || (cnt<0) )
    {
        QMessageBox::critical (NULL,"Shadows Error",
        "Unrecognized format.",
        QMessageBox::Ok,Qt::NoButton,Qt::NoButton);
        return;
    }

    Q3TextStream istream (file);
    int pos;
    for (int i=0;i<cnt;i++)
    {
        istream >> pos;
        shadeColumn(pos,true);
    }
}


