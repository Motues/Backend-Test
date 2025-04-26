#pragma once

#include "Utils/Log.hpp"

#include <sqlite3.h>

namespace Utils :: DataBase {

    class Model {
    public:
        // 创建表
        static bool createTable(sqlite3 *db, const std::string& tableName, const std::vector<std::string>& columns) {
            std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
            for (size_t i = 0; i < columns.size(); ++i) {
                sql += columns[i];
                if (i < columns.size() - 1) sql += ", ";
            }
            sql += ");";
            int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
            if (rc != SQLITE_OK) {

            }
        }

        // 插入数据
        static void insert(SQLiteWrapper& db, const std::string& tableName, const std::map<std::string, std::string>& data) {
            std::string columns, values;
            for (const auto& [key, value] : data) {
                columns += key + ", ";
                values += "'" + value + "', ";
            }
            columns.pop_back(); columns.pop_back();
            values.pop_back(); values.pop_back();

            std::string sql = "INSERT INTO " + tableName + " (" + columns + ") VALUES (" + values + ");";
            db.execute(sql);
        }

        // 查询数据
        static std::vector<std::map<std::string, std::string>> select(SQLiteWrapper& db, const std::string& tableName) {
            std::string sql = "SELECT * FROM " + tableName + ";";
            return db.query(sql);
        }

        // 更新数据
        static void update(SQLiteWrapper& db, const std::string& tableName, const std::map<std::string, std::string>& data, const std::string& condition) {
            std::string sql = "UPDATE " + tableName + " SET ";
            for (const auto& [key, value] : data) {
                sql += key + "='" + value + "', ";
            }
            sql.pop_back(); sql.pop_back();
            sql += " WHERE " + condition + ";";
            db.execute(sql);
        }

        // 删除数据
        static void remove(SQLiteWrapper& db, const std::string& tableName, const std::string& condition) {
            std::string sql = "DELETE FROM " + tableName + " WHERE " + condition + ";";
            db.execute(sql);
        }
    private:
        std::shared_ptr<Log::Logger> logger;
    };

}