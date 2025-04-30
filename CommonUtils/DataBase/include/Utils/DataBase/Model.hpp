#pragma once

#include "Utils/Log.hpp"
#include "BasicType.hpp"

#include <sqlite3.h>

namespace Utils :: DataBase {

    class Model {
    public:
        inline static bool Execute(sqlite3 *bd, const std::string& sql); // 执行sql语句
        inline static SQLiteQueryResult ExecuteQuery(sqlite3 *db, const std::string& sql); // 执行带回调的sql语句

        static bool CreateTable(sqlite3 *db, const std::string& tableName, const SQLiteKeyType & columns); // 创建表
        static bool Insert(sqlite3 *db, const std::string &tableName, const SQLiteKeyValue &data); // 插入数据
        static bool QueryColumnExist( // 查询满足条件的指定列
            sqlite3 *db, const std::string& tableName, const std::string& condition);
        static SQLiteQueryResult QueryColumnAll(sqlite3 *db, const std::string& tableName); // 查询所有列
        static SQLiteQueryResult QueryColumn( // 查询指定列
            sqlite3 *db, const std::string& tableName,
            std::vector<std::string> &columns);
        static SQLiteQueryResult QueryColumnCondition( // 查询满足条件的指定列
            sqlite3 *db, const std::string& tableName,
            std::vector<std::string> &columns, const std::string& condition);

        static bool Update( // 更新数据
            sqlite3 *db, const std::string& tableName,
            const SQLiteKeyValue &data, const std::string& condition);

        static bool Remove(sqlite3 *db, const std::string& tableName, const std::string& condition); // 删除数据
    };

}