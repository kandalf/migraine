#ifndef __CONNECTIONDIALOG_H__
#define __CONNECTIONDIALOG_H__

#include <QDialog>
#include <QSqlDatabase>
#include <QHash>
#include "ui_connectiondialog.h"
#include "types.h"

class ConnectionListItem;

class ConnectionDialog : 
	public QDialog, Ui::ConnectionDialog
{
	Q_OBJECT
	public:
		ConnectionDialog(QWidget *parent = 0, Qt::WFlags f = 0);
		
	public slots:
		void readSettings();
		void writeSettings();
		
	signals:
		void settingsWritten();
		
	private slots:
		void checkConnectionFields();
		void addConnection();
		void saveConnection();
		void deleteConnection();
		void itemConnectionSelected(QListWidgetItem*);
		void addDbConnection(ConnectionSettings*);
                void checkSelectedDriver(const QString &driverName);
                void browseFiles();
		
	private:
		void setupObjectConnections();
                void setupDriversList();
                void findDriverInList(const QString &driverName);

		QHash<QString, QSqlDatabase>connectionHash;
		bool saved;
};

#endif // __CONNECTIONDIALOG_H__
