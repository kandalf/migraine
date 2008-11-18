#include <QHash>
#include <QPair>
#include <QSqlField>
#include "dbanalyst.h"
#include "tableinfo.h"
#include "migrationtablematch.h"

DBAnalyst::DBAnalyst(QObject *parent)
        :QObject(parent){}

DBAnalyst::DBAnalyst(const QList<TableInfo*>&src, const QList<TableInfo*>&tgt, QObject *parent)
        :QObject(parent)
{
    srcList = src;
    tgtList = tgt;
}

DBAnalyst::~DBAnalyst()
{
    MigrationTableMatch *current;
    for (int i = 0; i < nameMatches.keys().count(); i++)
    {
        current = static_cast<MigrationTableMatch*>(nameMatches.take(nameMatches.keys().at(i)));
        if (current)
            delete current;
    }
}

void DBAnalyst::setSourceList(const QList<TableInfo*> &list)
{
    srcList = list;
}

void DBAnalyst::setTargetList(const QList<TableInfo*> &list)
{
    tgtList = list;
}


void DBAnalyst::analyzeDatabases()
{
    QHash<QString, TableInfo*> targetHash;

    for (int i = 0; i < tgtList.count(); i++)
        targetHash[tgtList.at(i)->name()] = tgtList.at(i);

    for (int srcIndex = 0; srcIndex < srcList.count(); srcIndex++)
    {
        TableInfo *src = static_cast<TableInfo*>(srcList[srcIndex]);
        if (targetHash.keys().contains(src->name()))
        {
            nameMatches[src->name()] = new MigrationTableMatch(src, targetHash[src->name()]);
            emit(nameMatchFound(src->name()));
        }
        else
        {
            emit(noMatchFound(src->name()));
        }
    }
}

void DBAnalyst::analyzeDatabases(const QList<TableInfo*> &src, const QList<TableInfo*> &tgt)
{
    srcList = src;
    tgtList = tgt;
    analyzeDatabases();
}

MigrationTableMatch* DBAnalyst::getNameMatchTable(const QString &name)
{
    return nameMatches.value(name);
}

void DBAnalyst::setTableMatch(const QString &tableName, const QString &src, const QString &tgt)
{
    getNameMatchTable(tableName)->setMatch(src, tgt);
}
