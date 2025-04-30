#include "Utils/DataBase/SQLiteWrapper.hpp"

namespace Utils :: DataBase {
    SQLiteWrapper::SQLiteWrapper(std::string &dbName, bool consoleLogger, const std::string &logFile) {
        ConfigureLogger(consoleLogger, logFile);
        int rc = sqlite3_open(dbName.c_str(), &db);
        if (rc != SQLITE_OK){
            logger.Error("Can't open database:{}", dbName);
            logger.Error("SQL error:{}", sqlite3_errmsg(db));
            return;
        }
        logger.Info("Opened database successfully");
    }
    SQLiteWrapper::~SQLiteWrapper() {
        logger.Flush();
        sqlite3_close(db);
    }


    void SQLiteWrapper::ConfigureLogger(bool consoleLogger, const std::string &logFile) {
        if (consoleLogger) {
            auto console_logger = std::make_shared<Log::ConsoleLogPolicy>();
            logger.AddPolicy(console_logger);
        }
        if(!logFile.empty()) {
            auto file_logger = std::make_shared<Log::FileLogPolicy>(logFile);
            logger.AddPolicy(file_logger);
        }
    }

    SQLiteQueryResult SQLiteWrapper::QueryData(const std::string& sql) {
        std::vector<std::map<std::string, std::string>> result;
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
        }

        int columnCount = sqlite3_column_count(stmt);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::map<std::string, std::string> row;
            for (int i = 0; i < columnCount; ++i) {
                const char* columnName = sqlite3_column_name(stmt, i);
                const unsigned char* columnValue = sqlite3_column_text(stmt, i);
                row[columnName] = columnValue ? reinterpret_cast<const char*>(columnValue) : "";
            }
            result.push_back(row);
        }

        sqlite3_finalize(stmt);
        return result;
    }
}