// ==================================================
//Copyright (C) 2003-2006 Intel Corporation
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

/**
  * @file  Q2DMAction.h
  * @brief Derives QAction to get back menu idx numbers 
  */


#ifndef _Q2DMACTION_H
#define _Q2DMACTION_H

// QT Library
#include <q3action.h>
#include <q3popupmenu.h>

class Q2DMAction : public Q3Action
{
	Q_OBJECT
	public:
		Q2DMAction ( QObject * parent, const char * name = 0, bool toggle = FALSE );
		Q2DMAction ( const QString & text, const QIcon & icon, const QString & menuText, QKeySequence accel, QObject * parent, const char * name = 0, bool toggle = FALSE );
		Q2DMAction ( const QString & text, const QString & menuText, QKeySequence accel, QObject * parent, const char * name = 0, bool toggle = FALSE );
		~Q2DMAction ();

		int getMenuId();
		
	protected:
		virtual void addedTo ( int index, Q3PopupMenu * menu );

	private:
		int menuidx;
		Q3PopupMenu* mymenu;
};

#endif

