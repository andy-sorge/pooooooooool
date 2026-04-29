cmake_minimum_required(VERSION 4.1)
project(pooooooooool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
#file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/include/*.hpp include/*.h)

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/main.cpp ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/external/asio/asio/include)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)

file(GLOB RESOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/resource/*.txt ${CMAKE_SOURCE_DIR}/resource/*.csv)
foreach(RES ${RESOURCES})
    configure_file(${RES} ${CMAKE_BINARY_DIR} COPYONLY)
endforeach()
