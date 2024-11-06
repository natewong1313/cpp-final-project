project(cpp-project)
include_directories(include)
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE)