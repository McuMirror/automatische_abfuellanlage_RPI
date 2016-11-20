/* ###########################################################################################################
 * Projekt:		Diplomarbeit: Autmatische Abfüllanlage
 * Host:		Raspberry PI 3B
 * Filename:	mainwindow.c
 *
 * Entwickler:	Wögerbauer Stefan
 * E-Mail:		woegste@hotmail.com
 *
 * Änderungen:
 * Name:	Datum:		Zeit[h]:	Änderung:
 *
 * WS		05.11.2016	1			Projekt anlgen,
 * WS		06.11.2016	4			Serielle Schnittstelle hinzugefügt, sämtliche Fenster erstellt
 * WS		07.11.2016	6			Programmieren der Klasse mixture sowie erstellen der Logik für das Löschen,
 *									Bearbeiten und Erstellen mon Mischungen
 *									Konfigurieren der seriellen SChnittstelle sowie senden von Daten
 * WS		9.11.2016	2			Begonnen Bluetooth Kommunikation herzustellen
 * ##########################################################################################################
 */

//********************** INCLUDES *************************************************
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menu.h"
#include "menu_mix.h"

#include <QPushButton>
#include <QListWidgetItem>
#include <QListWidget>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>
#include <QScreen>
#include <QDialog>

//Bluetooth
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServer>
#include <QBluetoothUuid>

//Serial Port
#include <QSerialPort>
#include <QSerialPortInfo>

//*********************** MACROS **************************************************

#define serialTimeOutTime_ms 100
#define Bluetooth 1			// 1 wenn Bluetooth im Programm entahlten sein soll, ansonsten 0 -> Dies muss auch im Header gesetzt werden

//**********************************************************************************

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	clockTimer = new QTimer(this);
	clockTimer->start(1000);
	connect(clockTimer, SIGNAL(timeout()),	 this, SLOT(updateTime()));			//Update time all second

	serialTimeOut= new QTimer(this);
	serialTimeOut->setSingleShot(true);
	connect(serialTimeOut, SIGNAL(timeout()), this, SLOT(readSerialCommand()));

	QScreen *screen = QApplication::screens().at(0);
	screenWidth  = screen->availableSize().width();
	screenHeight = screen->availableSize().height();

	//**************** BLUETOOTH **************************************************************************************************

#if Bluetooth

	localBluetoothAdapters = QBluetoothLocalDevice::allDevices();

	if(localBluetoothAdapters.size() == 0)
	{
		ui->label_BT_clients->setVisible(false);
		ui->label_BT_headline->setVisible(false);
		ui->label_BT_status_connection->setVisible(false);
		ui->listWidget_BT_Clienten->setVisible(false);
		ui->comboBox_BT_local_Bluetooth_Adapter->setVisible(false);
	}
	else
	{
		for(int i=0; i < localBluetoothAdapters.size(); i++)
		{
			ui->comboBox_BT_local_Bluetooth_Adapter->addItem(localBluetoothAdapters.at(i).name());
		}

		QBluetoothLocalDevice localDevice (localBluetoothAdapters.at(ui->comboBox_BT_local_Bluetooth_Adapter->currentIndex()).address());
		ui->label_BT_status_connection->setText("Adresse: " + localDevice.address().toString());

		if(localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff)
		{
			localDevice.powerOn();
		}

		if(localDevice.hostMode() == !QBluetoothLocalDevice::HostDiscoverable)
		{
			localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
		}
	}

	if(localBluetoothAdapters.size() != 0)
	{

		BluetoothServer = new BluetoothTransmissionServer(this);

		//******* CONNECTS ******************************************************************************************

		connect(BluetoothServer,	SIGNAL(clientConnected(QString)),           this,	SLOT(clientConnected(QString)));
		connect(BluetoothServer,	SIGNAL(clientDisconnected(QString)),        this,	SLOT(clientDisconnected(QString)));
		connect(BluetoothServer,	SIGNAL(messageReceived(QString,QString)),   this,	SLOT(BluetoothCommandReceived(QString,QString)));

		//***********************************************************************************************************

		BluetoothServer->startServer();
	}

#else

	ui->label_BT_clients->setVisible(false);
	ui->label_BT_headline->setVisible(false);
	ui->label_BT_status_connection->setVisible(false);
	ui->listWidget_BT_Clienten->setVisible(false);
	ui->comboBox_BT_local_Bluetooth_Adapter->setVisible(false);

#endif
	//*************************************************************************************************************************

	readMixtures();
	writeListWidget();

	setConatinerAmounts(100 * ((double)settings.value("ContainerVolume_1", "").toInt() / (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_2", "").toInt() / (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_3", "").toInt() / (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_4", "").toInt() / (double)settings.value("ContainerVolume", "").toInt()));
}

MainWindow::~MainWindow()
{
	serial.close();
	BluetoothServer->stopServer();
	delete ui;
}

void MainWindow::updateTime(void)
{
	QString time;
	time = QDateTime::currentDateTime().toString("dd.MM.yyyy	hh:mm:ss");
	ui->statusBar->showMessage(time);

	if(serial.isOpen() == false || ui->comboBox_serial->isMaximized())
	{
		ui->comboBox_serial->clear();
		foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
		{
			ui->comboBox_serial->addItem(info.portName());
		}
	}
}

void MainWindow::on_actionMischungen_bearbeiten_triggered()
{
	Menu_Mix menu_mix;
	menu_mix.setWindowTitle("Mischungen bearbeiten");

	menu_mix.setContainerNames(settings.value("ContainerName_1", "").toString(), settings.value("ContainerName_2", "").toString(),
							   settings.value("ContainerName_3", "").toString(), settings.value("ContainerName_4", "").toString());

	menu_mix.setMixtures(mixtures);
	menu_mix.setMaxVolume(settings.value("MaxVolume","").toInt());

	if(menu_mix.exec() == QDialog::Accepted)
	{
		this->mixtures = menu_mix.getMixtures();
		saveMixtures();
	}
	writeListWidget();
}

void MainWindow::on_action_ber_triggered()
{
	QMessageBox::information(this, "Automatische Abfüllanlage", "Version: 0.1\nEntwickler: Stefan Wögerbauer");
}

void MainWindow::on_actionBeenden_triggered()
{
	this->close();
}

void MainWindow::on_actionEinstellungen_triggered()
{
	menu Menu;
	Menu.setWindowTitle("Einstellungen");

	Menu.setContainerNames(settings.value("ContainerName_1", "").toString(), settings.value("ContainerName_2", "").toString(),
						   settings.value("ContainerName_3", "").toString(), settings.value("ContainerName_4", "").toString());
	Menu.setMaxVolume(settings.value("MaxVolume","").toInt());
	Menu.setContainerVolume(settings.value("ContainerVolume", "").toInt());

	if(Menu.exec() == QDialog::Accepted)
	{
		settings.setValue("ContainerName_1", Menu.getContainerName_1());
		settings.setValue("ContainerName_2", Menu.getContainerName_2());
		settings.setValue("ContainerName_3", Menu.getContainerName_3());
		settings.setValue("ContainerName_4", Menu.getContainerName_4());
		settings.setValue("MaxVolume",		 Menu.getMaxVolume());
		settings.setValue("ContainerVolume", Menu.getContainerVolume());

		if(settings.value("ContainerVolume_1", "").toInt() > settings.value("ContainerVolume", "").toInt())
		{
			settings.setValue("ContainerVolume_1", settings.value("ContainerVolume", "").toInt());
		}

		if(settings.value("ContainerVolume_2", "").toInt() > settings.value("ContainerVolume", "").toInt())
		{
			settings.setValue("ContainerVolume_2", settings.value("ContainerVolume", "").toInt());
		}

		if(settings.value("ContainerVolume_3", "").toInt() > settings.value("ContainerVolume", "").toInt())
		{
			settings.setValue("ContainerVolume_3", settings.value("ContainerVolume", "").toInt());
		}

		if(settings.value("ContainerVolume_4", "").toInt() > settings.value("ContainerVolume", "").toInt())
		{
			settings.setValue("ContainerVolume_4", settings.value("ContainerVolume", "").toInt());
		}
		calculateContainerVolumes(0, 0, 0, 0);
	}
}

void MainWindow::sendSerialCommand(QString sender ,QString command)
{
	serial.write(command.toLocal8Bit() + '\n');

	serialTimeOut->start(serialTimeOutTime_ms);


	QString time;
	time = QDateTime::currentDateTime().toString("hh:mm:ss");

	QListWidgetItem *newItem = new QListWidgetItem;
	newItem->setText(time + " " + sender + ": " + command);
	newItem->setTextColor(Qt::blue);
	ui->listWidget_serial_traffic->insertItem(1, newItem);

	//TODO

	qDebug() << "send Serial Command: " << command;
}

void MainWindow::readSerialCommand(void)
{
	response.clear();
	response.append(serial.readAll());

	QString time;
	time = QDateTime::currentDateTime().toString("hh:mm:ss");

	QListWidgetItem *newItem = new QListWidgetItem;
	newItem->setText(time + ": " + response);
	newItem->setTextColor(Qt::red);
	ui->listWidget_serial_traffic->insertItem(1, newItem);

	if(commandSended == true && response == "received")
	{
		currentlyWorking = true;
		commandSended = false;
		commandReceived = true;
	}

	if(commandReceived == true && response == "ready")
	{
		currentlyWorking = false;
		commandReceived = false;
	}
	//******************************************************************
	//Bluetooth
	if(localBluetoothAdapters.size() == 0)
	{
		BluetoothServer->sendMessage(response);
	}
	//******************************************************************


	//TODO: Timout -> send again
	qDebug() << "read Serial Command: " << response.trimmed();

}

void MainWindow::on_pushButton_connect_clicked()
{
	static int i=0;

	if(i%2 == 0)
	{
		if (serial.portName() != ui->comboBox_serial->currentText())
		{
			serial.close();
			serial.setBaudRate(QSerialPort::Baud4800);
			serial.setParity(QSerialPort::NoParity);
			serial.setDataBits(QSerialPort::Data8);
			serial.setPortName(ui->comboBox_serial->currentText());
		}

		if (!serial.open(QIODevice::ReadWrite))
		{
			qDebug() << "can't open serial Port";
			QMessageBox::critical(this, "Automatische Abfüllanlage",
								  tr("%1 konnte nicht geöffnet werden, error code: %2").arg(serial.portName()).arg(serial.error()));

			return;
		}
		else
		{
			ui->pushButton_connect->setText("Trennen");
			ui->comboBox_serial->setEnabled(false);
			qDebug()<< "connect to serial Port";
			ui->label_status->setText(tr("Verbunden mit Port: %1").arg(ui->comboBox_serial->currentText()));
		}

		i++;
	}
	else
	{
		qDebug()<< "close serial Port";
		serial.close();
		i++;
		ui->pushButton_connect->setText("Verbinden");
		ui->label_status->setText("getrennt");
		ui->comboBox_serial->setEnabled(true);
	}
}

void MainWindow::on_pushButton_clearSerialTraffic_clicked()
{
	ui->listWidget_serial_traffic->clear();
}

void MainWindow::saveMixtures (void)
{
	settings.remove("MixtureList");
	settings.beginWriteArray("MixtureList");

	for(int i=0; i<mixtures.size(); i++)
	{
		settings.setArrayIndex(i);
		settings.setValue("Name", mixtures.at(i)->getName());
		settings.setValue("Container_1", mixtures.at(i)->getAmountContainer_1());
		settings.setValue("Container_2", mixtures.at(i)->getAmountContainer_2());
		settings.setValue("Container_3", mixtures.at(i)->getAmountContainer_3());
		settings.setValue("Container_4", mixtures.at(i)->getAmountContainer_4());
	}
	settings.endArray();
}

void MainWindow::readMixtures (void)
{
	int size_Mixtures = settings.beginReadArray("MixtureList");

	for(int i=0; i<size_Mixtures; i++)
	{
		settings.setArrayIndex(i);

		mixtures.append(new mixture);
		mixtures.last()->setName(settings.value("Name","").toString());
		mixtures.last()->setAmountContainer_1(settings.value("Container_1", "").toInt());
		mixtures.last()->setAmountContainer_2(settings.value("Container_2", "").toInt());
		mixtures.last()->setAmountContainer_3(settings.value("Container_3", "").toInt());
		mixtures.last()->setAmountContainer_4(settings.value("Container_4", "").toInt());
	}
	settings.endArray();
}

void MainWindow::writeListWidget(void)
{
	ui->listWidget->clear();

	for(int i = 0; i<mixtures.size(); i++)
	{
		QPushButton *button = new QPushButton;
		button->setText(mixtures.at(i)->getName());

		QListWidgetItem *item = new QListWidgetItem;
		QSize size;
		size.setHeight(40);
		item->setSizeHint(size);
		ui->listWidget->addItem(item);
		ui->listWidget->setItemWidget(item, button);
		connect(button, SIGNAL(clicked(bool)), mixtures.at(i), SLOT(getCommandValues()));
		connect(mixtures.at(i), SIGNAL(sendCommandValues(QList<int>)), this, SLOT(ButtonSlot(QList<int>)));
	}
}

void MainWindow::ButtonSlot(QList<int> valueList)
{
		if(serial.isOpen() && currentlyWorking == false)
		{
			if(settings.value("ContainerVolume_1", "").toInt() - valueList.at(0) < 0)
			{
				BluetoothServer->sendMessage("_Nicht genügend "  + settings.value("ContainerName_1","") .toString() +
											 " in Behälter 1 vorhanden.\nBitte befüllen Sie "
											   "diesen vollständig und klicken Sie auf den Button unter der Grafik des jeweiligen Behälters");


				QMessageBox::critical(this, "Automatische Abfüllanlage", "Nicht genügend "  +
									  settings.value("ContainerName_1","") .toString() + " in Behälter 1 vorhanden.\nBitte befüllen Sie "
										"diesen vollständig und klicken Sie auf den Button unter der Grafik des jeweiligen Behälters");

			}
			else if(settings.value("ContainerVolume_2", "").toInt() - valueList.at(0) < 0)
			{
				BluetoothServer->sendMessage("_Nicht genügend "  + settings.value("ContainerName_2","") .toString() +
											 " in Behälter 2 vorhanden.\nBitte befüllen Sie "
											   "diesen vollständig und klicken Sie auf den Button unter der Grafik des jeweiligen Behälters");


				QMessageBox::critical(this, "Automatische Abfüllanlage", "Nicht genügend "  +
									  settings.value("ContainerName_2","") .toString() + " in Behälter 2 vorhanden.\nBitte befüllen Sie "
										"diesen vollständig und klicken Sie auf den Button unter der Grafik des jeweiligen Behälters");

			}
			else if(settings.value("ContainerVolume_3", "").toInt() - valueList.at(0) < 0)
			{
				BluetoothServer->sendMessage("_Nicht genügend "  + settings.value("ContainerName_3","") .toString() +
											 " in Behälter 3 vorhanden.\nBitte befüllen Sie "
											   "diesen vollständig und klicken Sie auf den Button unter der Grafik des jeweiligen Behälters");


				QMessageBox::critical(this, "Automatische Abfüllanlage", "Nicht genügend "  +
									  settings.value("ContainerName_3","") .toString() + " in Behälter 3 vorhanden.\nBitte befüllen Sie "
										"diesen vollständig und klicken Sie auf den Button unter der Grafik des jeweiligen Behälters");
			}
			else if(settings.value("ContainerVolume_4", "").toInt() - valueList.at(0) < 0)
			{
				BluetoothServer->sendMessage("_Nicht genügend "  + settings.value("ContainerName_4","") .toString() +
											 " in Behälter 4 vorhanden.\nBitte befüllen Sie "
											   "diesen vollständig und klicken Sie auf den Button unter der Grafik des jeweiligen Behälters");

				QMessageBox::critical(this, "Automatische Abfüllanlage", "Nicht genügend "  +
									  settings.value("ContainerName_4","") .toString() + " in Behälter 4 vorhanden.\nBitte befüllen Sie "
										"diesen vollständig und klicken Sie auf den Button unter der Grafik des jeweiligen Behälters");
			}
			else
			{
				sendSerialCommand("Host",	"/" + QString("%1").arg(valueList.at(0),2,'g',-1,'0') + "/" + QString("%1").arg(valueList.at(1),2,'g',-1,'0') +
								  "/" + QString("%1").arg(valueList.at(2),2,'g',-1,'0') + "/" + QString("%1").arg(valueList.at(3),2,'g',-1,'0') + "/");

				commandSended = true;

				calculateContainerVolumes(valueList.at(0), valueList.at(1), valueList.at(2), valueList.at(3));
			}
		}
		else if(!serial.isOpen())
		{
			BluetoothServer->sendMessage("_Bitte stellen Sie eine Verbindung zur Anlage her");
			QMessageBox::critical(this, "Automatische Anfüllanlage", "Bitte stellen Sie eine Verbindung zur Anlage her");	
		}
		else if(currentlyWorking == true)
		{
			BluetoothServer->sendMessage("_Bitte warten Sie bis der Vorgang abgeschlossen ist.");
			QMessageBox::critical(this, "Automatische Anfüllanlage", "Bitte warten Sie bis der Vorgang abgeschlossen ist.");	
		}
		else
		{
			BluetoothServer->sendMessage("_Ein unbekannter Fehler ist aufgetreten");
			QMessageBox::critical(this, "Automatische Anfüllanlage", "unbekannter Fehler");
		}
}

void MainWindow::on_comboBox_BT_local_Bluetooth_Adapter_currentIndexChanged(int index)
{
	//BluetoothServer->stopServer();

	QBluetoothLocalDevice localDevice (localBluetoothAdapters.at(ui->comboBox_BT_local_Bluetooth_Adapter->currentIndex()).address());
	ui->label_BT_status_connection->setText("Adresse: " + localDevice.address().toString());

	if(localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff)
	{
		localDevice.powerOn();
	}

	if(localDevice.hostMode() == !QBluetoothLocalDevice::HostDiscoverable)
	{
		localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
	}

	//BluetoothServer->startServer();
}

#if Bluetooth

void MainWindow::clientConnected(const QString &name)
{
	QListWidgetItem *item = new QListWidgetItem;
	item->setText(name);

	ui->listWidget_BT_Clienten->addItem(item);
}

void MainWindow::clientDisconnected(const QString &name)
{
	QMessageBox::information(this, "Automatische Abfüllanlage", name + " hat die Verbindung getrennt!");
}

void MainWindow::BluetoothCommandReceived(QString client, QString command)
{
	if(command.at(0) == '/')
	{
		int count;

		for(int i = 0; i< mixtures.size(); i++)
		{
			QString mix = "/" + mixtures.at(i)->getName();
			if(mix == command)
			{
				QList<int> valueList;
				valueList.append(mixtures.at(i)->getAmountContainer_1());
				valueList.append(mixtures.at(i)->getAmountContainer_2());
				valueList.append(mixtures.at(i)->getAmountContainer_3());
				valueList.append(mixtures.at(i)->getAmountContainer_4());

				ButtonSlot(valueList);

				count++;
			}
		}

		if(count != 1)
		{
			//BluetoothServer->sendMessage("_ERROR");
		}
	}
	else if(command.at(0) == '_')
	{
		//TODO: send back information via Bluetooth
		qDebug()<<"send back command: " << command;
		if(command == "_sendBackMixes")
		{
			sendMixes();
		}
	}
	else
	{
		//TODO: send back error via Bluetooth

		QMessageBox::critical(this, "Automatische Abfüllanlage", "Error: Befehl konnte nicht gelesen werden");
	}
}

void MainWindow::sendMixes(void)
{
	for(int i=0; i < mixtures.size(); i++)
	{
		BluetoothServer->sendMessage(mixtures.at(i)->getName());
	}
	BluetoothServer->sendMessage("ready");
}

#endif

void MainWindow::setConatinerAmounts(int Container_1, int Container_2, int Container_3, int Container_4)
{
	ui->progressBar_Contaner_1->setValue(Container_1);
	ui->progressBar_Contaner_2->setValue(Container_2);
	ui->progressBar_Contaner_3->setValue(Container_3);
	ui->progressBar_Contaner_4->setValue(Container_4);
}

void MainWindow::calculateContainerVolumes(int ConsumtionContainer_1, int ConsumtionContainer_2, int ConsumtionContainer_3, int ConsumtionContainer_4)
{
	settings.setValue("ContainerVolume_1", settings.value("ContainerVolume_1", "").toInt() - ConsumtionContainer_1);
	settings.setValue("ContainerVolume_2", settings.value("ContainerVolume_2", "").toInt() - ConsumtionContainer_2);
	settings.setValue("ContainerVolume_3", settings.value("ContainerVolume_3", "").toInt() - ConsumtionContainer_3);
	settings.setValue("ContainerVolume_4", settings.value("ContainerVolume_4", "").toInt() - ConsumtionContainer_4);

	setConatinerAmounts(100 * ((double)settings.value("ContainerVolume_1", "").toInt() / (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_2", "").toInt() / (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_3", "").toInt() / (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_4", "").toInt() / (double)settings.value("ContainerVolume", "").toInt()));

}

void MainWindow::on_pushButton_fill_C1_clicked()
{
	settings.setValue("ContainerVolume_1", settings.value("ContainerVolume", "").toInt());
	ui->progressBar_Contaner_1->setValue(100);
}

void MainWindow::on_pushButton_fill_C2_clicked()
{
	settings.setValue("ContainerVolume_2", settings.value("ContainerVolume", "").toInt());
	ui->progressBar_Contaner_2->setValue(100);
}

void MainWindow::on_pushButton_fill_C3_clicked()
{
	settings.setValue("ContainerVolume_3", settings.value("ContainerVolume", "").toInt());
	ui->progressBar_Contaner_3->setValue(100);
}

void MainWindow::on_pushButton_fill_C4_clicked()
{
	settings.setValue("ContainerVolume_4", settings.value("ContainerVolume", "").toInt());
	ui->progressBar_Contaner_4->setValue(100);
}

