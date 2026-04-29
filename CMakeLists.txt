cmake_minimum_required(VERSION 4.0.2)
project(pooooooooool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Audio)

set(ASIO_INCLUDE_DIR $ENV{ASIO_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
#file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/include/*.hpp ${CMAKE_SOURCE_DIR}/include/*.h)

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/main.cpp ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${ASIO_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Audio)
target_compile_definitions(${PROJECT_NAME} PRIVATE ASIO_STANDALONE)

file(GLOB RESOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/resource/*.txt ${CMAKE_SOURCE_DIR}/resource/*.csv)
foreach(RES ${RESOURCES})
    configure_file(${RES} ${CMAKE_BINARY_DIR} COPYONLY)
endforeach()
