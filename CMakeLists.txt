cmake_minimum_required(VERSION 4.0.2)
project(pooooooooool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Audio)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.hpp include/*.h)

add_executable(${PROJECT_NAME} main.cpp ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Audio)

file(GLOB RESOURCES CONFIGURE_DEPENDS resource/*.txt resource/*.csv resource/*.png)
foreach(RES ${RESOURCES})
    configure_file(${RES} ${CMAKE_BINARY_DIR} COPYONLY)
endforeach()