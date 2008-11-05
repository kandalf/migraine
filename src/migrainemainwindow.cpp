#include <QSqlDatabase>
#include "migrainemainwindow.h"
#include "connectiondialog.h"
//
MigraineMainWindow::MigraineMainWindow( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	dbSrcConnCombo->addItems(QSqlDatabase::connectionNames());
}

void MigraineMainWindow::showConnectionDialog()
{
	ConnectionDialog *connDialog = new ConnectionDialog(this);
	connDialog->show();
}
//
