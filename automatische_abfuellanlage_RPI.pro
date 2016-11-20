#-------------------------------------------------
#
# Project created by QtCreator 2016-11-04T17:11:01
#
#-------------------------------------------------

QT       += core
QT       += gui
QT       += serialport
QT       += bluetooth


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = automatische_abfuellanlage_RPI
TEMPLATE = app


SOURCES  += main.cpp\
	    mainwindow.cpp \
	    menu_mix.cpp \
	    menu.cpp \
	    dialog_addmix.cpp \
            mixture.cpp \
            bluetoothtransmissionserver.cpp

HEADERS  += mainwindow.h \
	    menu_mix.h \
	    menu.h \
	    dialog_addmix.h \
            mixture.h \
            bluetoothtransmissionserver.h

FORMS    += mainwindow.ui \
	    menu_mix.ui \
	    menu.ui \
	    dialog_addmix.ui

RESOURCES +=
