#include "connectiondialog.h"
#include <QSqlDatabase>


ConnectionDialog::ConnectionDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);
	this->saved = false;
	driversComboBox->addItems(QSqlDatabase::drivers());
	splitter->setStretchFactor(0, 1);
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
	QSqlDatabase db = QSqlDatabase::addDatabase(
			driversComboBox->itemText(driversComboBox->currentIndex()),
			hostLineEdit->text() + "-" + databaseLineEdit->text()
	);
	
	db.setDatabaseName(databaseLineEdit->text());
	db.setHostName(hostLineEdit->text());
	db.setUserName(userLineEdit->text());
	db.setPassword(passwordLineEdit->text());
	
	connListWidget->addItem(new QListWidgetItem(hostLineEdit->text() + "-" + databaseLineEdit->text(), connListWidget, QListWidgetItem::Type));
}
// place your code here
