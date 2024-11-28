cmake_minimum_required(VERSION 3.22.1)
project(cpp-project)

find_package(SQLite3 REQUIRED)
set(SQLITE3_THREADSAFE 1)

include_directories(include ${SQLite3_INCLUDE_DIRS})
include_directories(include ${ARGON2_INCLUDE_DIR})
include_directories(include)
add_executable(main src/main.cpp src/utils.cpp src/db.cpp src/api/messaging.cpp src/api/servers.cpp src/api/users.cpp src/api/auth.cpp src/api/channels.cpp src/services/messagemanager.cpp)
target_link_libraries(main PRIVATE SQLite::SQLite3)