#ifndef DBMIGRATOR_H
#define DBMIGRATOR_H

#include <QThread>
#include <QSqlQuery>
#include <QSqlField>

class DBAnalyst;
class TableInfo;
class MigrationTableMatch;

class DBMigrator : public QThread
{
Q_OBJECT

    public:
        DBMigrator(DBAnalyst *analyst, QObject *parent = 0);

        void migrateDatabase(const QString &srcConnName, const QString &tgtConnName, DBAnalyst*);
        void setSourceConnectionName(const QString &name);
        void setTargetConnectionName(const QString &name);
        QString sourceConnectionName() const;
        QString targetConnectionName() const;

    public slots:
        void copyTable(const TableInfo *table);
        void migrateTable(const MigrationTableMatch *migrationTable);
        void createTable(const TableInfo *table);

    protected:
        void run();
        QString constructSrcCopySQL(const TableInfo *table) const;
        QString constructTgtCopySQL(const TableInfo *tableInfo, const QSqlQuery &srcQuery) const;
        QString constructSrcMigrationSQL(const MigrationTableMatch *migrationTable) const;
        QString constructTgtMigrationSQL(const MigrationTableMatch *migrationTable, const QSqlQuery &srcQuery) const;
        QString fixSqlSyntax(const QString &qType, const QString &value) const;
        QString fieldNamesForCreate(const TableInfo *tableInfo) const;
        QString fieldTypeForCreate(const QSqlField &field) const;

    protected slots:
        void insertTransactionBatch(const QStringList &batch);

    signals:
        void beforeMigrationStart(const int &toCopy, const int &toMigrate, const int &toCreate);
        void tableCopyStarted(const QString &tableName, const int &index);
        void tableMigrationStarted(const QString &tableName, const int &index);
        void tableCreationStarted(const QString &tableName, const int &index);
        void tablesToCopy(const int &total);
        void tablesToMigrate(const int &total);
        void tablesToCreate(const int &total);
        void migrationError(const QString &msg);
        void migrationDone(const int &copied, const int &migrated, const int &created);
        void migrationDone();
        void insertProgress(const int &index, const int &total);

    private:
        DBAnalyst *_analyst;
        QString _srcConnectionName;
        QString _tgtConnectionName;

};

#endif // DBMIGRATOR_H
