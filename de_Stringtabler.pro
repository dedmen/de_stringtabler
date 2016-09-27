#-------------------------------------------------
#
# Project created by QtCreator 2016-09-24T22:29:26
#
#-------------------------------------------------

QT       += core gui xml concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = de_Stringtabler
TEMPLATE = app
win32:CONFIG(debug, debug|release): LIBS += -lqwindowsd -lqtfreetyped -lQt5PlatformSupportd

SOURCES += main.cpp\
        mainwindow.cpp \
    droppablelistwidget.cpp \
    stringtableentrywidget.cpp \
    stringtableentry.cpp \
    stringtableconflictfinder.cpp \
    addentrydialog.cpp \
    stringsyntaxhighlighter.cpp

HEADERS  += mainwindow.h \
    droppablelistwidget.h \
    stringtableentrywidget.h \
    stringtableentry.h \
    stringtableconflictfinder.h \
    addentrydialog.h \
    stringsyntaxhighlighter.h

FORMS    += mainwindow.ui \
    stringtableentrywidget.ui \
    addentrydialog.ui
