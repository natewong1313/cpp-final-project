#include <iostream>
#include "httplib.h"
#include "utils.h"
using namespace std;

httplib::Server svr;

int main()
{
    cout << "its working" << endl;
    string html = loadHTML("index.html");

    svr.Get("/", [](const httplib::Request &, httplib::Response &res)
            { res.set_content("Hello World!", "text/plain"); });
    svr.listen("0.0.0.0", 8080);
    return 0;
}