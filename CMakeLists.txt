project(cpp-project)
include_directories(include)
add_executable(main src/main.cpp src/utils.cpp)
target_link_libraries(main PRIVATE)