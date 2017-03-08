/*
 * ####################################################################################
 * Project:		Diplomarbeit: Autmatische Abfüllanlage								  #
 * Host:		Raspberry PI 3B														  #
 * Filename:	dialog_admix.cpp													  #
 *																					  #
 * Developer:	Wögerbauer Stefan													  #
 * E-Mail:		woegste@hotmail.com													  #
 * ####################################################################################
 */

//********************** INCLUDES *****************************************************
#include "dialog_addmix.h"
#include "ui_dialog_addmix.h"
#include <QMessageBox>
#include <QDebug>
//*************************************************************************************

Dialog_addMix::Dialog_addMix(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Dialog_addMix)
{
	ui->setupUi(this);

	/*
	 * connect the signals from the SpinBoxes with the Method calculateDiff. Now the
	 * available space for liquid is calculated after changes.
	 *
	 */

	connect(ui->spinBox_container_1,	SIGNAL(valueChanged(int)), this,
										SLOT(calculateDiff(void)));

	connect(ui->spinBox_container_2,	SIGNAL(valueChanged(int)), this,
										SLOT(calculateDiff(void)));

	connect(ui->spinBox_container_3,	SIGNAL(valueChanged(int)), this,
										SLOT(calculateDiff(void)));

	connect(ui->spinBox_container_4,	SIGNAL(valueChanged(int)), this,
										SLOT(calculateDiff(void)));

	ui->label_diff->setText(QString::number(maxVolume));
}

Dialog_addMix::~Dialog_addMix()
{
	delete ui;
}

void Dialog_addMix::on_pushButton_OK_clicked()
{
	setName(ui->lineEdit_name->text());

	//if there is no name inside the lineEdit, storing isn't allowed
	if(name.length() == 0)
	{
		QMessageBox::critical(this, "Automatische Abfüllanlage",
							  "Bitte geben Sie einen Namen ein.");
	}
	else	//Exceeding the maxVolume isn't allowed
	{
		if((ui->spinBox_container_1->value() + ui->spinBox_container_2->value() +
			ui->spinBox_container_3->value() + ui->spinBox_container_4->value())
				> maxVolume)
		{
			QMessageBox::critical(this, "Automatische Abfüllanlage",
							"Die maximale Füllmenge darf nicht überschritten werden");
		}
		else		//everything OK
		{
			QDialog::accept();
		}
	}
}

void Dialog_addMix::on_pushButton_cancel_clicked()
{
	QDialog::reject();
}

QString Dialog_addMix::getName(void)
{
	return name;						//return the name of the mixture
}

void Dialog_addMix::setName(QString name)
{
	this->name = name;					//set the current name of the mixture
	ui->lineEdit_name->setText(name);	//set the text of the line edit
}

int Dialog_addMix::getAmount_container_1(void)
{
	//return the amount of coantainer 1
	return ui->spinBox_container_1->value();
}

int Dialog_addMix::getAmount_container_2(void)
{
	//return the amount of coantainer 2
	return ui->spinBox_container_2->value();
}

int Dialog_addMix::getAmount_container_3(void)
{
	//return the amount of coantainer 3
	return ui->spinBox_container_3->value();
}

int Dialog_addMix::getAmount_container_4(void)
{
	//return the amount of coantainer 4
	return ui->spinBox_container_4->value();
}

//Set the names of the Containers
void Dialog_addMix::setContainerNames(QString Container_1, QString Container_2,
									  QString Container_3, QString Container_4)
{
	//set the names in the labels of the containers
	ui->label_container_1->setText(Container_1);
	ui->label_container_2->setText(Container_2);
	ui->label_container_3->setText(Container_3);
	ui->label_container_4->setText(Container_4);
}

//Set the liquid amounts of the containers
void Dialog_addMix::setAmout_Containers(int Container_1, int Container_2,
										int Container_3, int Container_4)
{
	//set the current amount of the containers
	ui->spinBox_container_1->setValue(Container_1);
	ui->spinBox_container_2->setValue(Container_2);
	ui->spinBox_container_3->setValue(Container_3);
	ui->spinBox_container_4->setValue(Container_4);
}

// set the maximum glass volume
void Dialog_addMix::setMaxVolume(int volume)
{
	//set the max volume of the glass
	this->maxVolume = volume;
}

//calculate the availables space for the liquid
void Dialog_addMix::calculateDiff(void)
{
	/*
	 * calculate the difference of the current liquid amount in the glass and the max
	 * Volume of the glass
	 *
	 * if the sum of the liquids is higher than the max volume a warning will be sended
	 */

	if((ui->spinBox_container_1->value() + ui->spinBox_container_2->value() +
		ui->spinBox_container_3->value() + ui->spinBox_container_4->value()) >maxVolume)
	{
		QMessageBox::critical(this, "Automatische Abfüllanlage",
							  "Die maximale Füllmenge wurde Überschritten");
	}
	ui->label_diff->setText(QString::number(maxVolume -(ui->spinBox_container_1->value()
											+ ui->spinBox_container_2->value()
											+ ui->spinBox_container_3->value()
											+ ui->spinBox_container_4->value())));
}
