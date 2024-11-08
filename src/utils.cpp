#include "db.h"

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

string getFileContents(string path) {
  string contents;
  ifstream in_file;
  in_file.open(path);
  // if (in_file.fail())
  // {
  //     return 0;
  // }
  string line;
  while (getline(in_file, line)) { contents += line; }
  in_file.close();
  return contents;
}

string loadHTML(string path) {
  return getFileContents("../html/" + path);
}

void handleDbError(Database *db) {
  cerr << "Db error occured: " << sqlite3_errmsg(db->getConnection()) << endl;
}