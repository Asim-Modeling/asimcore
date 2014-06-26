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
  * @file IconFactory.cpp
  */

#include "IconFactory.h"
#include "icons.cpp"
//Added by qt3to4:
#include <QPixmap>

IconFactory* IconFactory::_myInstance = NULL;

IconFactory*
IconFactory::getInstance()
{
    if (_myInstance==NULL)
        _myInstance = new IconFactory();

    Q_ASSERT(_myInstance!=NULL);
    return _myInstance;
}

void
IconFactory::destroy()
{
    if (_myInstance!=NULL) { delete _myInstance; }
}

IconFactory::IconFactory()
{
    image0 = QPixmap(image0_data);
    image1 = QPixmap(image1_data);
    image2 = QPixmap(image2_data);
    image3 = QPixmap(image3_data);
    image4 = QPixmap(image4_data);
    image5 = QPixmap(image5_data);
    image6 = QPixmap(image6_data);
    image7 = QPixmap(image7_data);
    image8 = QPixmap(image8_data);
    image9 = QPixmap(image9_data);
    image10 = QPixmap(image10_data);
    image11 = QPixmap(image11_data);
    image12 = QPixmap(image12_data);
    image13 = QPixmap(image13_data);
    image14 = QPixmap(image14_data);
    image15 = QPixmap(image15_data);
    image16 = QPixmap(image16_data);
    image17 = QPixmap(image17_data);
    image18 = QPixmap(image18_data);
    image19 = QPixmap(image19_data);
    image20 = QPixmap(image20_data);
    image21 = QPixmap(image21_data);
    image22 = QPixmap(image22_data);
    image23 = QPixmap(image23_data);
    image24 = QPixmap(image24_data);
    image25 = QPixmap(image25_data);
    image26 = QPixmap(image26_data);
    image27 = QPixmap(image27_data);
    image28 = QPixmap(image28_data);
    image29 = QPixmap(image29_data);
    image30 = QPixmap(image30_data);
    image31 = QPixmap(image31_data);
    image32 = QPixmap(image32_data);
    image33 = QPixmap(image33_data);
    image34 = QPixmap(image34_data);
    image35 = QPixmap(image35_data);
    image36 = QPixmap(image36_data);
    image37 = QPixmap(image37_data);
    image38 = QPixmap(image38_data);
    image39 = QPixmap(image39_data);
    image40 = QPixmap(image40_data);
    image41 = QPixmap(image41_data);

    image42 = QPixmap(image42_data);
    image43 = QPixmap(image43_data);
    image44 = QPixmap(image44_data);
    image45 = QPixmap(image45_data);
    image46 = QPixmap(image46_data);
    image47 = QPixmap(image47_data);
}

IconFactory::~IconFactory()
{
}


