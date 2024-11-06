#include "httplib.h"
#include "utils.h"

#include <iostream>
#include <sqlite3.h>
using namespace std;

httplib::Server svr;

int main() {
  // using some code from sqlite docs
  sqlite3 *db;
  int rc = sqlite3_open("pleasework.db", &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return (1);
  }

  svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
    string html = loadHTML("index.html");
    res.set_content(html, "text/html");
  });
  cout << "Server running on port 8080" << endl;
  svr.listen("0.0.0.0", 8080);
  sqlite3_close(db);
  return 0;
}