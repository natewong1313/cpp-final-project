#include <iostream>
#include "httplib.h"
#include "utils.h"
using namespace std;

httplib::Server svr;

int main()
{
    cout << "its working" << endl;
    svr.Get("/", [](const httplib::Request &, httplib::Response &res)
            { string html = loadHTML("index.html"); 
            res.set_content(html, "text/html"); });
    svr.listen("0.0.0.0", 8080);
    return 0;
}