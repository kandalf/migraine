#include <QSettings>
#include "migrainemainwindow.h"
#include "connectiondialog.h"


ConnectionDialog::ConnectionDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);
	this->saved = false;
	driversComboBox->addItems(QSqlDatabase::drivers());
	splitter->setStretchFactor(0, 1);
	connect( this, SIGNAL(accepted()), this, SLOT(writeSettings()) );
}

void ConnectionDialog::checkConnectionFields()
{
	addConnectionButton->setEnabled(
			(!hostLineEdit->text().isEmpty() && !hostLineEdit->text().isNull()) &&
			(!databaseLineEdit->text().isEmpty() && !databaseLineEdit->text().isNull()) &&
			(!driversComboBox->itemText(driversComboBox->currentIndex()).isEmpty() && !driversComboBox->itemText(driversComboBox->currentIndex()).isNull()) 
		);
}

void ConnectionDialog::addConnection()
{
		QString connectionName(hostLineEdit->text() + "-" + databaseLineEdit->text());
		QString driverName(driversComboBox->itemText(driversComboBox->currentIndex()));
		
	 	QSqlDatabase db = QSqlDatabase::addDatabase(
			driverName,
			connectionName
	);
	
	if (driverName.contains("ODBC")) {
		QString dbName = "DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1";
		db.setDatabaseName(dbName.arg(databaseLineEdit->text()));
	} else {
		db.setDatabaseName(databaseLineEdit->text());
		db.setHostName(hostLineEdit->text());
		db.setUserName(userLineEdit->text());
		db.setPassword(passwordLineEdit->text());
	}
	
	connListWidget->addItem(new QListWidgetItem(hostLineEdit->text() + "-" + databaseLineEdit->text(), connListWidget, QListWidgetItem::Type));
}


void ConnectionDialog::readSettings()
{
	
}

void ConnectionDialog::writeSettings()
{
	QSettings settings("conf/settings.ini", QSettings::IniFormat);
	
	settings.beginGroup("Connections");
	
	for (int i = 0; i < QSqlDatabase::connectionNames().size(); i++)
	{
		QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::connectionNames().at(i));
		settings.beginGroup("Connections/" + db.connectionName());
		//settings.setValue("name", db.connectionName());
		settings.setValue("hostname", db.hostName());
		settings.setValue("username", db.userName());
		settings.setValue("password", db.password());
		settings.setValue("driver", db.driverName());
		settings.endGroup();
	}
	
	settings.endGroup();
	
	settings.sync();
	emit(settingsWritten());
}
