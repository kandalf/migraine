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

QString TableInfo::fieldName(const int &index) const
{
    return tableData.field(index).name();
}

QString TableInfo::fieldType(const int &index) const
{
    return QVariant::typeToName(tableData.field(index).type());
}

QString TableInfo::fieldType(const QString &name) const
{
    return QVariant::typeToName(tableData.field(name).type());
}

QSqlField* TableInfo::field(const int &index) const
{
    return &tableData.field(index);
}

QSqlField* TableInfo::field(const QString &name) const
{
    return &tableData.field(name);
}
