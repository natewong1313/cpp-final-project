#ifndef DB_H
#define DB_H

#include <mutex>
#include <sqlite3.h>
#include <string>
using namespace std;

// https://www.geeksforgeeks.org/implementation-of-singleton-class-in-cpp/

class Database {
  private:
    static Database *instancePtr;
    static mutex mtx;
    Database();

    sqlite3 *db;

    void connect();
    void runSqlFile(string filePath);

  public:
    Database(const Database &obj) = delete;

    static Database *getInstance();
    sqlite3 *getConnection();
    void cleanup();
    void setupTables();
};

class Statement {
  private:
    sqlite3_stmt *stmt;
    sqlite3 *db;
    string query;
    int bindCount;
    void logError();

  public:
    Statement(string query, sqlite3 *db);
    void bind(string strVal);
    void bind(int intVal);
    void execute();
    void finish();
};

#endif