#include <QSqlDatabase>
#include <QStringListModel>
#include <QSqlError>

#include "migrainemainwindow.h"
#include "connectiondialog.h"

MigraineMainWindow::MigraineMainWindow( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	setupObjectConnections();
	refreshConnections();
	hSplitter->setStretchFactor(0,1);
	connDialog = 0;
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

MigraineMainWindow::~MigraineMainWindow()
{
	for (int i = 0; i < QSqlDatabase::connectionNames().size(); i++)
	{
		if (QSqlDatabase::database(QSqlDatabase::connectionNames().at(i)).isOpen())
			QSqlDatabase::database(QSqlDatabase::connectionNames().at(i)).close();
	}
}

void MigraineMainWindow::setupObjectConnections()
{
	connect( dbSrcConnCombo, SIGNAL(activated(const QString &)), this, SLOT(connectionSelected(const QString&)) );
}

void MigraineMainWindow::showConnectionDialog()
{
	if (!connDialog) {
		connDialog = new ConnectionDialog(this);
		connect( connDialog, SIGNAL(accepted()), this, SLOT(refreshConnections()) );
	}
		
	connDialog->show();
	/*
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
	db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=data/Database1.mdb");
	db.open();
	if (db.isOpen())
		logTextEdit->setText("Open");
	else
		logTextEdit->setText("Canot open db");
	
	db.close();*/
}

void MigraineMainWindow::refreshConnections()
{
	for (int i = 0; i < dbSrcConnCombo->count(); i++)
		dbSrcConnCombo->removeItem(i);
	dbSrcConnCombo->addItem("Select a connection");
	dbSrcConnCombo->addItems(QSqlDatabase::connectionNames());
	
	if (dbSrcConnCombo->count() > 1)
		dbSrcConnCombo->showPopup();
}

void MigraineMainWindow::connectionSelected(const QString &name)
{
	QSqlDatabase db = QSqlDatabase::database(name, true);
	if (!db.isOpen())
	{
		logTextEdit->append("Cannot Open Database: " + db.lastError().text());
	}
		
	QStringListModel *model = new QStringListModel(db.tables(QSql::Tables), connTablesView);
	connTablesView->setModel(model);
}
