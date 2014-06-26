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
  * @file  AnnotationCircle.h
  * @brief Circle annotation tool class.
  */
  
#ifndef _ANNOTATION_CIRCLE__H
#define _ANNOTATION_CIRCLE__H

#include <stdio.h>
#include <math.h>

#include "agt_syntax.h"
#include "AnnotationItem.h"

/**
  * Circle annotation tool class.
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class AnnotationCircle : public AnnotationItem
{
    Q_OBJECT
    public:
    
       /**
        * Function description
        */
        AnnotationCircle(double vx1, double vy1, double vx2, double vy2,AnnotationItem* parent=NULL);
        
       /**
        * Function description
        */
        virtual
        ~AnnotationCircle();

        // abstract methods

       /**
        * Function description
        */
        virtual AnnotationItem*
        clone();
        
       /**
        * Function description
        */
        virtual void 
        clone (
              AnnotationItem*
              );
              
       /**
        * Function description
        */
        virtual void 
        cloneGeometry (
                      AnnotationItem* item
                      );
                      
       /**
        * Function description
        */
        virtual void 
        move (
             double vx,
             double vy,
             double dis
             );

       /**
        * Function description
        */
        virtual void 
        move (
             double x, 
             double y
             );
             
       /**
        * Function description
        */
        virtual void 
        moveNode (
                 double nx, 
                 double ny, 
                 double x, 
                 double y, 
                 double scf_x, 
                 double scf_y
                 );

       /**
        * Function description
        */
        virtual bool 
        innerPoint (
                   double x, 
                   double y,
                   double scf_x,
                   double scf_y
                   );
                   
       /**
        * Function description
        */
        virtual bool 
        innerNodePoint (
                       double x, 
                       double y,
                       double scf_x,
                       double scf_y
                       );

       /**
        * Function description
        */
        virtual void 
        boundingRect (
                     double*,
                     double*,
                     double*,
                     double*
                     ) const;
                     
       /**
        * Function description
        */
        virtual void
        draw (
             QPainter*, 
             double , 
             double 
             );

       /**
        * Function description
        */
        virtual void 
        drawRubber (
                   int px, 
                   int py, 
                   int x, 
                   int y, 
                   QPainter* p
                   );
                   
       /**
        * Function description
        */
        virtual void 
        mouseWorldPressEvent (
                             int vx, 
                             int vy, 
                             double wx, 
                             double wy
                             );

       /**
        * Function description
        */
        virtual void
        mouseWorldReleaseEvent (
                               int vx, 
                               int vy, 
                               double wx, 
                               double wy
                               );
                               
       /**
        * Function description
        */
        virtual void 
        mouseWorldMoveEvent (
                            int vx, 
                            int vy, 
                            double wx, 
                            double wy
                            );

       /**
        * Function description
        */
        virtual void 
        snapToGrid (
                   int gridSize,
                   bool onlyCorner
                   );

    protected:
        double x1,x2,y1,y2;
};

#endif
