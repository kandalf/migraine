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
//        qDebug("Params: " + srcField.toAscii() + " - " + tgtField.toAscii());
//        qDebug("Matches: " + matches.at(0).first.name().toAscii() + " - " + matches.at(0).second.name().toAscii());
        return true;
    }
    else
    {
        return false;
    }
}

QPair<QSqlField, QSqlField> MigrationTableMatch::getMatch(const int &index) const
{
    return matches.at(index);
}
