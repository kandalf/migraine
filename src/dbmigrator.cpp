#include <QSqlDatabase>
#include <QSqlError>
#include <QVariant>
#include "dbmigrator.h"
#include "tableinfo.h"
#include "migrationtablematch.h"
#include "dbanalyst.h"
#include "geometrycolumninfo.h"

DBMigrator::DBMigrator(DBAnalyst *analyst, QObject *parent)
        :QThread(parent)
{
    _analyst = analyst;
}

DBMigrator::~DBMigrator()
{
    QString table;
    GeometryColumnInfo *column;

    foreach(table, geometryColumns.keys())
    {
        foreach(column, geometryColumns[table])
            delete column;
    }
}

void DBMigrator::setSourceConnectionName(const QString &name)
{
    _srcConnectionName = name;
}

void DBMigrator::setTargetConnectionName(const QString &name)
{
    _tgtConnectionName = name;
}

QString DBMigrator::sourceConnectionName() const
{
    return _srcConnectionName;
}

QString DBMigrator::targetConnectionName() const
{
    return _tgtConnectionName;
}

void DBMigrator::run()
{
    if (_srcConnectionName.isEmpty() || _tgtConnectionName.isEmpty())
    {
        emit (migrationError(tr("Cannot start migration. Either source database or target database names are empty")));
        return;
    } else {
        migrateDatabase(_srcConnectionName, _tgtConnectionName, _analyst);
    }
}

void DBMigrator::migrateDatabase(const QString &srcConnName, const QString &tgtConnName, DBAnalyst *analyst)
{
    _srcConnectionName = srcConnName;
    _tgtConnectionName = tgtConnName;
    _analyst = analyst;

    emit (beforeMigrationStart(analyst->exactMatches().count(), analyst->tablesToMigrate().count(), analyst->noMatches().count()));
    emit (tablesToCopy(analyst->exactMatches().count()));
    emit (tablesToMigrate(analyst->nameMatches().count()));
    emit (tablesToCreate(analyst->noMatches().count()));

    if (migrateGeometries() || migrateGeometriesAsText())
        findGeometryColumns();

    for (int i = 0; i < analyst->exactMatches().count(); i++)
    {
        emit(tableCopyStarted(analyst->exactMatches().value(i), i));
        copyTable(analyst->exactMatch(analyst->exactMatches().value(i)));
    }

    for (int i = 0; i < analyst->nameMatches().count(); i++)
    {
        emit(tableMigrationStarted(analyst->nameMatches().value(i), i));
        migrateTable(analyst->nameMatch(analyst->nameMatches().value(i)));
    }

    if (analyst->createTables())
    {
        for (int i = 0; i < analyst->noMatches().count(); i++)
        {
            emit(tableCreationStarted(analyst->noMatches().value(i), i));
            createTable(analyst->noMatch(analyst->noMatches().value(i)));
        }
    }

    emit(migrationDone(analyst->exactMatches().count(), analyst->nameMatches().count(), analyst->noMatches().count()));
    emit(migrationDone());
}

void DBMigrator::copyTable(const TableInfo *table)
{
    QSqlDatabase srcDb = QSqlDatabase::database(_srcConnectionName);
    QSqlDatabase tgtDb = QSqlDatabase::database(_tgtConnectionName);
    QStringList sqlBatch;

    QSqlQuery srcQuery = srcDb.exec(constructSrcCopySQL(table));
    QSqlQuery tgtQuery;

    if (srcQuery.isSelect() && srcQuery.isActive())
    {
        srcQuery.seek(-1);
        while(srcQuery.next())
        {
            sqlBatch << constructTgtCopySQL(table, srcQuery);
        }
        insertTransactionBatch(sqlBatch);
    }
    else
    {
        emit(migrationError(QString("Source DB Error: %1: %2").arg(srcDb.databaseName()).arg(srcDb.lastError().text())));
    }
}

void DBMigrator::migrateTable(const MigrationTableMatch *migrationTable)
{
    QSqlDatabase srcDb = QSqlDatabase::database(_srcConnectionName, true);
    QSqlDatabase tgtDb = QSqlDatabase::database(_tgtConnectionName, true);
    QSqlQuery srcQuery = srcDb.exec(constructSrcMigrationSQL(migrationTable));
    QStringList sqlBatch;

    if (srcQuery.isSelect() && srcQuery.isActive())
    {
        srcQuery.seek(-1);
        while(srcQuery.next())
        { 
            sqlBatch << constructTgtMigrationSQL(migrationTable, srcQuery);
        }
        insertTransactionBatch(sqlBatch);
    }
    else
    {
        emit(migrationError(QString("Source DB Error: %1: %2").arg(srcDb.databaseName()).arg(srcDb.lastError().text())));
    }

}

void DBMigrator::createTable(const TableInfo *tableInfo)
{
    QSqlDatabase tgtDb = QSqlDatabase::database(_tgtConnectionName);
    QString query = "CREATE TABLE %1 (%2)";

    tgtDb.exec(query.arg(tableInfo->name()).arg(fieldNamesForCreate(tableInfo)));

    copyTable(tableInfo);
//    if (tgtDb.lastError().text().isEmpty() || tgtDb.lastError().text().isNull())
//
//    else
//        emit(migrationError(tr("Table creation error: %1: %2").arg(tgtDb.databaseName()).arg(tgtDb.lastError().text())));
}


QString DBMigrator::fieldNamesForCreate(const TableInfo *tableInfo) const
{
    QStringList fields;

    foreach (QString fieldName, tableInfo->fieldNames())
        fields << fieldName.append(" ").append(fieldTypeForCreate(tableInfo->field(fieldName)));

    return fields.join(", ");
}

QString DBMigrator::fieldTypeForCreate(const QSqlField &field) const
{
    QString dbDriver = QSqlDatabase::database(_tgtConnectionName).driverName();
    switch (field.type())
    {
        case QVariant::Bool:
            return "boolean";

        case QVariant::Double:
            if (dbDriver.contains("MYSQL"))
                return "double";
            else
                return "real";

        case QVariant::Int:
            return "integer";

        case QVariant::Date:
        case QVariant::DateTime:
        case QVariant::Time:
            return "timestamp";

        default:
            return "text";

    }
}

QString DBMigrator::constructSrcCopySQL(const TableInfo *tableInfo) const
{
    QString query("SELECT %1 FROM %2");
    if ( shouldCreatePostGIS(tableInfo) )
        return query.arg(parsePostGISSrcFields(tableInfo)).arg(tableInfo->name());
    return query.arg(tableInfo->fieldNames().join(",")).arg(tableInfo->name());
}

QString DBMigrator::constructSrcMigrationSQL(const MigrationTableMatch *migrationTable) const
{
    QString selectQuery("SELECT %1 FROM %2");
    QStringList fields;
    QPair<QSqlField, QSqlField>currentMatch;
    TableInfo *srcTable = migrationTable->source();

    for (int i = 0; i < migrationTable->count(); i++)
    {
        currentMatch = migrationTable->getMatch(i);
        fields << currentMatch.first.name() + " AS " + currentMatch.second.name();
    }

    return selectQuery.arg(fields.join(", ")).arg(srcTable->name());
}

QString DBMigrator::constructTgtCopySQL(const TableInfo *tableInfo, const QSqlQuery &srcQuery) const
{
    QString insertQuery("INSERT INTO %1 (%2) VALUES (%3);");
    QStringList values;

    for( int i = 0; i < tableInfo->fieldNames().count(); i++)
    {
        values << this->fixSqlSyntax(tableInfo->name(), tableInfo->fieldType(i), srcQuery.value(i).toString());
    }

    return insertQuery.arg(tableInfo->name()).arg(tableInfo->fieldNames().join(", ")).arg(values.join(", "));
}

QString DBMigrator::constructTgtMigrationSQL(const MigrationTableMatch *migrationTable, const QSqlQuery &srcQuery) const
{
    QString insertQuery("INSERT INTO %1 (%2) VALUES (%3);");
    QStringList fieldNames;
    QStringList values;
    QPair<QSqlField, QSqlField>currentMatch;

    for( int i = 0; i < migrationTable->count(); i++)
    {
        currentMatch = migrationTable->getMatch(i);
        fieldNames << currentMatch.second.name();
        values << this->fixSqlSyntax(migrationTable->target()->name(), migrationTable->target()->fieldType(currentMatch.second.name()), srcQuery.value(i).toString());
    }
    return insertQuery.arg(migrationTable->target()->name()).arg(fieldNames.join(", ")).arg(values.join(", "));
}


void DBMigrator::insertTransactionBatch(const QStringList &batch)
{
//    emit(migrationError(QString("BATCH SIZE: %1").arg(batch.size())));
    QSqlDatabase tgtDb = QSqlDatabase::database(_tgtConnectionName, true);
    bool transactionValid = tgtDb.transaction();

    QSqlQuery query; // = tgtDb.exec("BEGIN TRANSACTION;");
    //transactionValid = (query.lastError().type() == QSqlError::NoError);

    foreach(QString sentence, batch)
    {
//        query = tgtDb.exec(sentence);
//
//        if (query.numRowsAffected() != 1) {
//            emit(migrationError(tr("Target DB %1 Error: %2. Aborted!").arg(tgtDb.databaseName()).arg(tgtDb.lastError().text())));
//            emit(migrationError(tr("Error executing: %1").arg(sentence)));
//            if (transactionValid)
//            {
////                tgtDb.exec("ROLLBACK");
//                tgtDb.rollback();
//                emit(migrationError(tr("Rolled Back")));
//            }
//            return;
//        } else {
//            emit(insertProgress(batch.indexOf(sentence), batch.size()));
//        }
    }
    if (transactionValid) {
        if (query.isActive())
            query.finish();

        tgtDb.commit();
//        tgtDb.exec("COMMIT;");
    }
    emit(insertProgress(batch.size(), batch.size()));
}


QString DBMigrator::fixSqlSyntax(const QString &tableName, const QString &qType, const QString &value) const
{
    QString sqlValue(value);

    if (qType == "QString" )
    {
        return "'" + sqlValue.replace("'", "\\'") + "'";
    }
    else if(qType == "QDateTime")
    {
        if (sqlValue.isEmpty())
            return QString("NOW()");
        else
            return QString("TIMESTAMP '%1'").arg(sqlValue.replace("'", "\\'"));
    }
    else if(qType == "QDate")
    {
        return QString("DATE '%1'").arg(sqlValue.replace("'", "\\'"));
    }
    else
    {
        if (sqlValue.isEmpty())
            return QString::number(0);
        return sqlValue.replace("'", "\\'");
    }

}

void DBMigrator::setPostGISOnSource(const bool &found)
{
    this->_postGISOnSource = found;
}

void DBMigrator::setPostGISOnTarget(const bool &found)
{
    this->_postGISOnTarget = found;
}

bool DBMigrator::postGISOnSource() const
{
    return this->_postGISOnSource;
}

bool DBMigrator::postGISOnTarget() const
{
    return this->_postGISOnTarget;
}

void DBMigrator::setMigrateGeometries(const bool &migrate)
{
//    qDebug(QString("Migrate Geometries: %1").arg(migrate).toAscii());
    this->_migrateGeometries = migrate;
}

void DBMigrator::setMigrateGeometriesAsText(const bool &migrate)
{
//    qDebug(QString("Migrate Geometries As Text: %1").arg(migrate).toAscii());
    this->_migrateAsText = migrate;
}

void DBMigrator::setIgnoreGeometries(const bool &ignore)
{
//    qDebug(QString("Ignore Geometries: %1").arg(ignore).toAscii());
    this->_ignoreGeometries = ignore;
}

bool DBMigrator::migrateGeometries() const
{
    return this->_migrateGeometries;
}

bool DBMigrator::migrateGeometriesAsText() const
{
    return this->_migrateAsText;
}

bool DBMigrator::ignoreGeometries() const
{
    return this->_ignoreGeometries;
}

void DBMigrator::findGeometryColumns()
{
    QSqlDatabase srcDb = QSqlDatabase::database(_srcConnectionName);
    QSqlQuery query(srcDb);

    query.exec("SELECT f_table_name, f_geometry_column, type, srid, coord_dimension FROM geometry_columns");

    if (query.isActive())
    {
        query.seek(-1);
        while(query.next())
        {
            this->geometryColumns[query.value(0).toString()].append(
                                                    new GeometryColumnInfo(
                                                                        query.value(0).toString(),
                                                                        query.value(1).toString(),
                                                                        query.value(2).toString(),
                                                                        query.value(3).toInt(),
                                                                        query.value(4).toInt()
                                                     )
                                                   );

        }
    }
}


QString DBMigrator::parsePostGISSrcFields(const TableInfo *tableInfo) const
{
    GeometryColumnInfo *column;
    QStringList fields;
    QString field;

    foreach(field, tableInfo->fieldNames())
    {
        foreach(column, geometryColumns[tableInfo->name()])
        {
            if (field == column->columnName())
                fields << QString("AsText(%1)").arg(field);
            else
                fields << field;
        }
    }
//    qDebug(QString("%1:\n %2").arg(tableInfo->name()).arg(fields.join(", ")).toAscii());
    return fields.join(", ");
}

bool DBMigrator::shouldCreatePostGIS(const TableInfo *tableInfo) const
{
    return (migrateGeometries() || migrateGeometriesAsText()) && geometryColumns.keys().contains(tableInfo->name());
}


