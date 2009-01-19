#ifndef __TABLEINFO_H__
#define __TABLEINFO_H__

#include <QSqlRecord>
#include <QStringList>

class QSqlField;

class TableInfo
{
    public:
        TableInfo(const QString &name, const QSqlRecord &data);

        QString name() const;
        QStringList fieldNames() const;
        QStringList fieldTypes() const;
        QString fieldName(const int&) const;
        QString fieldType(const int&) const;
        QString fieldType(const QString&) const;
        QSqlField field(const int&) const;
        QSqlField field(const QString&) const;

    protected:
        QString typeToName(QVariant::Type type) const;

    private:
        QString tableName;
        QSqlRecord tableData;
};

#endif // __TABLEINFO_H__
