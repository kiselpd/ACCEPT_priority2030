cmake_minimum_required(VERSION 3.16)

project(accept_server)	

set(SOURCE_EXE main.cpp)	

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)
add_executable(${PROJECT_NAME} ${SOURCE_EXE})

target_include_directories(${PROJECT_NAME} PUBLIC ${CORE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CORE_LIBRARIES})

add_definitions("-DBOOST_ALLOW_DEPRECATED_HEADERS")
add_definitions("-DBOOST_BIND_GLOBAL_PLACEHOLDERS")