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
        //qDebug(QString("Table: %1\nSource: %2\nTarget: %3").arg(this->source()->name()).arg(srcField).arg(tgtField).toAscii());
        matches.append(qMakePair(_source->field(srcField), _target->field(tgtField)));
        return true;
    }
    else
    {
       /* if (!_source->fieldNames().contains(srcField))
        {
            QStringList fields = _source->fieldNames();
            qDebug(QString("Source field %1 is not included").arg(srcField).toAscii());
        }


        if (!_target->fieldNames().contains(tgtField))
            qDebug("Target field is not included");
        */

        return false;
    }
}

QPair<QSqlField, QSqlField> MigrationTableMatch::getMatch(const int &index) const
{
    return matches.at(index);
}

int MigrationTableMatch::count() const
{
    return matches.count();
}
