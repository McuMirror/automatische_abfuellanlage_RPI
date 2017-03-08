/*
 * ####################################################################################
 * Project:		Diplomarbeit: Autmatische Abfüllanlage								  #
 * Host:		Raspberry PI 3B														  #
 * Filename:	menu_mix.cpp														  #
 *																					  #
 * Developer:	Wögerbauer Stefan													  #
 * E-Mail:		woegste@hotmail.com													  #
 * ####################################################################################
 */

//********************** INCLUDES *****************************************************
#include "menu_mix.h"
#include "ui_menu_mix.h"
#include "dialog_addmix.h"
#include "mixture.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>

//*************************************************************************************

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
	//accept the dialog
	QDialog::accept();
}

void Menu_Mix::on_pushButton_cancel_clicked()
{
	//cancel the dialog
	QDialog::reject();
}

void Menu_Mix::on_pushButton_add_clicked()
{
	//Add a new mixture

	Dialog_addMix Add;							//generate the object
	Add.setWindowTitle("Mischung hinzufügen");	//set the window Title
	Add.setContainerNames(ContainerName_1,		//name of container 1
						  ContainerName_2,		//name of container 2
						  ContainerName_3,		//name of container 3
						  ContainerName_4);		//name of container 4

	Add.setMaxVolume(maxVolume);				//set the max volume of the glass

	mixtures_1.append(new mixture);				//append the new objct to the list

	if(Add.exec() == QDialog::Accepted)
	{
		/*
		 *if the dialog is accepted set all the values to the new object
		 */

		mixtures_1.last()->setName(Add.getName());
		mixtures_1.last()->setAmountContainer_1(Add.getAmount_container_1());
		mixtures_1.last()->setAmountContainer_2(Add.getAmount_container_2());
		mixtures_1.last()->setAmountContainer_3(Add.getAmount_container_3());
		mixtures_1.last()->setAmountContainer_4(Add.getAmount_container_4());
	}

	writeTableWidget();		//update all the mixtures in the ListWidget
}

void Menu_Mix::setContainerNames(QString Container_1, QString Container_2,
								 QString Container_3, QString Container_4)
{
	//set the names of the container
	this->ContainerName_1 = Container_1;
	this->ContainerName_2 = Container_2;
	this->ContainerName_3 = Container_3;
	this->ContainerName_4 = Container_4;
}

void Menu_Mix::setMixtures(QList<mixture*> mixtures)
{
	//set the mixture list
	this->mixtures_1 = mixtures;

	writeTableWidget();		//update all the mixtures in the ListWidget
}

QList<mixture*> Menu_Mix::getMixtures(void)
{
	//return the mixture list
	return mixtures_1;
}

void Menu_Mix::writeTableWidget(void)
{
	ui->tableWidget->clear();			//delete the current entries
	ui->tableWidget->setRowCount(0);	//set the Row count to zero

	//list all mixes in an Table Widget
	for(int i=0; i < mixtures_1.size(); i++)
	{
		//create an TableWidgetitem with the name of the mixture
		QTableWidgetItem* nameItem	= new QTableWidgetItem(mixtures_1.at(i)->getName());

		//create an TableWidgetItem with the amount of liquid of container 1
		QTableWidgetItem* container_1 = new QTableWidgetItem(
					QString::number(mixtures_1.at(i)->getAmountContainer_1()) + " cl");

		//create an TableWidgetItem with the amount of liquid of container 2
		QTableWidgetItem* container_2 = new QTableWidgetItem(
					QString::number(mixtures_1.at(i)->getAmountContainer_2()) + " cl");

		//create an TableWidgetItem with the amount of liquid of container 3
		QTableWidgetItem* container_3 = new QTableWidgetItem(
					QString::number(mixtures_1.at(i)->getAmountContainer_3()) + " cl");

		//create an TableWidgetItem with the amount of liquid of container 4
		QTableWidgetItem* container_4 = new QTableWidgetItem(
					QString::number(mixtures_1.at(i)->getAmountContainer_4()) + " cl");

		int rows = ui->tableWidget->rowCount();					//get the row count
		ui->tableWidget->setRowCount(rows+1);					//increase row count
		ui->tableWidget->setColumnCount(5);						//Set collumn count = 5
		ui->tableWidget->setRowHeight(rows, 30);				//set row height = 30pt

		//insert the TableWidgetItems in the row
		ui->tableWidget->setItem(rows, 0, nameItem);
		ui->tableWidget->setItem(rows, 1, container_1);
		ui->tableWidget->setItem(rows, 2, container_2);
		ui->tableWidget->setItem(rows, 3, container_3);
		ui->tableWidget->setItem(rows, 4, container_4);
	}

	//Set the headers of the collumns

	//create and set an HorizontalHeaderItem with the text "name"
	ui->tableWidget->setHorizontalHeaderItem(
										0, new QTableWidgetItem("Name"));
	//create and set an HorizontalHeaderItem with the name of the liquid of container 1
	ui->tableWidget->setHorizontalHeaderItem(
										1, new QTableWidgetItem(ContainerName_1));
	//create and set an HorizontalHeaderItem with the name of the liquid of container 2
	ui->tableWidget->setHorizontalHeaderItem(
										2, new QTableWidgetItem(ContainerName_2));
	//create and set an HorizontalHeaderItem with the name of the liquid of container 3
	ui->tableWidget->setHorizontalHeaderItem(
										3, new QTableWidgetItem(ContainerName_3));
	//create and set an HorizontalHeaderItem with the name of the liquid of container 4
	ui->tableWidget->setHorizontalHeaderItem(
										4, new QTableWidgetItem(ContainerName_4));
}

void Menu_Mix::on_pushButton_edit_clicked()
{
	//edit a mixture if ther is only one marked and the number of mixtures isn't 0
	if((ui->tableWidget->rowCount() !=0) &&(ui->tableWidget->selectedItems().size()==1))
	{
		Dialog_addMix Add;							//create the object
		Add.setWindowTitle("Mischung bearbeiten");	//set the window Title
		int index = ui->tableWidget->currentRow();	//get the index of the special mix

		//set the container names
		Add.setContainerNames(ContainerName_1,
							  ContainerName_2,
							  ContainerName_3,
							  ContainerName_4);

		//load the data of this special mixture
		Add.setAmout_Containers(mixtures_1.at(index)->getAmountContainer_1(),
								mixtures_1.at(index)->getAmountContainer_2(),
								mixtures_1.at(index)->getAmountContainer_3(),
								mixtures_1.at(index)->getAmountContainer_4());

		Add.setName(mixtures_1.at(index)->getName());  //set the name of the mixture
		Add.setMaxVolume(maxVolume);				   //set the max volume of the glass
		Add.calculateDiff();

		if(Add.exec() == QDialog::Accepted)
		{
			//store the new values in the List
			mixtures_1.at(index)->setName(Add.getName());
			mixtures_1.at(index)->setAmountContainer_1(Add.getAmount_container_1());
			mixtures_1.at(index)->setAmountContainer_2(Add.getAmount_container_2());
			mixtures_1.at(index)->setAmountContainer_3(Add.getAmount_container_3());
			mixtures_1.at(index)->setAmountContainer_4(Add.getAmount_container_4());
		}
	}

	writeTableWidget();		//update the Table Widget
}

void Menu_Mix::on_pushButton_delete_clicked()
{
	//delete a mixture if ther is only one marked and the number of mixtures isn't 0
	if((ui->tableWidget->rowCount() !=0) &&(ui->tableWidget->selectedItems().size()==1))
	{
		int indexDelete = ui->tableWidget->currentRow();	//get the index of the mix
		//select the item
		QTableWidgetItem* ToDelete = ui->tableWidget->item(indexDelete, 0);
		//get the name of the item which is selected for deleteing
		QString nameToDelete = ToDelete->text();

		//ask a safty question
		if(QMessageBox::question(this, tr("Mischung löschen"),
								 tr("Wollen Sie die Mischung %1 wirklich löschen?")
								 .arg(nameToDelete))
				== QMessageBox::Yes)
		{
			//delete the mix from the tableWidget
			ui->tableWidget->removeRow(ui->tableWidget->currentRow());
			qDebug()<<"Mischung löschen: "<< mixtures_1.at(indexDelete)->getName();
			mixtures_1.removeAt(indexDelete);	//delete the mix from the list
		}
	}
}

void Menu_Mix::setMaxVolume(int volume)
{
	//set the max Volume of the glass
	this->maxVolume = volume;
}
