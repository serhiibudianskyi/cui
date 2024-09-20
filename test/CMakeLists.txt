cmake_minimum_required(VERSION 3.14)

project(test)

set(INCLUDES ${CMAKE_SOURCE_DIR}/include)
set(LIBLARIES cui)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBLARIES})