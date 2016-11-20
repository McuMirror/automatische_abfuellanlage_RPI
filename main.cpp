/* ###########################################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	main.c
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * Änderungen:
 * Name:	Datum:		Änderung:
 *
 * WS		7.11.2016	Fenstertitel geändert
 * ##########################################################################################################
 */

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.setWindowTitle("Automatische Abfüllanlage");
	w.show();
	//w.showFullScreen();

	return a.exec();
}
