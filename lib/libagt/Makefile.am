#
# Copyright (c) 2014, Intel Corporation
#
# All rights reserved.
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are 
# met:
# 
# - Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright 
#   notice, this list of conditions and the following disclaimer in the 
#   documentation and/or other materials provided with the distribution.
# - Neither the name of the Intel Corporation nor the names of its 
#   contributors may be used to endorse or promote products derived from 
#   this software without specific prior written permission.
#  
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

lib_LIBRARIES = libagt.a

libagt_a_SOURCES =  src/AColorDialog.cpp \
        src/Q2DMAction.cpp \
		src/AnnotationItem.cpp \
		src/ColorMenuItem.cpp \
		src/snapDialog.cpp \
		src/AScrollView.cpp \
		src/AnnotationItemDB.cpp \
		src/HelpWindow.cpp \
		src/snapDialogImpl.cpp \
		src/AnnotationCircle.cpp \
		src/AnnotationLine.cpp \
		src/AnnotationText.cpp \
		src/AnnotationContainer.cpp \
		src/AnnotationAutoBezierArrow.cpp \
		src/Main.cpp \
		src/AnnotationCtrl.cpp \
		src/AnnotationRect.cpp \
		src/bookmark.cpp \
		src/AnnotationFactory.cpp \
		src/BookmarkMgr.cpp \
		src/bookmarkImpl.cpp \
		src/ColumnShadows.cpp \
		src/MDIWindow.cpp \
		src/IconFactory.cpp 

nodist_libagt_a_SOURCES = src/AColorDialog_moc.cpp \
			src/AnnotationItem_moc.cpp \
            src/Q2DMAction_moc.cpp \
			src/snapDialog_moc.cpp \
			src/AScrollView_moc.cpp \
			src/HelpWindow_moc.cpp \
			src/snapDialogImpl_moc.cpp \
			src/AnnotationCircle_moc.cpp \
			src/AnnotationLine_moc.cpp \
			src/AnnotationContainer_moc.cpp \
			src/AnnotationText_moc.cpp \
            src/AnnotationAutoBezierArrow_moc.cpp \
			src/Main_moc.cpp \
			src/AnnotationCtrl_moc.cpp \
			src/AnnotationRect_moc.cpp \
			src/bookmark_moc.cpp \
			src/BookmarkMgr_moc.cpp \
			src/MDIWindow_moc.cpp \
			src/bookmarkImpl_moc.cpp 

libagt_a_DIALOGS= src/bookmark.ui \
		src/snapDialog.ui 

src/%_moc.cpp: include/asim/%.h src/%.cpp
	$(QTMOCDIR)/moc $< -o $@ 

# Note: QT .h files produce tons of warning if -Wall is present
# so we redefine the warning flags to an intermediate level:
WARNFLAGS = -ansi -pedantic -W -Wall -Wcast-align -Wformat=2 -Wno-unused -Wno-parentheses -Wno-long-long
AM_CXXFLAGS += -I./include/asim $(QTINCDIR) $(OPTFLAGS) -DQT3_SUPPORT

CLEANFILES = src/*moc* src/*.lo
DIST_SUBDIRS=include
SUBDIRS=include
