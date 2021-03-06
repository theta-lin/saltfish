cmake_minimum_required(VERSION 3.10)
project(saltfish)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(SDL2 REQUIRED MODULE)
find_package(SDL2_ttf REQUIRED MODULE)

include_directories("${PROJECT_SOURCE_DIR}/src" ${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIRS})
file(GLOB SRC_FILES
	"${PROJECT_SOURCE_DIR}/src/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	)
add_executable(saltfish ${SRC_FILES})
target_link_libraries(saltfish ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES})

if(MSVC)
	target_compile_options(saltfish PRIVATE /std:c++17 /W4)
else()
	target_compile_options(saltfish PRIVATE -std=c++17 -Wall -Wextra -pedantic)
endif()

