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
  * @file  IconFactory.h
  */

#ifndef _ICONFACTORY_H
#define _ICONFACTORY_H

// QT library
#include <qstring.h>
#include <qicon.h>
#include <q3iconview.h>
#include <qpixmap.h>

/**
  * This class holds DRAL Graph and configuration information.
  *
  * @author Federico Ardanaz
  * @date started at 2004-01-14
  * @version 0.1
  */
class IconFactory 
{
    public:
        static IconFactory* getInstance ();
        static void destroy();

    protected:
        IconFactory();
        virtual ~IconFactory();

    public:
        QPixmap image0;
        QPixmap image1;
        QPixmap image2;
        QPixmap image3;
        QPixmap image4;
        QPixmap image5;
        QPixmap image6;
        QPixmap image7;
        QPixmap image8;
        QPixmap image9;
        QPixmap image10;
        QPixmap image11;
        QPixmap image12;
        QPixmap image13;
        QPixmap image14;
        QPixmap image15;
        QPixmap image16;
        QPixmap image17;
        QPixmap image18;
        QPixmap image19;
        QPixmap image20;
        QPixmap image21;
        QPixmap image22;
        QPixmap image23;
        QPixmap image24;
        QPixmap image25;
        QPixmap image26;
        QPixmap image27;
        QPixmap image28;
        QPixmap image29;
        QPixmap image30;
        QPixmap image31;
        QPixmap image32;
        QPixmap image33;
        QPixmap image34;
        QPixmap image35;
        QPixmap image36;
        QPixmap image37;
        QPixmap image38;
        QPixmap image39;
        QPixmap image40;
        QPixmap image41;

        QPixmap image42;
        QPixmap image43;
        QPixmap image44;
        QPixmap image45;
        QPixmap image46;
        QPixmap image47;

    private:
       static IconFactory* _myInstance;
};



#endif

