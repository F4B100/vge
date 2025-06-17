cmake_minimum_required(VERSION 3.31)
project(vge C ASM)

set(CMAKE_C_STANDARD 23)

include(cmake/CPM.cmake)

file(GLOB_RECURSE SRC_FILES "src/*.c" "src/*.h" "src/*.asm")
file(GLOB_RECURSE SHADERS "shaders/*.frag" "shaders/*.vert" "shaders/*.geom" "shaders/*.comp")

set(SPIRV_OUTPUTS "")

file(MAKE_DIRECTORY "shaders_bin")

find_program(GLSLANG_VALIDATOR glslangValidator REQUIRED)

foreach (shader ${SHADERS})
	get_filename_component(FILE_NAME ${shader} NAME)
    get_filename_component(FILE_NAME_WE ${shader} NAME_WE)
    get_filename_component(dir ${shader} DIRECTORY)

	set(SPIRV "shaders_bin/${FILE_NAME}.spv")
	list(APPEND SPIRV_OUTPUTS ${SPIRV})

	add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSLANG_VALIDATOR} -V ${shader} -o ${SPIRV}
        DEPENDS ${shader}
        COMMENT "Compiling shader ${shader} to SPIR-V ${SPIRV}"
        VERBATIM
    )
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_OUTPUTS})

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

add_dependencies(vge CompileShaders)