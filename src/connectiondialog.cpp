#include "connectiondialog.h"
#include <QSqlDatabase>

ConnectionDialog::ConnectionDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
	setupUi(this);
	driversComboBox->addItems(QSqlDatabase::drivers());
}
// place your code here
