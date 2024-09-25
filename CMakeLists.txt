cmake_minimum_required(VERSION 3.14)

project(cui)

# External packages
find_package(Curses REQUIRED)
if(NOT CURSES_FOUND)
    message(FATAL_ERROR "Curses package not found")
endif()

find_package(trace REQUIRED)
if(NOT trace_FOUND)
    message(FATAL_ERROR "trace package not found")
endif()

# Common variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)
set(INCLUDES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CURSES_INCLUDE_DIR}
    ${trace_INCLUDE_DIR}
)
set(LIBLARIES curses panel menu form trace)
set(LIB_DIRS ${trace_LIB_DIR})

# Config settings
set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME})
set(INSTALL_LIB_DIR ${INSTALL_DIR}/lib)
set(INSTALL_INCLUDE_DIR ${INSTALL_DIR}/include)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
               @ONLY)

#  Source files
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

#  Target (static by default)
option(BUILD_SHARED "Build shared cui library" Off)
if(BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${SOURCES})

    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_LIB_DIR})
else()
    add_library(${PROJECT_NAME} STATIC ${SOURCES})

    install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION ${INSTALL_LIB_DIR})
endif()

# Target dependencies
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBLARIES})
target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIRS})

# Installation
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_INCLUDE_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake DESTINATION ${INSTALL_DIR})

# Testing
option(BUILD_TEST "Build test" Off)
if(BUILD_TEST)
    add_subdirectory(test)
endif()