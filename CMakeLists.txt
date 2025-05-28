cmake_minimum_required(VERSION 3.12)

project(game)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DDEBUG)

file(GLOB assets "./assets")
file(COPY ${assets} DESTINATION ${CMAKE_BINARY_DIR})

include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/include
)
link_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/lib
)

add_subdirectory(wrapper)
add_subdirectory(Application)
add_subdirectory(glframework)


add_executable(game "main.cpp" "glad.c")

target_link_libraries(game "glfw3.lib" assimp-vc143-mtd.lib zlibstaticd.lib wrapper app fw)