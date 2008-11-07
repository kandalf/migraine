#include <QSqlDatabase>
#include "migrainemainwindow.h"
#include "connectiondialog.h"
//
MigraineMainWindow::MigraineMainWindow( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	dbSrcConnCombo->addItems(QSqlDatabase::connectionNames());
	hSplitter->setStretchFactor(0,1);
}

void MigraineMainWindow::showConnectionDialog()
{
	ConnectionDialog *connDialog = new ConnectionDialog(this);
	connDialog->show();
	/*
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
	db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=database.mdb");
	db.open();
	if (db.isOpen())
		logTextEdit->setText("Open");
	else
		logTextEdit->setText("Canot open db");
		*/
}
//
