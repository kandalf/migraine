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
                types << typeToName(tableData.field(i).type());
	
	return types;
}

QString TableInfo::typeToName(QVariant::Type type) const
{
    switch(type)
    {
        case QVariant::Bool:
            return QObject::tr("Boolean");
        case QVariant::Char:
            return QObject::tr("Char");
        case QVariant::Date:
            return QObject::tr("Date");
        case QVariant::DateTime:
            return QObject::tr("DateTime");
        case QVariant::Double:
            return QObject::tr("Double");
        case QVariant::Int:
            return QObject::tr("Integer");
        case QVariant::LongLong:
            return QObject::tr("Long Long");
        case QVariant::String:
            return QObject::tr("Text");
        case QVariant::Time:
            return QObject::tr("Time");
        default:
            return QVariant::typeToName(type);
    }
}

QString TableInfo::fieldName(const int &index) const
{
    return tableData.field(index).name();
}

QString TableInfo::fieldType(const int &index) const
{
    return typeToName(tableData.field(index).type());
}

QString TableInfo::fieldType(const QString &name) const
{
    return typeToName(tableData.field(name).type());
}

QSqlField TableInfo::field(const int &index) const
{
    return tableData.field(index);
}

QSqlField TableInfo::field(const QString &name) const
{
    return tableData.field(name);
}
