#ifndef __CONNECTIONDIALOG_H__
#define __CONNECTIONDIALOG_H__

#include <QDialog>
#include "ui_connectiondialog.h"

class ConnectionDialog : 
	public QDialog, Ui::ConnectionDialog
{
	Q_OBJECT
	public:
		ConnectionDialog(QWidget *parent = 0, Qt::WFlags f = 0);
		
	private slots:
		void checkConnectionFields();
		void addConnection();
		
	private:
		bool saved;
};

#endif // __CONNECTIONDIALOG_H__
