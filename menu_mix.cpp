/* ####################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	Menu_Mix.cpp
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * ####################################################################################
 */

#include "menu_mix.h"
#include "ui_menu_mix.h"
#include "dialog_addmix.h"
#include "mixture.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>

Menu_Mix::Menu_Mix(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Menu_Mix)
{
	ui->setupUi(this);
}

Menu_Mix::~Menu_Mix()
{
	delete ui;
}

void Menu_Mix::on_pushButton_OK_clicked()
{
	QDialog::accept();
}

void Menu_Mix::on_pushButton_cancel_clicked()
{
	QDialog::reject();
}

void Menu_Mix::on_pushButton_add_clicked()
{
	Dialog_addMix Add;
	Add.setWindowTitle("Mischung hinzufügen");
	Add.setContainerNames(ContainerName_1, ContainerName_2, ContainerName_3,
						  ContainerName_4);
	Add.setMaxVolume(maxVolume);

	mixtures_1.append(new mixture);

	if(Add.exec() == QDialog::Accepted)
	{
		mixtures_1.last()->setName(Add.getName());
		mixtures_1.last()->setAmountContainer_1(Add.getAmount_container_1());
		mixtures_1.last()->setAmountContainer_2(Add.getAmount_container_2());
		mixtures_1.last()->setAmountContainer_3(Add.getAmount_container_3());
		mixtures_1.last()->setAmountContainer_4(Add.getAmount_container_4());
	}

	writeTableWidget();
}

void Menu_Mix::setContainerNames(QString Container_1, QString Container_2,
								 QString Container_3, QString Container_4)
{
	this->ContainerName_1 = Container_1;
	this->ContainerName_2 = Container_2;
	this->ContainerName_3 = Container_3;
	this->ContainerName_4 = Container_4;
}

void Menu_Mix::setMixtures(QList<mixture*> mixtures)
{
	this->mixtures_1 = mixtures;

	writeTableWidget();
}

QList<mixture*> Menu_Mix::getMixtures(void)
{
	return mixtures_1;
}

void Menu_Mix::writeTableWidget(void)
{
	ui->tableWidget->clear();
	ui->tableWidget->setRowCount(0);

	for(int i=0; i < mixtures_1.size(); i++)
	{
		QTableWidgetItem* nameItem	= new QTableWidgetItem(mixtures_1.at(i)->getName());
		QTableWidgetItem* container_1= new QTableWidgetItem(
					QString::number(mixtures_1.at(i)->getAmountContainer_1()) + " cl");

		QTableWidgetItem* container_2= new QTableWidgetItem(
					QString::number(mixtures_1.at(i)->getAmountContainer_2()) + " cl");

		QTableWidgetItem* container_3= new QTableWidgetItem(
					QString::number(mixtures_1.at(i)->getAmountContainer_3()) + " cl");

		QTableWidgetItem* container_4= new QTableWidgetItem(
					QString::number(mixtures_1.at(i)->getAmountContainer_4()) + " cl");

		int rows = ui->tableWidget->rowCount();
		ui->tableWidget->setRowCount(rows+1);                     // increase row count
		ui->tableWidget->setColumnCount(5);
		ui->tableWidget->setRowHeight(rows, 30);
		ui->tableWidget->setHorizontalHeaderItem(
					0, new QTableWidgetItem("Name"));
		ui->tableWidget->setHorizontalHeaderItem(
					1, new QTableWidgetItem(ContainerName_1));
		ui->tableWidget->setHorizontalHeaderItem(
					2, new QTableWidgetItem(ContainerName_2));
		ui->tableWidget->setHorizontalHeaderItem(
					3, new QTableWidgetItem(ContainerName_3));
		ui->tableWidget->setHorizontalHeaderItem(
					4, new QTableWidgetItem(ContainerName_4));

		ui->tableWidget->setItem(rows, 0, nameItem);
		ui->tableWidget->setItem(rows, 1, container_1);
		ui->tableWidget->setItem(rows, 2, container_2);
		ui->tableWidget->setItem(rows, 3, container_3);
		ui->tableWidget->setItem(rows, 4, container_4);
	}
}

void Menu_Mix::on_pushButton_edit_clicked()
{
	if((ui->tableWidget->rowCount() !=0) &&(ui->tableWidget->selectedItems().size()==1))
	{
		Dialog_addMix Add;
		Add.setWindowTitle("Mischung bearbeiten");
		int index = ui->tableWidget->currentRow();
		Add.setContainerNames(ContainerName_1,
							  ContainerName_2,
							  ContainerName_3,
							  ContainerName_4);
		Add.setAmout_Containers(mixtures_1.at(index)->getAmountContainer_1(),
								mixtures_1.at(index)->getAmountContainer_2(),
								mixtures_1.at(index)->getAmountContainer_3(),
								mixtures_1.at(index)->getAmountContainer_4());
		Add.setName(mixtures_1.at(index)->getName());
		Add.setMaxVolume(maxVolume);

		if(Add.exec() == QDialog::Accepted)
		{
			mixtures_1.at(index)->setName(Add.getName());
			mixtures_1.at(index)->setAmountContainer_1(Add.getAmount_container_1());
			mixtures_1.at(index)->setAmountContainer_2(Add.getAmount_container_2());
			mixtures_1.at(index)->setAmountContainer_3(Add.getAmount_container_3());
			mixtures_1.at(index)->setAmountContainer_4(Add.getAmount_container_4());
		}
	}

	writeTableWidget();
}

void Menu_Mix::on_pushButton_delete_clicked()
{
	if((ui->tableWidget->rowCount() !=0) &&(ui->tableWidget->selectedItems().size()==1))
	{
		int indexDelete = ui->tableWidget->currentRow();
		QTableWidgetItem* ToDelete = ui->tableWidget->item(indexDelete, 0);
		QString nameToDelete = ToDelete->text();

		if(QMessageBox::question(this, tr("Mischung löschen"),
								 tr("Wollen Sie die Mischung %1 wirklich löschen?")
								 .arg(nameToDelete))
				== QMessageBox::Yes)
		{
			ui->tableWidget->removeRow(ui->tableWidget->currentRow());
			qDebug()<<"Mischung löschen: "<< mixtures_1.at(indexDelete)->getName();
			mixtures_1.removeAt(indexDelete);
		}
	}
}

void Menu_Mix::setMaxVolume(int volume)
{
	this->maxVolume = volume;
}
