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
  * @file  AnnotationFactory.cpp
  */

#include "AnnotationFactory.h"

AnnotationItem* AnnotationFactory::createAnnItem(AnnToolType type)
{
    AnnotationItem* result = NULL;
    switch (type)
    {
        case ANN_LINE:
        result = new AnnotationLine(0,0,0,0);
        Q_ASSERT(result!=NULL);
        break;

        case ANN_RECTANGLE:
        result = new AnnotationRect(0,0,0,0);
        Q_ASSERT(result!=NULL);
        break;

        case ANN_CIRCLE:
        result = new AnnotationCircle(0,0,0,0);
        Q_ASSERT(result!=NULL);
        break;

        case ANN_AUTOBEZIER:
        result = new AnnotationAutoBezierArrow(0,0,0,0);
        Q_ASSERT(result!=NULL);
        break;

        case ANN_TEXT:
        result = new AnnotationText(0,0,QString::null);
        Q_ASSERT(result!=NULL);
        break;
    }
    return (result);
}

