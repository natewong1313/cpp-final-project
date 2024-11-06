#include "httplib.h"
#include "json.hpp"
#include "utils.h"

#include <iostream>
#include <sqlite3.h>
using namespace std;
using namespace httplib;
using json = nlohmann::json;

Server svr;

int main() {
  // using some code from sqlite docs
  sqlite3 *db;
  int rc = sqlite3_open("pleasework.db", &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return (1);
  }

  svr.Get("/", [](const Request &, Response &res) {
    string html = loadHTML("index.html");
    res.set_content(html, "text/html");
  });
  svr.Get("/login", [](const Request &, Response &res) {
    string html = loadHTML("login.html");
    res.set_content(html, "text/html");
  });

  svr.Get("/test", [](const Request &, Response &res) {
    json j;
    j["hello"] = "world";
    res.set_content(to_string(j), "application/json");
  });
  cout << "Server running on port 8080" << endl;
  svr.listen("0.0.0.0", 8080);
  sqlite3_close(db);
  return 0;
}