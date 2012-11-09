/*
 *Copyright (C) 2003-2006 Intel Corporation
 *
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/****************************************************************************
** Form interface generated from reading ui file 'snapDialog.ui'
**
** Created: Wed May 29 22:09:11 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef FORM1_H
#define FORM1_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>

class Q3VBoxLayout; 
class Q3HBoxLayout; 
class Q3GridLayout; 
class QCheckBox;
class Q3GroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSlider;

class Form1 : public QDialog
{ 
    Q_OBJECT

public:
    Form1( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~Form1();

    Q3GroupBox* GroupBox1;
    QLabel* TextLabel3;
    QLabel* TextLabel1;
    QCheckBox* CheckBoxSnap;
    QLineEdit* LineEditDst;
    QSlider* SliderDst;
    QPushButton* PushButton1;
    QPushButton* PushButton2;


public slots:
    virtual void Slider1_sliderMoved(int);

protected:
    Q3GridLayout* Form1Layout;
    Q3VBoxLayout* Layout14;
    Q3GridLayout* GroupBox1Layout;
    Q3VBoxLayout* Layout13;
    Q3HBoxLayout* Layout12;
    Q3VBoxLayout* Layout8;
    Q3VBoxLayout* Layout11;
    Q3HBoxLayout* Layout10;
    Q3HBoxLayout* Layout4;
};

#endif // FORM1_H
