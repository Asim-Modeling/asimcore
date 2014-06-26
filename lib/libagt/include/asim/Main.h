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
  * @file   Main.h
  * @brief  Main class creates the fundamental application objects and structures.
  */

#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>

#include <q3toolbar.h>
#include <qtoolbutton.h>
#include <q3popupmenu.h>
#include <q3mainwindow.h>
#include <q3intdict.h>
#include <qdatetime.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qlabel.h>
#include <qimage.h>
#include <q3progressdialog.h>
#include <qstylefactory.h>
#include <qaction.h>
#include <qsignalmapper.h>
#include <q3dict.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <q3filedialog.h>
#include <q3textbrowser.h>
#include <q3dockwindow.h>
#include <q3listview.h>
#include <qerrormessage.h>
#include <qworkspace.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QPixmap>

#include "HelpWindow.h"
#include "BookmarkMgr.h"
#include "AColorDialog.h"
#include "snapDialogImpl.h"
#include "Q2DMAction.h"
#include "IconFactory.h"
#include "MDIWindow.h"

#define AGT_MAJOR_VERSION 1
#define AGT_MINOR_VERSION 5
#define STR_AGT_MAJOR_VERSION "1"
#define STR_AGT_MINOR_VERSION "5"

// textual description of most common platform combinations...


#if defined(HOST_LINUX_X86)
    #define STR_PLATFORM "0x86"
    #define STR_OS "Linux"

#elif defined(HOST_LINUX_IA64)
    #define STR_PLATFORM "IA64"
    #define STR_OS "Linux"

#elif defined(HOST_DUNIX)
    #define STR_PLATFORM "Alpha"
    #define STR_OS "Digital/Tru64 Unix"

#elif defined(HOST_FREEBSD)
    #define STR_PLATFORM "?"
    #define STR_OS "FreeBSD Unix"

#elif defined(HOST_FREEBSD_X86)
    #define STR_PLATFORM "0x86"
    #define STR_OS "FreeBSD Unix"

#else
    #define STR_PLATFORM "Unknown" ///< @def STR_PLATFORM by default unknown platform
    #define STR_OS "Unknown"

#endif

typedef Q3PtrList<MDIWindow> MDIList;

/**
  * This class create the fundamental application objects to launch
  * an AGT application (2Dreams, Stripchartviewer, etc).
  *
  * Put long explanation here
  * @version 0.3
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class Main : public Q3MainWindow
{
    Q_OBJECT

public:

    /**
      * Constructor for Main class.
      */
    Main (
         QWidget* parent=0,          ///< Parent Widget
         const char* name=0,         ///< Internal use string name
         Qt::WFlags f=0                  ///< QT Widget flags (stay on top, etc)
         );

    /**
      * Destructor for Main class.
      */
    virtual
    ~Main();

    /**
      * After creation we initialize some internal structures.
      * We need to invoke some pure virtual functions and so
      * this initialization cannot be done in the constuctor.
      * (Template Design Pattern)
      */
    virtual void
    initApplication();

    /**
      * Just return a string with a welcoming string.
      */
    virtual char*                 /// @return A string with a welcoming string.
    getApplicationCaption() = 0;

    /**
      * Gets back the AScrollView object.
      * This object is created after initialization process.
      * @see initApplication()
      */
    virtual AScrollView*          /// @return AScrollView attached object.
    getAScrollViewObject()=0;

    virtual MDIWindow* getActiveMDI();
    virtual AScrollView* getActiveASV();

    inline void setAnnotationToolsEnabled(bool v);
    inline bool getAnnotationToolsEnabled();
    inline void setGenericOpenEnabled(bool v);
    inline bool getGenericOpenEnabled();
    inline void setGenericSaveEnabled(bool v);
    inline bool getGenericSaveEnabled();
    inline void setGenericPrintEnabled(bool v);
    inline bool getGenericPrintEnabled();
    inline void setGenericCloseEnabled(bool v);
    inline bool getGenericCloseEnabled();
    inline void setBookmarkIOEnabled(bool v);
    inline bool getBookmarkIOEnabled();

    inline bool getPanLocked(void);
public slots:

    virtual void do_lockUnlockZoom();
    virtual void do_lockUnlockPan();
    virtual void do_lockUnlockShading();
    virtual void do_lockUnlockHighlight();
    virtual void do_lockUnlockAnnotation();
    
    // --------------------------------------------------------------
    // file menu methods
    // --------------------------------------------------------------

     /**
      * This method is called by the default "open file" toolbar buttom.
      */
    virtual void
    do_genericOpen() = 0;

    /**
      * This method is called by the default "save file" toolbar buttom.
      */
    virtual void
    do_genericSave() = 0;

    /**
      * This method is called by the default "close" toolbar buttom.
      */
    virtual void
    do_genericClose() = 0;

    /**
      * This method is called by the default "print" toolbar buttom.
      */
    virtual void
    do_genericPrint() = 0;

    /**
      * This method is called at exit time.
      */
    virtual void
    do_quit() = 0;


    // --------------------------------------------------------------
    // help menu methods
    // --------------------------------------------------------------

    /**
      * This method is called by the default help action (F1).
      */
    virtual void
    do_help();

    /**
      * This method is called by the default "about agt library" action.
      */
    virtual void
    do_about_agt();

    /**
      * This method is called by the default "about qt library" action.
      */
    virtual void
    do_about_qt();

    virtual void cycle_msg_changed(QString);
    virtual void row_msg_changed(QString);
    virtual void progress_cnt_changed(int);
    virtual void progress_reinit(int);
    virtual void progress_show();
    virtual void progress_hide();
    
    virtual void slotZoomComboActivated( const QString &s );
    virtual void zoomStatusChanged(int, int);    
    // --------------------------------------------------------------
    // edit menu methods
    // --------------------------------------------------------------

    /**
      * This method is called by the default <Edit>Copy action.
      */
    virtual void
    do_copy();

    /**
      * This method is called by the default <Edit>Paste action.
      */
    virtual void
    do_paste();

    /**
      * This method is called by the default <Edit>Cut action.
      */
    virtual void
    do_cut();

    /**
      * This method is called by the default <Edit>Remove action.
      */
    virtual void
    do_remove();

    // --------------------------------------------------------------
    // view menu methods
    // --------------------------------------------------------------

    /**
      * This method is called by the default Zoom In action.
      */
    virtual void
    do_zoomIn();

    /**
      * This method is called by the default Zoom Out action.
      */
    virtual void
    do_zoomOut();

    /**
      * This method is called by the default Zoom In Horizontal action.
      */
    virtual void
    do_zoomInH();

    /**
      * This method is called by the default Zoom Out Horizontal action.
      */
    virtual void
    do_zoomOutH();

    /**
      * This method is called by the default Zoom In Vertical action.
      */
    virtual void
    do_zoomInV();

    /**
      * This method is called by the default Zoom Out Vertical action.
      */
    virtual void
    do_zoomOutV();

    /**
      * This method is called by the default Reset Aspect Ratio action.
      */
    virtual void
    do_resetAspectRatio();

    /**
      * This method is called by the default Zoom 200% action.
      */
    virtual void
    do_z200();

    /**
      * This method is called by the default Zoom 100% action.
      */
    virtual void
    do_z100();

    /**
      * This method is called by the default Zoom 50% action.
      */
    virtual void
    do_z50();

    /**
      * This method is called by the default Zoom 25% action.
      */
    virtual void
    do_z25();

    /**
      * This method is called by the default "Scale to Fit" action.
      */
    virtual void
    do_scaleToFit();

    /**
      * This method is called to switch on and off the visualization of grid lines.
      */
    virtual void
    do_switchShowGridLines();




    // --------------------------------------------------------------
    // tools menu methods
    // --------------------------------------------------------------

    /**
      * This method is called by when "Selection Tool" option is clicked.
      */
    virtual void
    do_selectToolEnabled();

    /**
      * This method is called by when "Magic Wand Tool" option is clicked.
      */
    virtual void
    do_mwandToolEnabled();

    /**
      * This method is called by when "Padding Hand Tool" option is clicked.
      */
    virtual void
    do_paddingToolEnabled();

    /**
      * This method is called by when "Zooming Tool" option is clicked.
      */
    virtual void
    do_zoomingToolEnabled();

    /**
      * This method is called by when "Zooming W Tool" option is clicked.
      */
    virtual void
    do_zoomingWToolEnabled();

    /**
      * This method is called by when "Distance Tool" option is clicked.
      */
    virtual void
    do_distanceToolEnabled();

    /**
      * This method is called by when "Shading Tool" option is clicked.
      */
    virtual void
    do_shadingToolEnabled();

    /**
      * This method is called by when "Annotation (Line) Tool" option is clicked.
      */
    virtual void
    do_annToolLineEnabled();

    /**
      *  This method is called by when "Annotation (LRectangle) Tool" option is clicked.
      */
    virtual void
    do_annToolRectEnabled();

    /**
      *  This method is called by when "Annotation (Circle) Tool" option is clicked.
      */
    virtual void
    do_annToolCircleEnabled();

    /**
      *  This method is called by when "Annotation (Text) Tool" option is clicked.
      */
    virtual void
    do_insertText();




    // --------------------------------------------------------------
    // bookmarks menu methods
    // --------------------------------------------------------------

    /**
      *  This method is called for adding a new bookmark in current position.
      */
    virtual void
    do_addBookmark();

    /**
      * This method launches the bookmark manager dialog (move, remove, etc.)
      */
    virtual void
    do_manageBookmark();

    /**
      * This method resets bookmarks
      */
    virtual void
    do_resetBookmarks();

    virtual void
    do_importBookmarks();

    virtual void
    do_saveBookmarks();

    // --------------------------------------------------------------
    // options menu methods
    // --------------------------------------------------------------

    /**
      *  This method launches the Gridlines & snaping dialog,
      *  you can switch on and off the snap to grid behaviour of
      *  annotation tools and configure the gap between gridlines.
      */
    virtual void
    do_snapDialog();

    /**
      * Apply a given "skin" to QT widgets.
      * @see addStylesMenu()
      */
    virtual void
    do_makeStyle (
                 const QString &style      ///< Skin name (windows, motif, etc.)
                 );


    virtual 
    void status_msg_changed(QString msg);
    
    // --------------------------------------------------------------
    // window menu methods
    // --------------------------------------------------------------

    /**
      * Hide/Show Annotation Toolbar
      */
    virtual void
    do_switchAnnotationsBar();

    /**
      * Hide/Show View Toolbar (zooming, scalling, etc.)
      */
    virtual void
    do_switchViewBar();

    /**
      * Hide/Show Pointer Toolbar (selection, panning, zooming, etc.)
      */
    virtual void
    do_switchToolsBar();

    /**
      * Hide/Show generic file Toolbar (open, save, print)
      */
    virtual void
    do_switchFileBar();

    /**
      * Hide/Show Color Toolbar (text color, pen color & brush color)
      */
    virtual void
    do_switchColorsBar();

    /**
      * This method is called when for any reason the visibility of
      * the Pointer toolbar changes.
      */
    virtual void
    toolsBarVisibilityChanged (
                              bool visible
                              );

    /**
      * This method is called when for any reason the visibility of
      * the file toolbar changes.
      */
    virtual void
    fileBarVisibilityChanged (
                             bool visible
                             );

    /**
      * This method is called when for any reason the visibility of
      * the View (scalling, zooming) toolbar changes.
      */
    virtual void
    viewBarVisibilityChanged (
                             bool visible
                             );

    /**
      * This method is called when for any reason the visibility of
      * the Annotation toolbar changes.
      */
    virtual void
    annotationsBarVisibilityChanged (
                                    bool visible
                                    );

    /**
      * This method is called when for any reason the visibility of
      * the Color (text, brush, etc.) toolbar changes.
      */
    virtual void
    colorsBarVisibilityChanged (
                               bool visible
                               );

protected:
    virtual void
    initStatusBar();
    
    /**
      * Gives a 'welcome' message which is displayed when starting the application.
      * @return 'welcome' message displayed when starting the application.
      */
    virtual QString
    getWelcomeMessage();

    /**
      * Helper method to add Pointer Toolbar to the application.
      */
    virtual void
    addPointerToolBar();

    /**
      * Helper method to add File Toolbar to the application.
      */
    virtual void
    addFileToolBar();

    /**
      * Helper method to add View Toolbar to the application.
      */
    virtual void
    addViewToolBar();

    /**
      * Helper method to add Annotation Toolbar to the application.
      */
    virtual void
    addAnnotationToolBar();

    /**
      * Helper method to add Color Toolbar to the application.
      */
    virtual void
    addColorToolBar();

    virtual void
    addMDIToolBar();
    /**
      * Helper method to add File Menu to the application.
      */
    virtual void
    addFileMenu();

    /**
      * Helper method to add Edit Menu to the application.
      */
    virtual void
    addEditMenu();

    /**
      * Helper method to add View Menu to the application.
      */
    virtual void
    addViewMenu();

    /**
      * Helper method to add Tools Menu to the application.
      */
    virtual void
    addToolsMenu();

    /**
      * Helper method to add Annotation Menu to the application.
      */
    virtual void
    addAnnotationMenu();

    /**
      * Helper method to add Bookmark Menu to the application.
      */
    virtual void
    addBookmarkMenu();

    /**
      * Helper method to add Options Menu to the application.
      */
    virtual void
    addOptionsMenu();

    /**
      * Helper method to add Window Menu to the application.
      */
    virtual void
    addWindowMenu();

    /**
      * Helper method to add Styles (Skins) Menu to the application.
      */
    virtual void
    addStylesMenu();

    /**
      * Helper method to add Help Menu to the application.
      */
    virtual void
    addHelpMenu();

    virtual void
    addZoomDialog(Q3ToolBar* parent);
        
private:
    void create_aboutAgt();
    
protected:
    AScrollView* asv;
    Q3VBox* vb;
    QWorkspace* ws;
    MDIList*   mdiList;

protected:    // misc menu and toolbars objects
    QMessageBox* about;
    QStatusBar* myStatusBar;
    QMenuBar* menu;
    Q3PopupMenu* file;
    Q3PopupMenu* edit;
    Q3PopupMenu* window;
    Q3PopupMenu* window_toolbar;
    Q3PopupMenu* options;
    Q3PopupMenu* view;
    Q3PopupMenu* zfixed;
    Q3PopupMenu* zoneaxe;
    Q3PopupMenu* tools;
    Q3PopupMenu* pointers;
    Q3PopupMenu* annotation;
    Q3PopupMenu* style;
    Q3PopupMenu* bookmark;

    Q3ToolBar* annotationsTools;
    Q3ToolBar* viewTools;
    Q3ToolBar* pointerTools;
    Q3ToolBar* fileTools;
    Q3ToolBar* colorTools;
    Q3ToolBar* mdiTools;

    int win_annotations_bar_id;
    int win_view_bar_id;
    int win_tool_bar_id;
    int win_file_bar_id;
    int win_color_bar_id;

    int view_grid_id;
    int edit_id;
    int copy_id;
    int paste_id;
    int cut_id;
    int remove_id;
    int options_id;
    int close_id;

    int zoomin_id;
    int zoomout_id;
    int zoominh_id;
    int zoominv_id;
    int zoomouth_id;
    int zoomoutv_id;
    int rstartio_id;
    int z200_id;
    int z100_id;
    int z50_id;
    int z25_id;
    int scaletofit_id;
    int showgl_id;
    int zoomaxes_id;
    int scaleto_id;
    
    int addbookmrk_id;
    int mbookmrk_id;
    int sbookmrk_id;
    int ibookmrk_id;

    int pointers_id;

    BookmarkMgr* bkMgr;

    AColorDialog* brushColorDlg;
    AColorDialog* penColorDlg;
    AColorDialog* fontColorDlg;

    bool annotationToolsEnabled;
    bool genericOpenEnabled;
    bool genericSaveEnabled;
    bool genericCloseEnabled;
    bool genericPrintEnabled;

    bool bookmarkIOEnabled;

    bool zoomLocked;
    bool panLocked;
    bool shadingLocked;
    bool highlightLocked;
    bool annotationLocked;
    
protected:    //pixmaps & icons
    QPixmap selectToolIcon;
    QPixmap mwandToolIcon;
    QPixmap paddingToolIcon;
    QPixmap zoomingToolIcon;
    QPixmap distanceToolIcon;
    QPixmap shadingToolIcon;
    QPixmap openFileIcon;
    QPixmap saveFileIcon;
    QPixmap printIcon;
    QPixmap helpContentsIcon;
    QPixmap zoominIcon;
    QPixmap zoomoutIcon;
    QPixmap zhpIcon;
    QPixmap zhmIcon;
    QPixmap zvpIcon;
    QPixmap zvmIcon;
    QPixmap resetartoIcon;
    QPixmap annLineIcon;
    QPixmap annRectIcon;
    QPixmap annCircleIcon;
    QPixmap annTextIcon;
    QPixmap fillColorIcon;
    QPixmap penColorIcon;
    QPixmap textColorIcon;
    QPixmap zoomLockedIcon;
    QPixmap zoomUnlockedIcon;
    QPixmap paddingLockedIcon;
    QPixmap paddingUnlockedIcon;
    QPixmap shadingLockedIcon;
    QPixmap shadingUnlockedIcon;
    QPixmap highlightLockedIcon;
    QPixmap highlightUnlockedIcon;
    QPixmap annotationLockedIcon;
    QPixmap annotationUnlockedIcon;
    
protected: //actions
    Q2DMAction* actionZoomIn;
    Q2DMAction* actionZoomOut;
    Q2DMAction* actionZoomInH;
    Q2DMAction* actionZoomInV;
    Q2DMAction* actionZoomOutH;
    Q2DMAction* actionZoomOutV;
    Q2DMAction* actionResetARatio;
    Q2DMAction* actionSelectPointer;
    Q2DMAction* actionMWandPointer;
    Q2DMAction* actionPanningPointer;
    Q2DMAction* actionZoomingPointer;
    Q2DMAction* actionZoomingWPointer;
    Q2DMAction* actionDistancePointer;
    Q2DMAction* actionShadingPointer;
    Q2DMAction* actionAddBM;

protected: // buttons
    QToolButton* zoomLockBtn;
    QToolButton* panLockBtn;
    QToolButton* shadingLockBtn;
    QToolButton* highlightLockBtn;
    QToolButton* annotationLockBtn;
    
protected:    // for the status bar...
    Q3ProgressBar *progressBar;
    QLabel* currentCycleLbl;
    QLabel* currentRowLbl;
    QComboBox* myZoomCombo;
    
};


void
Main::setAnnotationToolsEnabled(bool v)
{annotationToolsEnabled = v; }

bool
Main::getAnnotationToolsEnabled()
{ return annotationToolsEnabled; }

void
Main::setGenericOpenEnabled(bool v)
{genericOpenEnabled = v; }

bool
Main::getGenericOpenEnabled()
{ return genericOpenEnabled; }

void
Main::setGenericSaveEnabled(bool v)
{genericSaveEnabled = v; }

bool
Main::getGenericSaveEnabled()
{ return genericSaveEnabled; }

void
Main::setGenericPrintEnabled(bool v)
{genericPrintEnabled = v; }

bool
Main::getGenericPrintEnabled()
{ return genericPrintEnabled; }

void
Main::setGenericCloseEnabled(bool v)
{genericCloseEnabled = v; }

bool
Main::getGenericCloseEnabled()
{ return genericCloseEnabled; }

void
Main::setBookmarkIOEnabled(bool v)
{bookmarkIOEnabled = v; }

bool
Main::getBookmarkIOEnabled()
{ return bookmarkIOEnabled; }


bool 
Main::getPanLocked(void)
{ return panLocked; }

/**
  * Function description
  */
int agtMain(Main* m);

#endif

