#pragma once

#include "Utils/Log.hpp"
#include "Model.hpp"

#include <sqlite3.h>
#include <vector>
#include <map>

namespace Utils :: DataBase {

	using SQLiteQueryResult = std::vector<std::map<std::string, std::string>>;

	class SQLiteWrapper {
	public:
        SQLiteWrapper(std::string &dbFileName, bool consoleLogger = true, const std::string &logFile = "DataBase.log");
		~SQLiteWrapper();

        void CreateTable();
        void DeleteTable();
        void InsertData();
        void DeleteData();
        void UpdateData();
        SQLiteQueryResult QueryData();
        void SQLitesyntax(std::string &sql);

	private:
		sqlite3* db;
        Log::Logger logger;

        void ConfigureLogger(bool consoleLogger = true, const std::string &logFile = "DataBase.log");
	};
}
