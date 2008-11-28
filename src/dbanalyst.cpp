#include <QHash>
#include <QHashIterator>
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
    for (int i = 0; i < _nameMatches.keys().count(); i++)
    {
        current = static_cast<MigrationTableMatch*>(_nameMatches.take(_nameMatches.keys().at(i)));
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
            if (isExactMatch(src, targetHash[src->name()]))
            {
                _exactMatches[src->name()] = src;
                emit( exactMatchFound(src->name()) );
            }
            else
            {
                _nameMatches[src->name()] = new MigrationTableMatch(src, targetHash[src->name()]);
                emit(nameMatchFound(src->name()));
            }
        }
        else
        {
            _noMatches[src->name()] = src;
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
    return _nameMatches.value(name);
}

void DBAnalyst::setTableMatch(const QString &tableName, const QString &src, const QString &tgt)
{
    if (!getNameMatchTable(tableName)->setMatch(src, tgt))
        emit setMatchError(tr("Unable to set match: %1 -> %2 on table %3").arg(src).arg(tgt).arg(tableName));

}

bool DBAnalyst::isExactMatch(TableInfo *src, TableInfo *tgt)
{
    bool matchFound;

    if (src->fieldNames().count() != tgt->fieldNames().count())
        return false;

    for (int i = 0; i < src->fieldNames().count(); i++)
    {
        for (int j = 0; j < tgt->fieldNames().count(); j++)
        {
            if (tgt->fieldName(j) == src->fieldName(i) && tgt->fieldType(j) == src->fieldType(i))
            {
                matchFound = true;
                break;
            }
            else
            {
                matchFound = false;
            }
        }

        if (!matchFound)
            return false;
    }
    return true;
}

bool DBAnalyst::createTables() const
{
    return _createTables;
}

void DBAnalyst::setCreateTables(const bool &create)
{
    _createTables = create;
}


QStringList DBAnalyst::nameMatches() const
{
    return _nameMatches.keys();
}

QStringList DBAnalyst::exactMatches() const
{
    return _exactMatches.keys();
}

QStringList DBAnalyst::noMatches() const
{
    return _noMatches.keys();
}

QStringList DBAnalyst::tablesToMigrate() const
{
    QStringList tableNames;
    QHashIterator<QString, MigrationTableMatch*> it(_nameMatches);
    MigrationTableMatch *current;

    while(it.hasNext())
    {
        current = static_cast<MigrationTableMatch *>(it.next().value());
        if (current->count() > 0)
            tableNames << current->source()->name();
    }
    return tableNames;
}
