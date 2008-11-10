#ifndef __CONNECTIONLISTITEM_H__
#define __CONNECTIONLISTITEM_H__

#include <QListWidgetItem>
#include "types.h"

class ConnectionListItem 
	:public QListWidgetItem
{
	public:
		ConnectionListItem(QString text, QListWidget *parent = 0);
		ConnectionListItem(ConnectionSettings *settings, QListWidget *parent = 0);
		~ConnectionListItem();
		
		QString text();
		QString host();
		QString name();
		QString driver();
		QString user();
		QString password();
		QString database();
		qint16 port();
		ConnectionSettings *settings();
	private:
		ConnectionSettings *_settings;
};

#endif // __CONNECTIONLISTITEM_H__
