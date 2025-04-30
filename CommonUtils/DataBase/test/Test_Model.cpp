#include "Utils/DataBase/Model.hpp"

using namespace Utils::DataBase;

int main() {
    sqlite3 *db;
    const std::string dbName = "../../cache/test.db";
    int rc = sqlite3_open(dbName.c_str(), &db);
    if (rc != SQLITE_OK){
        std::cout << "Open database failed" << std::endl;

    }else {
        std::cout << "Open database success" << std::endl;
    }
//    if(Model::CreateTable(
//        db, "test",
//        {{"id", SQLiteDataType::INTEGER},
//         {"name", SQLiteDataType::TEXT}})) {
//        std::cout << "Create table success" << std::endl;
//    }else {
//        std::cout << "Create table failed" << std::endl;
//    }
//    if (Model::Insert(db, "test", {{"id", "2"}, {"name", "Motues"}})) {
//        std::cout << "Insert data success" << std::endl;
//    }else {
//        std::cout << "Insert data failed" << std::endl;
//    }
    if(Model::QueryColumnExist(db, "test",  "name=\"Motues\"")) {
        std::cout << "Query data success" << std::endl;
    }else {
        std::cout << "Query data failed" << std::endl;
    }
}