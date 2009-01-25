#ifndef DBMIGRATOR_H
#define DBMIGRATOR_H

#include <QThread>
#include <QSqlQuery>
#include <QSqlField>
#include <QHash>
#include <QStringList>

class DBAnalyst;
class TableInfo;
class MigrationTableMatch;
class GeometryColumnInfo;

class DBMigrator : public QThread
{
Q_OBJECT
Q_PROPERTY(bool postGISOnSource READ postGISOnSource WRITE setPostGISOnSource);
Q_PROPERTY(bool postGISOnTarget READ postGISOnTarget WRITE setPostGISOnTarget);
Q_PROPERTY(bool migrateGeometries READ migrateGeometries WRITE setMigrateGeometries);
Q_PROPERTY(bool migrateGeometriesAsText READ migrateGeometriesAsText WRITE setMigrateGeometriesAsText);
Q_PROPERTY(bool ignoreGeometries READ ignoreGeometries WRITE setIgnoreGeometries);
    public:
        DBMigrator(DBAnalyst *analyst, QObject *parent = 0);
        ~DBMigrator();

        void migrateDatabase(const QString &srcConnName, const QString &tgtConnName, DBAnalyst*);
        void setSourceConnectionName(const QString &name);
        void setTargetConnectionName(const QString &name);
        QString sourceConnectionName() const;
        QString targetConnectionName() const;
        bool postGISOnSource() const;
        bool postGISOnTarget() const;
        bool migrateGeometries() const;
        bool migrateGeometriesAsText() const;
        bool ignoreGeometries() const;

    public slots:
        void copyTable(const TableInfo *table);
        void migrateTable(const MigrationTableMatch *migrationTable);
        void createTable(const TableInfo *table);
        void setPostGISOnSource(const bool &found);
        void setPostGISOnTarget(const bool &found);
        void setMigrateGeometries(const bool &migrate);
        void setMigrateGeometriesAsText(const bool &migrate);
        void setIgnoreGeometries(const bool &migrate);

    protected:
        void run();
        QString constructSrcCopySQL(const TableInfo *tableInfo);
        QString constructTgtCopySQL(const TableInfo *tableInfo, const QSqlQuery &srcQuery) const;
        QString constructSrcMigrationSQL(const MigrationTableMatch *migrationTable) const;
        QString constructTgtMigrationSQL(const MigrationTableMatch *migrationTable, const QSqlQuery &srcQuery) const;
        QString fixSqlSyntax(const QString &tableName, const QString &qType, const QString &value) const;
        QString fieldNamesForCreate(const TableInfo *tableInfo) const;
        QString fieldTypeForCreate(const QSqlField &field) const;
        QString parsePostGISSrcFields(const TableInfo *tableInfo, const bool &ignore);
        bool    shouldCreatePostGIS(const TableInfo *tableInfo) const;

    protected slots:
        void insertTransactionBatch(const QStringList &batch);
        void findGeometryColumns();

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
        bool _postGISOnSource;
        bool _postGISOnTarget;
        bool _migrateGeometries;
        bool _migrateAsText;
        bool _ignoreGeometries;
        QHash<QString, QList<GeometryColumnInfo*> >geometryColumns;
        QStringList geometryFields;
        QStringList ignoredFields;
};

#endif // DBMIGRATOR_H
