cmake_minimum_required(VERSION 3.31)
project(vge C ASM)

set(CMAKE_C_STANDARD 23)

include(cmake/CPM.cmake)

file(GLOB_RECURSE SRC_FILES "src/*.c" "src/*.h")

CPMAddPackage(
		NAME cglm
		GITHUB_REPOSITORY recp/cglm
		VERSION 0.9.6
)
CPMAddPackage(
		NAME glfw
		GITHUB_REPOSITORY glfw/glfw
		GIT_TAG 3.4
)

CPMAddPackage(
		NAME stb
		GITHUB_REPOSITORY nothings/stb
		GIT_TAG master
		DOWNLOAD_ONLY YES
)

add_library(stb INTERFACE)

find_package(Vulkan REQUIRED)

add_executable(vge main.c ${SRC_FILES})

target_link_libraries(vge PRIVATE cglm m glfw stb Vulkan::Vulkan)

target_include_directories(stb INTERFACE ${stb_SOURCE_DIR} ${glfw_INCLUDE_DIR})