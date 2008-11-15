#include <QSqlField>
#include "tableinfo.h"

TableInfo::TableInfo(const QString &name, const QSqlRecord &data)
{
	tableName = name;
	tableData = data;
}

QString TableInfo::name() const
{
	return this->tableName;
}

QStringList TableInfo::fieldNames() const
{
	QStringList fields;
	for (int i = 0; i < tableData.count(); i++)
		fields << tableData.field(i).name();
	return fields;
}

QStringList TableInfo::fieldTypes() const
{
	QStringList types;
	for (int i = 0; i < tableData.count(); i++)
		types << QVariant::typeToName(tableData.field(i).type());
	
	return types;
}