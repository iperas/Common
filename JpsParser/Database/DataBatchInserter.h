#ifndef DataBatchInserter_h__
#define DataBatchInserter_h__

#include <QtCore/QtCore>
#include "Util/Exception.h"
#include "Util/Logger.h"
#include "Connection.h"
#include "DatabaseHelper.h"
#include "Util/InvalidOperationException.h"
#include "Util/SharedPtr.h"

using Util::InvalidOperationException;

namespace Database 
{
    class DataBatchInserter
    {
    public:
        SHARED_PTR_T(DataBatchInserter);
    private:
        QString _insertQuery;
        QString _tableName;
        int _rowsAdded;
        int _batchSize;
        Connection* _connection;
        DatabaseHelper* _dbHelper;
        QVector<QVariantList> _boundValues;
        QList<DataBatchInserter::Pointer_t> _children;
    public:
        // insertQuery: "INSERT INTO <table name>(<column name>[, <column name>]) VALUES (?, ?, ?)"
        // connection: pointer to connection class
        DataBatchInserter(const QString& insertQuery, int boundColumnsCount, Connection* connection, int batchSize = 1000, const QString& tableName = "")
        {
            _insertQuery = insertQuery;
            _connection = connection;
            _dbHelper = _connection->DbHelper();
            _rowsAdded = 0;
            _batchSize = batchSize;
            _boundValues.resize(boundColumnsCount);
            _tableName = tableName;
        }

        ~DataBatchInserter()
        {
            Flush();
        }

        void AddChild(DataBatchInserter::Pointer_t child)
        {
            _children.push_back(child);
        }

        const QList<DataBatchInserter::Pointer_t>& GetChildren() const { return _children; }

        void AddRow(const QList<QVariant>& values)
        {
            if (values.size() != _boundValues.size())
            {
                throw InvalidOperationException("Invalid elements count in `AddRow(values)` array.");
            }
            if (_rowsAdded > _batchSize)
            {
                Flush();
            }

            for (int i = 0; i < _boundValues.size(); ++i)
            {
                _boundValues[i].append(values.at(i));
            }
            _rowsAdded++;
        }

        // execute all pending queries
        void Flush()
        {
            if (_rowsAdded > 0)
            {
                foreach (DataBatchInserter::Pointer_t child, _children)
                {
                    child->Flush();
                }
                QSqlQuery query = _dbHelper->ExecuteQuery("");
                query.prepare(_insertQuery);
                DatabaseHelper::ThrowIfError(query);
                foreach (QVariantList boundValue, _boundValues)
                {
                    query.addBindValue(boundValue);
                }
                query.execBatch();
                DatabaseHelper::ThrowIfError(query);

                if (_tableName.isEmpty() || _tableName.isNull())
                {
                    sLogger.Info(QString("��������� %1 �������...").arg(_rowsAdded));
                } else {
                    sLogger.Info(QString("��������� %1 ������� � `%2`...").arg(_rowsAdded).arg(_tableName));
                }
                _rowsAdded = 0;
                int size = _boundValues.size();
                _boundValues.clear();
                _boundValues.resize(size);
            }
        }

        // returns pending query
        inline const QString& InsertQuery() const
        {
            return _insertQuery;
        }
    };
}

#endif // DataBatchInserter_h__