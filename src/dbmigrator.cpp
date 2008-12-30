#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QVariant>
#include "dbmigrator.h"
#include "tableinfo.h"
#include "migrationtablematch.h"
#include "dbanalyst.h"

DBMigrator::DBMigrator(DBAnalyst *analyst, QObject *parent)
        :QThread(parent)
{
    _analyst = analyst;
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

    for (int i = 0; i < analyst->exactMatches().count(); i++)
    {
        copyTable(analyst->exactMatch(analyst->exactMatches().value(i)));
    }

    for (int i = 0; i < analyst->nameMatches().count(); i++)
    {
        migrateTable(analyst->nameMatch(analyst->nameMatches().value(i)));
    }
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
        srcQuery.seek(0);
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

QString DBMigrator::constructSrcCopySQL(const TableInfo *table) const
{
    QString query("SELECT %1 FROM %2");
    return query.arg(table->fieldNames().join(",")).arg(table->name());
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
        values << this->fixSqlSyntax(tableInfo->fieldType(i), srcQuery.value(i).toString());
//        if (tableInfo->fieldType(i) == "QString" )
//        {
//            values << "'" + srcQuery.value(i).toString() + "'";
//        }
//        else if(tableInfo->fieldType(i) == "QDateTime")
//        {
//            values << QString("TIMESTAMP '%1'").arg(srcQuery.value(i).toString());
//        }
//        else
//        {
//            values << srcQuery.value(i).toString();
//        }
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
        values << this->fixSqlSyntax(migrationTable->target()->fieldType(currentMatch.second.name()), srcQuery.value(i).toString());
    }
    return insertQuery.arg(migrationTable->target()->name()).arg(fieldNames.join(", ")).arg(values.join(", "));
}


void DBMigrator::insertTransactionBatch(const QStringList &batch)
{
    QSqlDatabase tgtDb = QSqlDatabase::database(_tgtConnectionName, true);
    bool transactionValid = false;

    QSqlQuery query = tgtDb.exec("BEGIN TRANSACTION;");
    transactionValid = (query.lastError().type() == QSqlError::NoError);

    foreach(QString sentence, batch)
    {
        query = tgtDb.exec(sentence);

        if (query.numRowsAffected() != 1) {
            emit(migrationError(tr("Target DB %1 Error: %2. Aborted!").arg(tgtDb.databaseName()).arg(tgtDb.lastError().text())));
            emit(migrationError(tr("Error executing: %1").arg(sentence)));
            if (transactionValid)
            {
                tgtDb.exec("ROLLBACK");
                emit(tr("Rolled Back"));
            }
            return;
        } else {
            emit(tableCopyProgress(batch.indexOf(sentence), batch.size()));
        }
    }
    if (transactionValid) {
        tgtDb.exec("COMMIT;");
    }
}


QString DBMigrator::fixSqlSyntax(const QString &qType, const QString &value) const
{
    if (qType == "QString" )
    {
        return "'" + value + "'";
    }
    else if(qType == "QDateTime")
    {
        return QString("TIMESTAMP '%1'").arg(value);
    }
    else
    {
        return value;
    }

}
