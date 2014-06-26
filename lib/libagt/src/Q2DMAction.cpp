// ==================================================
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

#include "Q2DMAction.h"
//Added by qt3to4:
#include <Q3PopupMenu>

Q2DMAction::Q2DMAction ( QObject * parent, const char * name , bool toggle) : 
			Q3Action (parent,name,toggle) {menuidx=-1;mymenu=NULL;}
			
Q2DMAction::Q2DMAction ( const QString & text, const QIcon & icon, 
			const QString & menuText, QKeySequence accel, QObject * parent, 
			const char * name, bool toggle) : 
			Q3Action (text,icon,menuText,accel,parent,name,toggle) {menuidx=-1;mymenu=NULL;}
			
Q2DMAction::Q2DMAction ( const QString & text, const QString & menuText, 
			QKeySequence accel, QObject * parent, const char * name, bool toggle) :
			Q3Action (text,menuText,accel,parent,name,toggle) {menuidx=-1;mymenu=NULL;}
			
Q2DMAction::~Q2DMAction () {}

int
Q2DMAction::getMenuId()
{
	if (mymenu!=NULL)
	{
		return mymenu->idAt(menuidx);
	}
	else
	{
		return -1;
	}
}

void 
Q2DMAction::addedTo ( int index, Q3PopupMenu * menu )
{
	menuidx = index;
	mymenu=menu;
}
	
