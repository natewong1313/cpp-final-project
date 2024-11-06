#include <iostream>
#include "httplib.h"
using namespace std;

httplib::Server svr;

int main()
{
    cout << "its working" << endl;
    svr.Get("/", [](const httplib::Request &, httplib::Response &res)
            { res.set_content("Hello World!", "text/plain"); });
    svr.listen("0.0.0.0", 8080);
    return 0;
}