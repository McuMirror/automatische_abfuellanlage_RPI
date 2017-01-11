/* ####################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	menu.cpp
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * ####################################################################################
 */

#include "menu.h"
#include "ui_menu.h"


menu::menu(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::menu)
{
	ui->setupUi(this);
}

menu::~menu()
{
	delete ui;
}

void menu::on_pushButton_OK_clicked()
{
	QDialog::accept();
}

void menu::on_pushButton_cancel_clicked()
{
	QDialog::reject();
}

QString menu::getContainerName_1(void)
{
	return ui->lineEdit_container_1->text();
}

QString menu::getContainerName_2(void)
{
	return ui->lineEdit_container_2->text();
}

QString menu::getContainerName_3(void)
{
	return ui->lineEdit_container_3->text();
}

QString menu::getContainerName_4(void)
{
	return ui->lineEdit_container_4->text();
}

void menu::setContainerNames(QString Container_1, QString Container_2,
							 QString Container_3, QString Container_4)
{
	ui->lineEdit_container_1->setText(Container_1);
	ui->lineEdit_container_2->setText(Container_2);
	ui->lineEdit_container_3->setText(Container_3);
	ui->lineEdit_container_4->setText(Container_4);
}

int menu::getMaxVolume(void)
{
	return ui->spinBox_volume_Glas->value();
}

void menu::setMaxVolume(int volume)
{
	ui->spinBox_volume_Glas->setValue(volume);
}

int menu::getContainerVolume(void)
{
	return ui->spinBox_Container_Volume->value();
}

void menu::setContainerVolume(int volume)
{
	ui->spinBox_Container_Volume->setValue(volume);
}
