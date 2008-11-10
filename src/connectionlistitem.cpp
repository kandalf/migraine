#include "connectionlistitem.h"


ConnectionListItem::ConnectionListItem(QString text, QListWidget *parent) 
	:QListWidgetItem(text, parent, QListWidgetItem::UserType)
{
	_settings = new ConnectionSettings;
}

ConnectionListItem::ConnectionListItem(ConnectionSettings *settings, QListWidget *parent) 
{
	QListWidgetItem(settings->host + "-" + settings->name + "-" + settings->driver, parent, QListWidgetItem::UserType);
	_settings = settings;
}


ConnectionListItem::~ConnectionListItem()
{
	if (_settings)
		delete _settings;
}

QString ConnectionListItem::text()
{
	return _settings->host + "-" + _settings->name + "-" + _settings->driver;
}

QString ConnectionListItem::host()
{
	return _settings->host;
}

QString ConnectionListItem::name()
{
	return _settings->name;
}

QString ConnectionListItem::driver()
{
	return _settings->driver;
}

QString ConnectionListItem::user()
{
	return _settings->user;
}

QString ConnectionListItem::password()
{
	return _settings->password;
}

QString ConnectionListItem::database()
{
	return _settings->database;
}

qint16 ConnectionListItem::port()
{
	return _settings->port;
}

ConnectionSettings* ConnectionListItem::settings()
{
	return _settings;
}