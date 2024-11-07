#include "db.h"

#include <iostream>
#include <mutex>
#include <sqlite3.h>
using namespace std;

// https://www.geeksforgeeks.org/implementation-of-singleton-class-in-cpp/

// class Database {
//   private:
//     static Database *instancePtr;
//     static mutex mtx;

//     Database() {};

//     sqlite3 *db;

//   public:
//     Database(const Database &obj) = delete;

//     // https://www.geeksforgeeks.org/implementation-of-singleton-class-in-cpp/
//     static Database *getInstance() {
//       if (instancePtr == nullptr) {
//         lock_guard<mutex> lock(mtx);
//         if (instancePtr == nullptr) { instancePtr = new Database(); }
//       }
//       return instancePtr;
//     }

//     void setupTables() {
//       cout << "setup tables called" << endl;
//       // add some logic for initializing tables once i figure out what the tables will look like
//     };
// };

Database *Database::instancePtr = nullptr;

mutex Database::mtx;

Database::Database() {
  setupTables();
};

Database *Database::getInstance() {
  if (instancePtr == nullptr) {
    lock_guard<mutex> lock(mtx);
    if (instancePtr == nullptr) { instancePtr = new Database(); }
  }
  return instancePtr;
}

void Database::setupTables() {
  cout << "setup tables called" << endl;
  // add some logic for initializing tables once i figure out what the tables will look like
};