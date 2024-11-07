#include "db.h"

#include "utils.h"

#include <filesystem>
#include <iostream>
#include <mutex>
#include <sqlite3.h>
#include <string>

using namespace std;

Database *Database::instancePtr = nullptr;
mutex Database::mtx;

Database::Database() {
  connect();
  setupTables();
};

Database *Database::getInstance() {
  if (instancePtr == nullptr) {
    lock_guard<mutex> lock(mtx);
    if (instancePtr == nullptr) { instancePtr = new Database(); }
  }
  return instancePtr;
}

void Database::connect() {
  int rc = sqlite3_open("main.db", &db);
  if (rc) {
    // TODO: handle error
    return;
    // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    // sqlite3_close(db);
    // return (1);
  }
}

sqlite3 *Database::getConnection() {
  return db;
}

void Database::cleanup() {
  sqlite3_close(db);
}

void Database::setupTables() {
  // https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
  // loop through all sql files and run them
  for (const auto &file : filesystem::directory_iterator("../sql")) { runSqlFile(file.path()); }
};

void Database::runSqlFile(string filePath) {
  string sqlToExec = getFileContents(filePath);
  char *errorMsg;
  int rc = sqlite3_exec(db, sqlToExec.c_str(), NULL, 0, &errorMsg);
  if (rc != SQLITE_OK) {
    std::cerr << "Error Create Table" << std::endl;
    sqlite3_free(errorMsg);
  } else {
    cout << rc << endl;
  }
}