/* ####################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	dialog_addmix.h
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * ####################################################################################
 */

#ifndef DIALOG_ADDMIX_H
#define DIALOG_ADDMIX_H

#include <QDialog>
#include <QString>

namespace Ui {
class Dialog_addMix;
}

class Dialog_addMix : public QDialog
{
	Q_OBJECT

public:
	explicit Dialog_addMix(QWidget *parent = 0);
	~Dialog_addMix();

private slots:
	void on_pushButton_OK_clicked();
	void on_pushButton_cancel_clicked();

public slots:
	QString getName(void);
	int getAmount_container_1(void);
	int getAmount_container_2(void);
	int getAmount_container_3(void);
	int getAmount_container_4(void);
	void setName(QString name);
	void setContainerNames(QString Container_1,
						   QString Container_2,
						   QString Container_3,
						   QString Container_4);

	void setAmout_Containers(int Container_1,
							 int Container_2,
							 int Container_3,
							 int Container_4);

	void setMaxVolume(int volume);
	void calculateDiff(void);

private:
	Ui::Dialog_addMix *ui;

	QString name;
	int amount_container_1;
	int amount_container_2;
	int amount_container_3;
	int amount_container_4;

	int maxVolume = 0;
};

#endif // DIALOG_ADDMIX_H
