#include "db.h"
#include "sole.hpp"

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

// Returns a files contents given a path
string getFileContents(string path) {
  string contents;
  ifstream in_file;
  in_file.open(path);
  string line;
  while (getline(in_file, line)) { contents += line; }
  in_file.close();
  return contents;
}

string loadHTML(string path) {
  return getFileContents("../html/" + path);
}

// Pretty error logging
void handleDbError(Database *db) {
  cerr << "Db error occured: " << sqlite3_errcode(db->getConnection())
       << sqlite3_errmsg(db->getConnection()) << endl;
}

// Generates a uuid
string createId() {
  return sole::uuid4().str();
}