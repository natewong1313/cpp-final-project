#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string loadHTML(string path)
{
    string html;
    ifstream in_file;
    in_file.open(path);
    // if (in_file.fail())
    // {
    //     return 0;
    // }
    string line;
    while (getline(in_file, line))
    {
        html += line;
    }
    in_file.close();
    return html;
}