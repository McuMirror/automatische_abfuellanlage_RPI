/* #####################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	Dialog_addMix.cpp
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * #####################################################################################
 */


#include "dialog_addmix.h"
#include "ui_dialog_addmix.h"

#include <QMessageBox>

Dialog_addMix::Dialog_addMix(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Dialog_addMix)
{
	ui->setupUi(this);

	//connect(ui->spinBox_container_1, SIGNAL(valueChanged(int)), this, SLOT(calculateDiff(void)));
	//connect(ui->spinBox_container_2, SIGNAL(valueChanged(int)), this, SLOT(calculateDiff(void)));
	//connect(ui->spinBox_container_3, SIGNAL(valueChanged(int)), this, SLOT(calculateDiff(void)));
	//connect(ui->spinBox_container_4, SIGNAL(valueChanged(int)), this, SLOT(calculateDiff(void)));

	ui->label_diff->setText(QString::number(maxVolume));
}

Dialog_addMix::~Dialog_addMix()
{
	delete ui;
}

void Dialog_addMix::on_pushButton_OK_clicked()
{
	setName(ui->lineEdit_name->text());
	if(name.length() == 0)
	{
		QMessageBox::critical(this, "Automatische Abfüllanlage",
							  "Bitte geben Sie einen Namen ein.");
	}
	else
	{
		if((ui->spinBox_container_1->value() + ui->spinBox_container_2->value() +
			ui->spinBox_container_3->value() + ui->spinBox_container_4->value())
				> maxVolume)
		{
			QMessageBox::critical(this, "Automatische Abfüllanlage",
							"Die maximale Füllmenge darf nicht überschritten werden");
		}
		else
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
	return name;
}

void Dialog_addMix::setName(QString name)
{
	this->name = name;
	ui->lineEdit_name->setText(name);
}

int Dialog_addMix::getAmount_container_1(void)
{
	return ui->spinBox_container_1->value();
}

int Dialog_addMix::getAmount_container_2(void)
{
	return ui->spinBox_container_2->value();
}

int Dialog_addMix::getAmount_container_3(void)
{
	return ui->spinBox_container_3->value();
}

int Dialog_addMix::getAmount_container_4(void)
{
	return ui->spinBox_container_4->value();
}

void Dialog_addMix::setContainerNames(QString Container_1, QString Container_2,
									  QString Container_3, QString Container_4)
{
	ui->label_container_1->setText(Container_1);
	ui->label_container_2->setText(Container_2);
	ui->label_container_3->setText(Container_3);
	ui->label_container_4->setText(Container_4);
}

void Dialog_addMix::setAmout_Containers(int Container_1, int Container_2,
										int Container_3, int Container_4)
{
	ui->spinBox_container_1->setValue(Container_1);
	ui->spinBox_container_2->setValue(Container_2);
	ui->spinBox_container_3->setValue(Container_3);
	ui->spinBox_container_4->setValue(Container_4);
}

void Dialog_addMix::setMaxVolume(int volume)
{
	this->maxVolume = volume;
}

void Dialog_addMix::calculateDiff(void)
{
	if((ui->spinBox_container_1->value() + ui->spinBox_container_2->value() +
		ui->spinBox_container_3->value() + ui->spinBox_container_4->value()) >maxVolume)
	{
		QMessageBox::critical(this, "Automatische Abfüllanlage",
							  "Die maximale Füllmenge wurde Überschritten");
	}
	ui->label_diff->setText(QString::number(maxVolume - ui->spinBox_container_1->value()
											+ ui->spinBox_container_2->value()
											+ ui->spinBox_container_3->value()
											+ ui->spinBox_container_4->value()));
}
