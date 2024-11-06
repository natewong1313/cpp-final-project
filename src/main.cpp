#include "httplib.h"
#include "utils.h"

#include <iostream>
using namespace std;

httplib::Server svr;

int main() {
  svr.Get("/", [](const httplib::Request &,
                  httplib::Response &res) {
    string html = loadHTML("index.html");
    res.set_content(html, "text/html");
  });
  cout << "Server running on port 8080" << endl;
  svr.listen("0.0.0.0", 8080);
  return 0;
}