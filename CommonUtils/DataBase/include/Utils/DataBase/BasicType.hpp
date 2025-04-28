#pragma once

#include <string>
#include <vector>
#include <map>


namespace Utils :: DataBase {

    enum SQLiteDataType {
        INTEGER,
        REAL,
        TEXT,
        BLOB,
        NONE
    };

    using SQLiteKeyType = std::map<std::string, SQLiteDataType>;
    using SQLiteKeyValue = std::map<std::string, std::string>;
    using SQLiteQueryResult = std::vector<std::map<std::string, std::string>>;


}