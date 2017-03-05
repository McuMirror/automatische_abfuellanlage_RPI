/*
 * Project:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	menu_mix.h
 *
 * Developer:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 */

#ifndef MENU_MIX_H
#define MENU_MIX_H

#include "mixture.h"
#include <QDialog>
#include <QString>
#include <QList>

namespace Ui {
class Menu_Mix;
}

class Menu_Mix : public QDialog
{
	Q_OBJECT

public:
	explicit Menu_Mix(QWidget *parent = 0);
	~Menu_Mix();

private slots:
	void on_pushButton_OK_clicked();
	void on_pushButton_cancel_clicked();
	void on_pushButton_add_clicked();
	void writeTableWidget(void);

	void on_pushButton_edit_clicked();

	void on_pushButton_delete_clicked();

public slots:
	void setContainerNames(QString Container_1,
						   QString Container_2,
						   QString Container_3,
						   QString Container_4);

	void setMixtures(QList<mixture*> mixtures);
	QList<mixture*> getMixtures(void);
	void setMaxVolume(int volume);

private:
	Ui::Menu_Mix *ui;

	int maxVolume = 0;

	QString ContainerName_1;
	QString ContainerName_2;
	QString ContainerName_3;
	QString ContainerName_4;

	QList<mixture*> mixtures_1;

};

#endif // MENU_MIX_H
