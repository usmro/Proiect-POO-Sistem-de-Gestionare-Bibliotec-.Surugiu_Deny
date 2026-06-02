#include <iostream>
#include <sqlite3.h>

int main() {
    sqlite3* db;
    if(sqlite3_open("biblioteca.db", &db) != SQLITE_OK) {
        std::cerr << "Cannot open db" << std::endl;
        return 1;
    }
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, "SELECT * FROM Carti;", -1, &stmt, nullptr);
    if(rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }
    
    int count = 0;
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        count++;
    }
    sqlite3_finalize(stmt);
    std::cout << "Loaded " << count << " books." << std::endl;
    sqlite3_close(db);
    return 0;
}
