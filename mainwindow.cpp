/*
 *#####################################################################################
 * Project:		Diplomarbeit: Autmatische Abfüllanlage								  #
 * Host:		Raspberry PI 3B														  #
 * Filename:	mainwindow.cpp														  #
 *																					  #
 * Developer:	Wögerbauer Stefan													  #
 * E-Mail:		woegste@hotmail.com													  #
 *#####################################################################################
 *
 */

//********************** INCLUDES *****************************************************
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

//*********************** MACROS ******************************************************

#define serialTimeOutTime_ms 100
#define Bluetooth 1			// 1 wenn Bluetooth im Programm entahlten sein soll,
							//ansonsten 0 -> Dies muss auch im Header gesetzt werden

//*************************************************************************************

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	//Update time all second
	clockTimer = new QTimer(this);
	clockTimer->start(1000);
	connect(clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));

	/*
	 * Slave has a defined time to answer, so the Method to read the serial input data
	 * is called after a special time
	*/
	serialTimeOut = new QTimer(this);
	//serialTimeOut->setSingleShot(true);
	connect(serialTimeOut,	SIGNAL(timeout()), this, SLOT(readSerialCommand()));
	serialTimeOut->start(serialTimeOutTime_ms);

	//Update the serial Devices evrey 5 seconds
	updateSerialDevicesTimer = new QTimer(this);
	updateSerialDevicesTimer->start(5000);
	connect(updateSerialDevicesTimer, SIGNAL(timeout()), this,
										SLOT(updateSerialDevices()));

	//read the screensize
	QScreen *screen = QApplication::screens().at(0);
	screenWidth  = screen->availableSize().width();
	screenHeight = screen->availableSize().height();

	//**************** BLUETOOTH ******************************************************

#if Bluetooth

	/*
	 * read in all local Bluetooth adapters
	 */

	localBluetoothAdapters = QBluetoothLocalDevice::allDevices();

	/*
	 * if there is no Bluetooth adapter available hide all Bluetooth components
	 * in the Mainwindow
	 *
	 * otherwise:
	 *			list all adapters in a ComboBox
	 *			read the MAC-Adress of the Bluetooth and write it into a label
	 *			turn on the Power of the Bluetooth if it isn't
	 *			make the Bluetoothadapter discoverable if it isn't
	 *
	*/

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
			ui->comboBox_BT_local_Bluetooth_Adapter->
					addItem(localBluetoothAdapters.at(i).name());
		}

		QBluetoothLocalDevice localDevice (localBluetoothAdapters.at(
					ui->comboBox_BT_local_Bluetooth_Adapter->currentIndex()).address());

		ui->label_BT_status_connection->
				setText("Adresse: " + localDevice.address().toString());

		if(localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff)
		{
			localDevice.powerOn();
		}

		if(localDevice.hostMode() == !QBluetoothLocalDevice::HostDiscoverable)
		{
			localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
		}
	}

	/*
	 * If there is a bluetooth adapter available on the host, create a bluetooth server
	 *
	 * Conect some signals of the Server with methods of the MainWindow:
	 *
	 *		SIGNAL:					SLOT:
	 *		clientConnected		->	clientConnected
	 *		clientDisconnected	->	clientDisconnected
	 *		messageReceived		->	BluetoothCommandReceived
	 */

	if(localBluetoothAdapters.size() != 0)
	{

		BluetoothServer = new BluetoothTransmissionServer(this);

		//******* CONNECTS ************************************************************

		connect(BluetoothServer,SIGNAL(clientConnected(QString)), this,
								SLOT(clientConnected(QString)));
		connect(BluetoothServer,SIGNAL(clientDisconnected(QString)), this,
								SLOT(clientDisconnected(QString)));
		connect(BluetoothServer,SIGNAL(messageReceived(QString,QString)), this,
								SLOT(BluetoothCommandReceived(QString,QString)));

		//*****************************************************************************


		//Start the Server
		BluetoothServer->startServer();
	}

#else

	/*
	 * if no bluetooh is available on the host, because the host doesn't have a USB-Port
	 * or any other reason
	 *
	 * then set the Macro Bluetooth to 0
	 * now the application doesn't have Bluetooth so you only can controll the maschine
	 * with the touch display
	 */

	ui->label_BT_clients->setVisible(false);
	ui->label_BT_headline->setVisible(false);
	ui->label_BT_status_connection->setVisible(false);
	ui->listWidget_BT_Clienten->setVisible(false);
	ui->comboBox_BT_local_Bluetooth_Adapter->setVisible(false);

#endif
	//**********************************************************************************

	readMixtures();			//read in the mixtures
	writeListWidget();		//write the mixtures into the listWidget
	updateSerialDevices();	//update the serial Devices first time


	/*
	 * Set the Container amounts:
	 *
	 * the Status Bar needs a value in percent.
	 * so the actual value must be calculated.
	 *
	 * first the actual fill value is restored from the variable settings. then the max
	 * fill value is restored
	 * after the actual value is devided by the max value an multiplyd with 100 so
	 * the equal value is in percent
	 *
	 * all those values are delivered to the method setContainer amounts
	 */

	setConatinerAmounts(100 * ((double)settings.value("ContainerVolume_1", "").toInt() /
							   (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_2", "").toInt() /
							   (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_3", "").toInt() /
							   (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_4", "").toInt() /
							   (double)settings.value("ContainerVolume", "").toInt()));
}

MainWindow::~MainWindow()
{
	serial.close();						//close the serial port
	BluetoothServer->stopServer();		//stop the Bluetooh server
	delete ui;
}

void MainWindow::updateTime(void)
{
	//Update the time in the statusbar every second

	QString time;

	//read out the system time in a special String format
	time = QDateTime::currentDateTime().toString("dd.MM.yyyy	hh:mm:ss");

	ui->statusBar->showMessage(time);	//set the time in the status bar
}

void MainWindow::updateSerialDevices(void)
{
	//Update the serial devices every 5 seconds

	if(serial.isOpen() == false && ui->comboBox_serial->isMaximized() == false)
	{
		ui->comboBox_serial->clear();

		//add all names of the available serial ports to the ComboBox
		foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
		{
			ui->comboBox_serial->addItem(info.portName());
		}
	}
}

void MainWindow::on_actionMischungen_bearbeiten_triggered()
{
	/*
	 * generate Object menu_mix, thats the Window to add, delete or edit the mixtures
	 */

	Menu_Mix menu_mix;
	menu_mix.setWindowTitle("Mischungen bearbeiten");	//set the Window Title

	/*
	 * set the Container names in the Window, restore the names form the settings
	 */

	menu_mix.setContainerNames(settings.value("ContainerName_1", "").toString(),
							   settings.value("ContainerName_2", "").toString(),
							   settings.value("ContainerName_3", "").toString(),
							   settings.value("ContainerName_4", "").toString());

	//deliver all current available mixtures
	menu_mix.setMixtures(mixtures);

	//deliver the max glass volume
	menu_mix.setMaxVolume(settings.value("MaxVolume", "").toInt());

	/*
	 * open the window and wait until it's accepted
	 * if it's accepted read out the new mixture list and save the list
	 *
	 * if the window is canceled, nothing have to be done
	 */
	if(menu_mix.exec() == QDialog::Accepted)
	{
		for(int i=0; i<mixtures.size(); i++)
		{
			disconnect(ui->listWidget->itemWidget(ui->listWidget->item(i)),
							SIGNAL(clicked(bool)), mixtures.at(i),
							SLOT(getCommandValues()));

			disconnect(mixtures.at(i), SIGNAL(sendCommandValues(QList<int>)), this,
									SLOT(ButtonSlot(QList<int>)));
		}

		mixtures.clear();
		/*this->*/ mixtures = menu_mix.getMixtures();
		saveMixtures();
	}
	writeListWidget();
}

void MainWindow::on_action_ber_triggered()
{
	//open a Message Box

	QMessageBox::information(this, "Automatische Abfüllanlage",
							 "Version: 0.1\nEntwickler: Stefan Wögerbauer");
}

void MainWindow::on_actionBeenden_triggered()
{
	//close the application
	this->close();
}

void MainWindow::on_actionEinstellungen_triggered()
{
	menu Menu;								//generate the object
	Menu.setWindowTitle("Einstellungen");	//Set the Window Title

	//set the current container names, retore them from the setiings
	Menu.setContainerNames(settings.value("ContainerName_1", "").toString(),
						   settings.value("ContainerName_2", "").toString(),
						   settings.value("ContainerName_3", "").toString(),
						   settings.value("ContainerName_4", "").toString());

	//set the current stored max glass volume
	Menu.setMaxVolume(settings.value("MaxVolume", "").toInt());
	//set the current stored volume of the container
	Menu.setContainerVolume(settings.value("ContainerVolume", "").toInt());

	//execute the window
	if(Menu.exec() == QDialog::Accepted)
	{
		/*
		 * if the Window is accepted save all the new values and names
		 */

		settings.setValue("ContainerName_1", Menu.getContainerName_1());
		settings.setValue("ContainerName_2", Menu.getContainerName_2());
		settings.setValue("ContainerName_3", Menu.getContainerName_3());
		settings.setValue("ContainerName_4", Menu.getContainerName_4());
		settings.setValue("MaxVolume",		 Menu.getMaxVolume());
		settings.setValue("ContainerVolume", Menu.getContainerVolume());

		/*
		 * if the current fill amount of container 1 is bigger than the max
		 * container volume, set the current fill value to the max container value
		*/
		if(settings.value("ContainerVolume_1", "").toInt() >
				settings.value("ContainerVolume", "").toInt())
		{
			settings.setValue("ContainerVolume_1",
							  settings.value("ContainerVolume", "").toInt());
		}

		/*
		 * if the current fill amount of container 2 is bigger than the max
		 * container volume, set the current fill value to the max container value
		*/
		if(settings.value("ContainerVolume_2", "").toInt() >
				settings.value("ContainerVolume", "").toInt())
		{
			settings.setValue("ContainerVolume_2",
							  settings.value("ContainerVolume", "").toInt());
		}

		/*
		 * if the current fill amount of container 3 is bigger than the max
		 * container volume, set the current fill value to the max container value
		*/
		if(settings.value("ContainerVolume_3", "").toInt() >
				settings.value("ContainerVolume", "").toInt())
		{
			settings.setValue("ContainerVolume_3",
							  settings.value("ContainerVolume", "").toInt());
		}

		/*
		 * if the current fill amount of container 4 is bigger than the max
		 * container volume, set the current fill value to the max container value
		*/
		if(settings.value("ContainerVolume_4", "").toInt() >
				settings.value("ContainerVolume", "").toInt())
		{
			settings.setValue("ContainerVolume_4",
							  settings.value("ContainerVolume", "").toInt());
		}

		//calculate the new contianer volumes for the Status Bar
		calculateContainerVolumes(0, 0, 0, 0);
	}
}

void MainWindow::sendSerialCommand(QString sender ,QString command)
{
	/*
	 * send the serial command and an '\n' on the end so that the end of the String
	 * is marked
	 *
	 */

	if(serial.isOpen() == true)
	{
		serial.write(command.toLocal8Bit() + '\n');

		//start the timer for the serial timout, in this time the slave has to answer
		//serialTimeOut->start(serialTimeOutTime_ms);

		QString time;
		//read the current system time, write it into a sting in a special format
		time = QDateTime::currentDateTime().toString("hh:mm:ss");

		//Write the sendet command in a List Widget of the serial traffic
		QListWidgetItem *newItem = new QListWidgetItem;
		newItem->setText(time + " " + sender + ": " + command);
		newItem->setTextColor(Qt::blue);
		ui->listWidget_serial_traffic->insertItem(1, newItem);

		//TODO

		qDebug() << "send Serial Command: " << command;
	}
	else
	{
		QMessageBox::critical(this, "Automatische Abfüllanlage", "Serial Port Error");
	}
}

void MainWindow::readSerialCommand(void)
{
	if(serial.isOpen() == true)
	{
		response.clear();				//delete the Byte Array before reading the new data
		response.append(serial.readAll());	//read the received data

		if(response.length() > 0)
		{
			QString time;
			//read the current system time, write it into a string in a special format
			time = QDateTime::currentDateTime().toString("hh:mm:ss");

			//write the receved data in the listWidget of the serial traffic
			QListWidgetItem *newItem = new QListWidgetItem;
			newItem->setText(time + "Client: " + response);
			newItem->setTextColor(Qt::red);
			ui->listWidget_serial_traffic->insertItem(1, newItem);

			if(response == "NH")
			{
				QMessageBox::critical(this, "Automatische Abfüllanlage",
									  "Not-Halt wurde betätigt oder eine "
									  "Störung ist aufgetreten.\n Um die Anlage zu "
									  "Quitieren wählen Sie bitte eine neue Mischung");
			}

			//TODO: do some checks

			if(commandSended == true && response == "R")	//'R' for received
			{
				currentlyWorking =	true;
				commandSended =		false;
				commandReceived =	true;

				qDebug() << "serial command received";
				qDebug() << "maschine is working";

				//TODO: calculate the new container amounts
			}

			if(commandReceived == true && response == "Y") //'Y' for ready
			{
				currentlyWorking =	false;
				commandReceived =	false;

				qDebug() << "ready for the next mix";
			}

#if Bluetooth
			//*************************************************************************
			//Bluetooth

			//if there is a Bluetooth adapeter available
			if(localBluetoothAdapters.size() == 0)
			{
				//send the response to all clients
				BluetoothServer->sendMessage(response);
			}
			//*************************************************************************
#endif

			qDebug() << "read Serial Command: " << response.trimmed();
		}
		else
		{
			qDebug()<<"no traffic";
		}
	}
}

void MainWindow::on_pushButton_connect_clicked()
{
	/*
	 * open the serial Port
	 *
	 * Port Name:	User choose the correct Port, on the RPI it's "dev/ttyS0"
	 * Baudrate:	4800
	 * Parity:		No Parity Bit
	 * Data lenght:	8 Bit
	 * Port Mode:	Read and Write
	 *
	 */

	//count the clicks on the button to difference "connect" and "disconnect"
	static int i=0;

	if(i%2 == 0) //if port is open --> close it, else open the port
	{
		if (serial.portName() != "/dev/pts/6")
		//if (serial.portName() != ui->comboBox_serial->currentText())
		{
			serial.close();								//close the serial port
			serial.setBaudRate(QSerialPort::Baud4800);	//set the Baudrate
			serial.setParity(QSerialPort::NoParity);	//set the Parity
			serial.setDataBits(QSerialPort::Data8);		//set the data lenght
			//serial.setPortName(ui->comboBox_serial->currentText()); //set the portname
			serial.setPortName("/dev/pts/6");
		}

		if (!serial.open(QIODevice::ReadWrite))	//open the serial port
		{
			qDebug() << "can't open serial Port";
			//open a critical message box: error on the port
			QMessageBox::critical(this, "Automatische Abfüllanlage",
								  tr("%1 konnte nicht geöffnet werden, error code: %2")
								  .arg(serial.portName()).arg(serial.error()));

			return;
		}
		else
		{
			/*
			 * if the port is opened succesfully change the button text to "trennen"
			 * disable the comboBox of the serial devices
			 * set the label
			 */

			ui->pushButton_connect->setText("Trennen");
			ui->comboBox_serial->setEnabled(false);
			qDebug()<< "connect to serial Port";
			ui->label_status->setText(tr("Verbunden mit Port: %1")
									  .arg(ui->comboBox_serial->currentText()));
		}

		i++;	//increase the botton clicks
	}
	else	//disconnect the serial port
	{
		qDebug()<< "close serial Port";
		serial.close();									//close the serial port
		i++;											//increase the botton clicks
		ui->pushButton_connect->setText("Verbinden");	//change the button text
		ui->label_status->setText("getrennt");			//set the label
		ui->comboBox_serial->setEnabled(true);			//enable the combo box of the..
														//serial devices
	}
}

void MainWindow::on_pushButton_clearSerialTraffic_clicked()
{
	/*
	 * delete all text of the serial Data Traffic
	 */

	ui->listWidget_serial_traffic->clear(); //delete all entries of the List Widget
}

void MainWindow::saveMixtures (void)
{
	/*
	 * store the different Mixes in QSettings in an Data Array
	 */

	settings.remove("MixtureList");				//delete the old List
	settings.beginWriteArray("MixtureList");

	for(int i=0; i<mixtures.size(); i++)
	{
		settings.setArrayIndex(i);				//set the index of the Array
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
	/*
	 * read the different Mixes from QSettings
	 * store the values of the liquid amounts from each container and convert them into
	 * integer, after save them in a QList
	 * load the Name of the Mix and save them in a QList
	 */

	//get the size of the Array
	int size_Mixtures = settings.beginReadArray("MixtureList");

	for(int i=0; i<size_Mixtures; i++)
	{
		settings.setArrayIndex(i);			//set the index of the Array

		mixtures.append(new mixture);		//add a new Objet to the List

		//set the data of the new Object
		mixtures.last()->setName(settings.value("Name","").toString());
		mixtures.last()->setAmountContainer_1(
					settings.value("Container_1", "").toInt());
		mixtures.last()->setAmountContainer_2(
					settings.value("Container_2", "").toInt());
		mixtures.last()->setAmountContainer_3(
					settings.value("Container_3", "").toInt());
		mixtures.last()->setAmountContainer_4(
					settings.value("Container_4", "").toInt());
	}
	settings.endArray();
}

void MainWindow::writeListWidget(void)
{
	/*
	 * write the List Widget
	 * insert all Mixtures
	 */

	ui->listWidget->clear();	//delete the list Widget
	qDebug() << "Write List Widget Mixtures size" << mixtures.size();

	for(int i = 0; i<mixtures.size(); i++)
	{
		QPushButton *button = new QPushButton;		//generate a new button
		button->setText(mixtures.at(i)->getName());	//set the text of Button

		QListWidgetItem *item = new QListWidgetItem;
		QSize size;
		size.setHeight(40);							//set the height of the button
		item->setSizeHint(size);
		ui->listWidget->addItem(item);				//add a new item to the listWidget
		ui->listWidget->setItemWidget(item, button);//set the buttons item

		/*
		 * connect the signal of the button clicked with the slot of the mixutre
		 * getCommandValues
		 *
		 * when the button is clicked the Object mixture generates a list of the amounts
		 * of the liquid of each container which should filled in the glass
		 *
		 * this Slot emits a signal (sendCommandValues) which contains the List with
		 * the values
		 *
		 * This Signal is connected to the slot (ButtonSlot)
		 * There the data is going to be checked and sendet if everything is OK
		 */

		connect(button, SIGNAL(clicked(bool)), mixtures.at(i),
						SLOT(getCommandValues()));

		connect(mixtures.at(i), SIGNAL(sendCommandValues(QList<int>)), this,
								SLOT(ButtonSlot(QList<int>)));
	}
}

void MainWindow::ButtonSlot(QList<int> valueList)
{
	// if serial Port is open and the mashine isn't working send the new command
	if(serial.isOpen() && currentlyWorking == false)
	{
		if(settings.value("ContainerVolume_1", "").toInt() - valueList.at(0) < 0)
		{
			/*
			 * Not enough liquid in container 1
			 * sent this message to all Bluetooth Clients
			 * also show it in the UI
			 */

			BluetoothServer->sendMessage("_Nicht genügend "  +
									settings.value("ContainerName_1","") .toString() +
									" in Behälter 1 vorhanden.\nBitte befüllen Sie "
									"diesen vollständig und klicken Sie auf den Button "
									"unter der Grafik des jeweiligen Behälters");


			QMessageBox::critical(this, "Automatische Abfüllanlage", "Nicht genügend " +
								  settings.value("ContainerName_1","") .toString() +
								  " in Behälter 1 vorhanden.\nBitte befüllen Sie "
								  "diesen vollständig und klicken Sie auf den Button "
								  "unter der Grafik des jeweiligen Behälters");

		}
		else if(settings.value("ContainerVolume_2", "").toInt() - valueList.at(1) < 0)
		{
			/*
			 * Not enough liquid in container 2
			 * sent this message to all Bluetooth Clients
			 * also show it in the UI
			 */

			BluetoothServer->sendMessage("_Nicht genügend "  +
									settings.value("ContainerName_2","") .toString() +
									" in Behälter 2 vorhanden.\nBitte befüllen Sie "
									"diesen vollständig und klicken Sie auf den Button "
									"unter der Grafik des jeweiligen Behälters");


			QMessageBox::critical(this, "Automatische Abfüllanlage", "Nicht genügend " +
									settings.value("ContainerName_2","") .toString() +
									" in Behälter 2 vorhanden.\nBitte befüllen Sie "
									"diesen vollständig und klicken Sie auf den Button "
									"unter der Grafik des jeweiligen Behälters");

		}
		else if(settings.value("ContainerVolume_3", "").toInt() - valueList.at(2) < 0)
		{
			/*
			 * Not enough liquid in container 3
			 * sent this message to all Bluetooth Clients
			 * also show it in the UI
			 */

			BluetoothServer->sendMessage("_Nicht genügend "  +
									settings.value("ContainerName_3","") .toString() +
									" in Behälter 3 vorhanden.\nBitte befüllen Sie "
									"diesen vollständig und klicken Sie auf den Button "
									"unter der Grafik des jeweiligen Behälters");


			QMessageBox::critical(this, "Automatische Abfüllanlage", "Nicht genügend " +
									settings.value("ContainerName_3","") .toString() +
									" in Behälter 3 vorhanden.\nBitte befüllen Sie "
									"diesen vollständig und klicken Sie auf den Button "
									"unter der Grafik des jeweiligen Behälters");
		}
		else if(settings.value("ContainerVolume_4", "").toInt() - valueList.at(3) < 0)
		{
			/*
			 * Not enough liquid in container 4
			 * sent this message to all Bluetooth Clients
			 * also show it in the UI
			 */

			BluetoothServer->sendMessage("_Nicht genügend "  +
									settings.value("ContainerName_4","") .toString() +
									" in Behälter 4 vorhanden.\nBitte befüllen Sie "
									"diesen vollständig und klicken Sie auf den Button "
									"unter der Grafik des jeweiligen Behälters");

			QMessageBox::critical(this, "Automatische Abfüllanlage", "Nicht genügend " +
									settings.value("ContainerName_4","") .toString() +
									" in Behälter 4 vorhanden.\nBitte befüllen Sie "
									"diesen vollständig und klicken Sie auf den Button "
									"unter der Grafik des jeweiligen Behälters");
		}
		else
		{
			/*
			 * send the values of the mix to the maschine
			 * data length = 16
			 * for example: #001/002/003/004
			 *				 001 is the amount of liquid of conatiner 1 in ml
			 *				 002 is the amount of liquid of container 2 in ml
			 *				 ....
			 */

			sendSerialCommand("Host", "#" +
							  QString("%1").arg(valueList.at(0)*10 ,3,'g',-1,'0') + "/"+
							  QString("%1").arg(valueList.at(1)*10 ,3,'g',-1,'0') + "/"+
							  QString("%1").arg(valueList.at(2)*10 ,3,'g',-1,'0') + "/"+
							  QString("%1").arg(valueList.at(3)*10 ,3,'g',-1,'0'));

			commandSended = true;

			//calculate the new container values
			calculateContainerVolumes(valueList.at(0),
									  valueList.at(1),
									  valueList.at(2),
									  valueList.at(3));
		}
	}
	else if(!serial.isOpen())
	{
		/*
		 * serial Port isn't open
		 * Message: Port not open, please connect the Bar-Roboter
		 */

		BluetoothServer->sendMessage(
					"_Bitte stellen Sie eine Verbindung zur Anlage her");
		QMessageBox::critical(this, "Automatische Anfüllanlage",
							  "Bitte stellen Sie eine Verbindung zur Anlage her");
	}
	else if(currentlyWorking == true)
	{
		/*
		 * maschine is working
		 */

		BluetoothServer->sendMessage(
					"_Bitte warten Sie bis der Vorgang abgeschlossen ist.");
		QMessageBox::critical(this, "Automatische Anfüllanlage",
							  "Bitte warten Sie bis der Vorgang abgeschlossen ist.");
	}
	else
	{
		/*
		 * unknown error
		 */

		BluetoothServer->sendMessage("_Ein unbekannter Fehler ist aufgetreten");
		QMessageBox::critical(this, "Automatische Anfüllanlage", "unbekannter Fehler");
	}
}

void MainWindow::on_comboBox_BT_local_Bluetooth_Adapter_currentIndexChanged(int index)
{
	//TODO: check why this is uncommented
	//BluetoothServer->stopServer();

	QBluetoothLocalDevice localDevice (localBluetoothAdapters.at(
					ui->comboBox_BT_local_Bluetooth_Adapter->currentIndex()).address());

	ui->label_BT_status_connection->setText(
				"Adresse: "+localDevice.address().toString());

	if(localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff)
	{
		localDevice.powerOn();
	}

	if(localDevice.hostMode() == !QBluetoothLocalDevice::HostDiscoverable)
	{
		localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
	}
	//TODO: check why this is uncommented
	//BluetoothServer->startServer();
}

#if Bluetooth

void MainWindow::clientConnected(const QString &name)
{
	/*
	 * Client connected
	 */

	QListWidgetItem *item = new QListWidgetItem;
	item->setText(name);		//set the name

	/*
	 * add the name of the new client to the list widget
	 */
	ui->listWidget_BT_Clienten->addItem(item);
}

void MainWindow::clientDisconnected(const QString &name)
{
	/*
	 * Client disconnected
	 */

	QMessageBox::information(this, "Automatische Abfüllanlage",
							 name + " hat die Verbindung getrennt!");
}

void MainWindow::BluetoothCommandReceived(QString client, QString command)
{
	//command begins with "/" it's a command
	if(command.at(0) == '/')
	{
		int count = 0;

		for(int i = 0; i< mixtures.size(); i++)
		{
			QString mix = "/" + mixtures.at(i)->getName();
			if(mix == command)		//find out which mix is choosen
			{
				QList<int> valueList;
				valueList.append(mixtures.at(i)->getAmountContainer_1());
				valueList.append(mixtures.at(i)->getAmountContainer_2());
				valueList.append(mixtures.at(i)->getAmountContainer_3());
				valueList.append(mixtures.at(i)->getAmountContainer_4());

				ButtonSlot(valueList);	//send commmand via the ButtonSlot to Mashine

				count++;
			}
		}

		//if the mix isn't in the list send an error
		if(count != 1)
		{
			BluetoothServer->sendMessage("_ERROR");
			//return;
		}
	}
	//command begins with "_" it's a informaion Request
	else if(command.at(0) == '_')
	{
		//send back information via Bluetooth
		qDebug()<<"send back command: " << command;
		if(command == "_sendBackMixes")
		{
			sendMixes();		//send the names of the mixes to BT-Client
		}
	}
	else
	{
		//send back error via Bluetooth
		BluetoothServer->sendMessage("_ERROR");

		QMessageBox::critical(this, "Automatische Abfüllanlage",
							  "Error: Befehl konnte nicht gelesen werden");
	}
}

void MainWindow::sendMixes(void)
{
	for(int i=0; i < mixtures.size(); i++)
	{
		//send the name of the mixes via Bluetooth
		BluetoothServer->sendMessage(mixtures.at(i)->getName());
	}
	BluetoothServer->sendMessage("ready");
}

#endif

void MainWindow::setConatinerAmounts(int Container_1,
									 int Container_2,
									 int Container_3,
									 int Container_4)
{
	//set the values of the progressBars
	ui->progressBar_Contaner_1->setValue(Container_1);
	ui->progressBar_Contaner_2->setValue(Container_2);
	ui->progressBar_Contaner_3->setValue(Container_3);
	ui->progressBar_Contaner_4->setValue(Container_4);
}

void MainWindow::calculateContainerVolumes(int ConsumtionContainer_1,
										   int ConsumtionContainer_2,
										   int ConsumtionContainer_3,
										   int ConsumtionContainer_4)
{
	/*
	 * calculate the new container liquid amounts with the stored amounts of the
	 * different mixed
	 *
	 * Set the new container values in percent
	 */

	settings.setValue("ContainerVolume_1",
					  settings.value("ContainerVolume_1", "").toInt() -
										ConsumtionContainer_1);
	settings.setValue("ContainerVolume_2",
					  settings.value("ContainerVolume_2", "").toInt() -
										ConsumtionContainer_2);
	settings.setValue("ContainerVolume_3",
					  settings.value("ContainerVolume_3", "").toInt() -
										ConsumtionContainer_3);
	settings.setValue("ContainerVolume_4",
					  settings.value("ContainerVolume_4", "").toInt() -
										ConsumtionContainer_4);

	setConatinerAmounts(100 * ((double)settings.value("ContainerVolume_1", "").toInt() /
							   (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_2", "").toInt() /
							   (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_3", "").toInt() /
							   (double)settings.value("ContainerVolume", "").toInt()),
						100 * ((double)settings.value("ContainerVolume_4", "").toInt() /
							   (double)settings.value("ContainerVolume", "").toInt()));

}

void MainWindow::on_pushButton_fill_C1_clicked()
{
	/*
	 * Fill container 1
	 * Set Value of the progress Bar 100, this value is in percent
	 * This Button gets pressed when the container gets filled with some liquid
	 * It is important that the container gets filled full
	 */

	settings.setValue("ContainerVolume_1",
					  settings.value("ContainerVolume","").toInt());
	ui->progressBar_Contaner_1->setValue(100);

}

void MainWindow::on_pushButton_fill_C2_clicked()
{
	/*
	 * Fill container 2
	 * Set Value of the progress Bar 100, this value is in percent
	 * This Button gets pressed when the container gets filled with some liquid
	 * It is important that the container gets filled full
	 */

	settings.setValue("ContainerVolume_2",
					  settings.value("ContainerVolume","").toInt());
	ui->progressBar_Contaner_2->setValue(100);

}

void MainWindow::on_pushButton_fill_C3_clicked()
{
	/*
	 * Fill container 3
	 * Set Value of the progress Bar 100, this value is in percent
	 * This Button gets pressed when the container gets filled with some liquid
	 * It is important that the container gets filled full
	 */

	settings.setValue("ContainerVolume_3",
					  settings.value("ContainerVolume","").toInt());
	ui->progressBar_Contaner_3->setValue(100);
}

void MainWindow::on_pushButton_fill_C4_clicked()
{
	/*
	 * Fill container 4
	 * Set Value of the progress Bar 100, this value is in percent
	 * This Button gets pressed when the container gets filled with some liquid
	 * It is important that the container gets filled full
	 */

	settings.setValue("ContainerVolume_4",
					  settings.value("ContainerVolume","").toInt());
	ui->progressBar_Contaner_4->setValue(100);
}


void MainWindow::on_pushButton_empty_container_1_clicked()
{
		if(QMessageBox::question(this, "Automatische Abfüllanlage",
								 "Wollen Sie den Behälter 1 wirklich entleeren ?")
				== QMessageBox::Yes)
		{
			QMessageBox::information(this, "Automatische Abfüllanlage",
									 "Bitte Stellen Sie ein geeignetes Gefäß "
									 "unter die Abfüllstation 1 und klicken "
									 "Sie anschlißend auf OK");

			if(serial.isOpen() == true)
			{
				sendSerialCommand("Host", "EC1");
				ui->progressBar_Contaner_1->setValue(0);
				qDebug() << "empty container 1";
			}
			else if (serial.isOpen() == false)
			{
				QMessageBox::critical(this, "Automatische Abfüllanlage",
								"Bitte stellen Sie eine Verbindung zur Anlage her!");
			}
		}
}

void MainWindow::on_pushButton_empty_container_2_clicked()
{

	if(QMessageBox::question(this, "Automatische Abfüllanlage",
							 "Wollen Sie den Behälter 2 wirklich entleeren ?")
			== QMessageBox::Yes)
	{
		QMessageBox::information(this, "Automatische Abfüllanlage",
								 "Bitte Stellen Sie ein geeignetes Gefäß "
								 "unter die Abfüllstation 2 und klicken "
								 "Sie anschlißend auf OK");

		if(serial.isOpen() == true)
		{
			sendSerialCommand("Host", "EC2");
			ui->progressBar_Contaner_1->setValue(0);
			qDebug() << "empty container 2";
		}
		else if (serial.isOpen() == false)
		{
			QMessageBox::critical(this, "Automatische Abfüllanlage",
							"Bitte stellen Sie eine Verbindung zur Anlage her!");
		}
	}
}

void MainWindow::on_pushButton_emptycontainer_3_clicked()
{

	if(QMessageBox::question(this, "Automatische Abfüllanlage",
							 "Wollen Sie den Behälter 3 wirklich entleeren ?")
			== QMessageBox::Yes)
	{
		QMessageBox::information(this, "Automatische Abfüllanlage",
								 "Bitte Stellen Sie ein geeignetes Gefäß "
								 "unter die Abfüllstation 3 und klicken "
								 "Sie anschlißend auf OK");

		if(serial.isOpen() == true)
		{
			sendSerialCommand("Host", "EC3");
			ui->progressBar_Contaner_1->setValue(0);
			qDebug() << "empty container 3";
		}
		else if (serial.isOpen() == false)
		{
			QMessageBox::critical(this, "Automatische Abfüllanlage",
							"Bitte stellen Sie eine Verbindung zur Anlage her!");
		}
	}
}

void MainWindow::on_pushButton_empty_container_4_clicked()
{

	if(QMessageBox::question(this, "Automatische Abfüllanlage",
							 "Wollen Sie den Behälter 4 wirklich entleeren ?")
			== QMessageBox::Yes)
	{
		QMessageBox::information(this, "Automatische Abfüllanlage",
								 "Bitte Stellen Sie ein geeignetes Gefäß "
								 "unter die Abfüllstation 4 und klicken "
								 "Sie anschlißend auf OK");

		if(serial.isOpen() == true)
		{
			sendSerialCommand("Host", "EC4");
			ui->progressBar_Contaner_1->setValue(0);
			qDebug() << "empty container 4";
		}
		else if (serial.isOpen() == false)
		{
			QMessageBox::critical(this, "Automatische Abfüllanlage",
							"Bitte stellen Sie eine Verbindung zur Anlage her!");
		}
	}
}
