#ifndef __TYPES_H__
#define __TYPES_H__


typedef struct{
	QString name;
	QString host;
	qint16 port;
	QString user;
	QString password;
	QString database;
	QString driver;
}ConnectionSettings;

#endif // __TYPES_H__
