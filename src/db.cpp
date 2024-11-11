#include "db.h"

#include "utils.h"

#include <filesystem>
#include <iostream>
#include <mutex>
#include <sqlite3.h>
#include <string.h>
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
  int rc = sqlite3_exec(db, sqlToExec.c_str(), NULL, 0, NULL);
  if (rc != SQLITE_OK) {
    std::cerr << "Error Creating Table - " << sqlite3_errmsg(db) << std::endl;
  }
}

Statement::Statement(string query, sqlite3 *db) {
  this->query = query;
  this->bindCount = 0;
  this->db = db;
  int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) { logError(); }
}

void Statement::bind(string strVal) {
  bindCount++;
  sqlite3_bind_text(stmt, bindCount, strVal.c_str(), strlen(strVal.c_str()), NULL);
}

void Statement::bind(int intVal) {
  bindCount++;
  sqlite3_bind_int(this->stmt, bindCount, intVal);
}

void Statement::execute() {
  int rc = sqlite3_step(stmt);
  if (rc != 101 || rc != 0) { logError(); }
}

void Statement::finish() {
  int rc = sqlite3_finalize(stmt);
  if (rc != 0) { logError(); }
}

void Statement::logError() {
  cerr << "Statement error occured: " << sqlite3_errcode(db) << sqlite3_errmsg(db) << endl;
  cerr << query << endl;
}