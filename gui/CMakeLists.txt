cmake_minimum_required(VERSION 2.8)
project(XStaticGUI)

#ADD_DEFINITIONS( -Wall )


  # with SET() command you can change variables or define new ones
  # here we define XSTATIC_SRCS variable that contains a list of all .cpp files
  # note that we don't need \ at the end of line
  SET( XSTATIC_SRCS
       sqlqueryadv.cpp
       std2qt.cpp
       langtable.cpp
       CodeEditor.cpp
       highlighter.cpp
       fileviewer.cpp
       listhandler.cpp
       searchhandler.cpp
       mainwindow.cpp
       graphdialog.cpp
       aboutdialog.cpp
       fileviewsettingsdialog.cpp
       main_gui.cpp
       winmain.cpp
  )
  
  # another list, this time it includes all header files that should be treated with moc
  SET( XSTATIC_MOC_HDRS
       CodeEditor.h
       highlighter.h
       fileviewer.h
       listhandler.h
       searchhandler.h
       mainwindow.h
       graphdialog.h
       aboutdialog.h
       fileviewsettingsdialog.h
  )
  
  # some .ui files
  SET( XSTATIC_UIS
       ui/mainWindow.ui
       ui/graphDialog.ui
       ui/aboutDialog.ui
       ui/fileViewSettingsDialog.ui
  )
  
  # and finally an resource file
  SET( XSTATIC_RCS
       xstatic.qrc
  )

  # translation files
  SET( XSTATIC_TRANS

translations/xstatic_de.ts
translations/xstatic_en.ts
translations/xstatic_es.ts
translations/xstatic_fr.ts
translations/xstatic_id.ts
translations/xstatic_it.ts
translations/xstatic_ja.ts
translations/xstatic_ko.ts
translations/xstatic_zh-CHS.ts
translations/xstatic_zh-CHT.ts

  )  

  add_definitions( -DNO_QDESIGNER_WIDGET_EXPORT )
  
  # by default only QtCore and QtGui modules are enabled
  # other modules must be enabled like this:
  #SET( QT_USE_QTXML TRUE )
  #SET( QT_USE_QTSQL TRUE )
  
  # this command finds Qt4 libraries and sets all required variables
  # note that it's Qt4, not QT4 or qt4
  FIND_PACKAGE( Qt4 REQUIRED )
  FIND_PACKAGE( Sqlite REQUIRED)
  FIND_PACKAGE( Lemon REQUIRED)
  
  # add some useful macros and variables
  # (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
  INCLUDE( ${QT_USE_FILE} ${LEMON_INCLUDE_DIR} )

  QT4_ADD_TRANSLATION( QM ${XSTATIC_TRANS} )  
  
  # this command will generate rules that will run rcc on all files from XSTATIC_RCS
  # in result XSTATIC_RC_SRCS variable will contain paths to files produced by rcc
  # QT4_ADD_RESOURCES( XSTATIC_RC_SRCS ${XSTATIC_RCS} )


  SET( XSTATIC_RC_NAME xstatic )
  SET( XSTATIC_RC_QRC ${CMAKE_CURRENT_SOURCE_DIR}/${XSTATIC_RC_NAME}.qrc)
  SET( XSTATIC_RC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/qrc_${XSTATIC_RC_NAME}.cxx)

  # Run the resource compiler (rcc_options should already be set). We can't
  # use QT4_ADD_RESOURCES because the qrc file may not exist yet.
  ADD_CUSTOM_COMMAND(
    OUTPUT ${XSTATIC_RC_SRCS}
    COMMAND ${QT_RCC_EXECUTABLE}
    ARGS ${rcc_options} -name ${XSTATIC_RC_NAME} -o ${XSTATIC_RC_SRCS} ${XSTATIC_RC_QRC}
    DEPENDS ${QM}
    )

  # this will run uic on .ui files:
  QT4_WRAP_UI( XSTATIC_UI_HDRS ${XSTATIC_UIS} )
  
  # and finally this will run moc:
  QT4_WRAP_CPP( XSTATIC_MOC_SRCS ${XSTATIC_MOC_HDRS} )

  # we need this to be able to include headers produced by uic in our code
  # (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
  INCLUDE_DIRECTORIES( ${SQLITE_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${LEMON_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
  INCLUDE_DIRECTORIES( "." )
  INCLUDE_DIRECTORIES( "./translations" )
  INCLUDE_DIRECTORIES( "../querylib" )
  INCLUDE_DIRECTORIES( "../makedb" )
  INCLUDE_DIRECTORIES( "../showgraph" )

if(WIN32)
  SET(CQ_WIN_RCS cqwin32.rc)
  add_executable( xstatic WIN32 ${XSTATIC_SRCS} ${XSTATIC_MOC_SRCS} ${XSTATIC_RC_SRCS} ${XSTATIC_UI_HDRS} ${QM} ${CQ_WIN_RCS} )
else()
  add_executable( xstatic ${XSTATIC_SRCS} ${XSTATIC_MOC_SRCS} ${XSTATIC_RC_SRCS} ${XSTATIC_UI_HDRS} ${QM} )
endif()

  target_link_libraries( xstatic ${QT_LIBRARIES} ${SQLITE_LIBRARIES} ${LEMON_LIBRARIES} sqlquery_lib small_lib cqshowgraph)
  
  install(TARGETS xstatic RUNTIME DESTINATION bin)


