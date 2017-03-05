/*
 *#####################################################################################
 * Project:		Diplomarbeit: Autmatische Abfüllanlage								  #
 * Host:		Raspberry PI 3B														  #
 * Filename:	main.cpp															  #
 *																					  #
 * Developer:	Wögerbauer Stefan													  #
 * E-Mail:		woegste@hotmail.com													  #
 *#####################################################################################
 */


//********************** INCLUDES *****************************************************
#include "mainwindow.h"
#include <QApplication>

//*************************************************************************************

int main(int argc, char *argv[])
{
	/*
	 * include the virtual keyboard
	 * but the virtual keyboard is not available on the Raspberry Pi because the
	 * qml-module is not installed on it
	 *
	*/
	//qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

	QApplication a(argc, argv);
	MainWindow w;
	w.setWindowTitle("Automatische Abfüllanlage");	//set the window title
	w.show();										//show the MainWindow
	//w.showFullScreen();							//show the MainWindow in Fullscreen

	return a.exec();
}
