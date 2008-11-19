#ifndef MIGRATIONTABLEMATCH_H
#define MIGRATIONTABLEMATCH_H

#include <QList>
#include <QPair>

class TableInfo;
class QSqlField;

class MigrationTableMatch
{
    public:
        MigrationTableMatch(TableInfo*, TableInfo*);

        bool setMatch(const QString&, const QString&);
        TableInfo* source() const;
        TableInfo* target() const;
        QPair<QSqlField, QSqlField> getMatch(const int &) const;
        int count() const;

    private:
        TableInfo *_source;
        TableInfo *_target;

        QList<QPair<QSqlField, QSqlField> > matches;
};

#endif // MIGRATIONTABLEMATCH_H
