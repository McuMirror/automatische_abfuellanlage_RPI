/* ####################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	menu.h
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * ####################################################################################
 */

#ifndef MENU_H
#define MENU_H

#include <QDialog>

namespace Ui {
class menu;
}

class menu : public QDialog
{
	Q_OBJECT

public:
	explicit menu(QWidget *parent = 0);
	~menu();

private slots:
	void on_pushButton_OK_clicked();
	void on_pushButton_cancel_clicked();

private:
	Ui::menu *ui;

public slots:
	QString getContainerName_1(void);
	QString getContainerName_2(void);
	QString getContainerName_3(void);
	QString getContainerName_4(void);
	void	setContainerNames(QString Container_1,
							  QString Container_2,
							  QString Container_3,
							  QString Container_4);

	int		getMaxVolume(void);
	void	setMaxVolume(int volume);
	int		getContainerVolume(void);
	void	setContainerVolume(int volume);

};

#endif // MENU_H
