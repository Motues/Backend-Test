#include <sqlite3.h>
#include <iostream>

int main() {
    // 写一个测试用例
    sqlite3 *db;
    if (sqlite3_open("test.db", &db) != SQLITE_OK) {
        std::cout << "open db failed" << std::endl;
        return -1;
    }
    if (sqlite3_exec(db, "create table if not exists test (id integer primary key autoincrement, name text)", nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cout << "create table failed" << std::endl;
        return -1;
    }
    if (sqlite3_exec(db, "insert into test (name) values ('test')", nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cout << "insert failed" << std::endl;
        return -1;
    }
    return 0;
}