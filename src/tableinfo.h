#ifndef __TABLEINFO_H__
#define __TABLEINFO_H__

#include <QSqlRecord>
#include <QStringList>

class TableInfo
{
	public:
		TableInfo(const QString &name, const QSqlRecord &data); 
	
		QString name() const;
		QStringList fieldNames() const;
		QStringList fieldTypes() const;
		
	private:
		QString tableName;
		QSqlRecord tableData;
};

#endif // __TABLEINFO_H__