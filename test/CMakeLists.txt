cmake_minimum_required(VERSION 3.14)

project(test)

find_package(trace REQUIRED)
if(NOT trace_FOUND)
    message(FATAL_ERROR "trace package not found")
endif()

set(INCLUDES 
    ${CMAKE_SOURCE_DIR}/include
    ${trace_INCLUDE_DIR}
)
set(LIBLARIES cui)
set(LIB_DIRS ${trace_LIB_DIR})

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBLARIES})
target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIRS})