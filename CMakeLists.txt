cmake_minimum_required(VERSION 3.10)
project(saltfish)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
find_package(SDL2 REQUIRED MODULE)
set(CXX_STANDARD 17)
include_directories("${PROJECT_SOURCE_DIR}/src" ${SDL2_INCLUDE_DIR})
file(GLOB SRC_FILES
	"${PROJECT_SOURCE_DIR}/src/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	)
add_executable(saltfish ${SRC_FILES})
target_link_libraries(saltfish ${SDL2_LIBRARY})

