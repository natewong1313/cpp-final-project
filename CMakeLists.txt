cmake_minimum_required(VERSION 3.28)
project(cpp-project)

find_package(SQLite3 REQUIRED)
set(SQLITE3_THREADSAFE 1)

include_directories(include ${SQLite3_INCLUDE_DIRS})
include_directories(include)
add_executable(main src/main.cpp src/utils.cpp src/db.cpp)
target_link_libraries(main PRIVATE SQLite::SQLite3)