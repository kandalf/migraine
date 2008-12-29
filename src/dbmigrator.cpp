#include <QSqlDatabase>
#include <QSqlError>
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
}

void DBMigrator::copyTable(const TableInfo *table)
{
    QSqlDatabase srcDb = QSqlDatabase::database(_srcConnectionName);
    QSqlDatabase tgtDb = QSqlDatabase::database(_tgtConnectionName);
    QStringList sqlBatch;

    QSqlQuery srcQuery = srcDb.exec(constructSrcCopySQL(table));
    QSqlQuery tgtQuery;

    if (srcQuery.isActive() && srcQuery.isSelect())
    {
        srcQuery.seek(0);
        while(srcQuery.next())
        {
            QString sql = constructTgtCopySQL(table, srcQuery);
            sqlBatch << sql;
//            tgtQuery = tgtDb.exec(sql);
//            if (!tgtQuery.isValid())
//                emit(migrationError(tr("Error executing: %1 -- %2").arg(sql).arg(tgtDb.lastError().text())));
//            qDebug(sql.toAscii());
        }
        insertTransactionBatch(sqlBatch);
    }
    else
    {
        emit(migrationError(QString("Source DB Error: %1: %2").arg(srcDb.databaseName()).arg(srcDb.lastError().text())));
    }
    srcDb.close();
    tgtDb.close();
}


QString DBMigrator::constructSrcCopySQL(const TableInfo *table) const
{
    QString query("SELECT %1 FROM %2");
    return query.arg(table->fieldNames().join(",")).arg(table->name());

}

QString DBMigrator::constructTgtCopySQL(const TableInfo *tableInfo, const QSqlQuery &srcQuery) const
{
    QString insertQuery("INSERT INTO %1 (%2) VALUES (%3);");
    QStringList values;

    for( int i = 0; i < tableInfo->fieldNames().count(); i++)
    {
//        qDebug("TYPE: " + tableInfo->fieldType(i).toAscii());

        if (tableInfo->fieldType(i) == "QString" )
        {
            values << "'" + srcQuery.value(i).toString() + "'";
        }
        else if(tableInfo->fieldType(i) == "QDateTime")
        {
            values << QString("TIMESTAMP '%1'").arg(srcQuery.value(i).toString());
        }
        else
        {
            values << srcQuery.value(i).toString();
        }
    }

    return insertQuery.arg(tableInfo->name()).arg(tableInfo->fieldNames().join(", ")).arg(values.join(", "));
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

//        emit(migrationError(QString::number(query.numRowsAffected())));
        if (query.numRowsAffected() != 1) {
            emit(migrationError(tr("Target DB %1 Error: %2. Aborted!").arg(tgtDb.databaseName()).arg(tgtDb.lastError().text())));
            emit(migrationError(tr("Error executing: %1").arg(sentence)));
            if (transactionValid)
                tgtDb.exec("ROLLBACK");
            return;
        } else {
            emit(migrationError(QString("Executing: %1").arg(sentence)));
            emit(tableCopyProgress(batch.indexOf(sentence), batch.size()));
        }
    }
    if (transactionValid) {
        tgtDb.exec("COMMIT;");
        emit(migrationError("Committed"));
    }
}
