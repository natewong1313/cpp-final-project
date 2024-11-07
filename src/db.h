#ifndef DB_H
#define DB_H

#include <mutex>
#include <sqlite3.h>
using namespace std;

// https://www.geeksforgeeks.org/implementation-of-singleton-class-in-cpp/

class Database {
  private:
    static Database *instancePtr;
    static mutex mtx;
    Database();
    sqlite3 *db;

  public:
    Database(const Database &obj) = delete;

    static Database *getInstance();

    void setupTables();
};

#endif