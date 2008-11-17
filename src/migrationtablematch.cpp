#include <QSqlField>
#include "migrationtablematch.h"
#include "tableinfo.h"

MigrationTableMatch::MigrationTableMatch(TableInfo *src, TableInfo *tgt)
{
    _source = src;
    _target = tgt;
}

TableInfo* MigrationTableMatch::source() const
{
    return _source;
}

TableInfo* MigrationTableMatch::target() const
{
    return _target;
}

bool MigrationTableMatch::setMatch(const QString &srcField, const QString &tgtField)
{
    if (_source->fieldNames().contains(srcField) && _target->fieldNames().contains(tgtField))
    {
        matches.append(qMakePair(_source->field(srcField), _target->field(tgtField)));
        return true;
    }
    else
    {
        return false;
    }
}
