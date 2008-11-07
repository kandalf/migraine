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
	
	/*
	QFile file("data/Database1.mdb");
	if (file.open(QIODevice::ReadOnly)) {
		logTextEdit->setText("Data file readable");
		file.close();
	} else {
		logTextEdit->setText("Cannot open data file");
		logTextEdit->append(file.errorString());
	}*/
		
}

void MigraineMainWindow::showConnectionDialog()
{
	/*ConnectionDialog *connDialog = new ConnectionDialog(this);
	connDialog->show();*/
	
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
	db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=data/Database1.mdb");
	db.open();
	if (db.isOpen())
		logTextEdit->setText("Open");
	else
		logTextEdit->setText("Canot open db");
		
}
//
