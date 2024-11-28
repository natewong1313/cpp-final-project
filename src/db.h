#ifndef DB_H
#define DB_H

#include <mutex>
#include <sqlite3.h>
#include <string>
using namespace std;

// Utility class around the sqlite3 statement functionality in order to provide better error
// handling and more modular code
class Statement {
  private:
    sqlite3_stmt *stmt;
    sqlite3 *db;
    string query;
    int bindCount;
    void logError();

  public:
    // statement takes in a query string and a db connection
    Statement(string query, sqlite3 *db);
    // sets variable values in the query
    void bind(string strVal);
    void bind(int intVal);
    // runs the query
    int execute();
    // gets the next returned row
    int step();
    // cleanup
    void finish();
    // getting result column values in a row, after calling step, at a specified column index
    string getResultString(int position);
    int getResultInt(int position);
    sqlite3_stmt *getStmt();
};

// Singleton class that allows the single database connection to be reused across the app
// Sqlite automatically handles pooling connections for us, so using a singleton is safe in this
// instance
// https : // www.geeksforgeeks.org/implementation-of-singleton-class-in-cpp/
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
    // ONLY called at end of app life
    void cleanup();
    // Runs .sql files in /sql
    void setupTables();
    Statement newStatement(string query);
};

#endif