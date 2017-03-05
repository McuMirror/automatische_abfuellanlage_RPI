#######################################################################################
#										      #
# Project:	Diplomarbeit: Autmatische Abfüllanlage				      #
# Host:		Raspberry PI 3B							      #
# Filename:	automatische_abfuellanlage.pro					      #
#										      #
# Developer:	Wögerbauer Stefan						      #
# E-Mail:	woegste@hotmail.com						      #
#										      #
#######################################################################################

QT       += core
QT       += gui			#add librarys for the graphical user interface
QT       += serialport		#add librarys for the serial Port
QT       += bluetooth		#add librarys for Bluetooth


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = automatische_abfuellanlage_RPI
TEMPLATE = app


SOURCES  += main.cpp\
	    mainwindow.cpp \
	    menu_mix.cpp \
	    menu.cpp \
	    dialog_addmix.cpp \
            mixture.cpp \
            bluetoothtransmissionserver.cpp \


HEADERS  += mainwindow.h \
	    menu_mix.h \
	    menu.h \
	    dialog_addmix.h \
            mixture.h \
            bluetoothtransmissionserver.h \

FORMS    += mainwindow.ui \
	    menu_mix.ui \
	    menu.ui \
	    dialog_addmix.ui

RESOURCES += \
	    daten.qrc

DISTFILES += \
            LICENSE.md \

