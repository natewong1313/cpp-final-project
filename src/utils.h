#ifndef UTILS_H
#define UTILS_H
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

string getFileContents(string path);
string loadHTML(string path);
bool handleDbError(Database *db);
string createId();

#endif