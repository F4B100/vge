cmake_minimum_required(VERSION 3.31)
project(vge C ASM)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Platform Detection ------------------------------------------------------------------------------

if (WIN32)
	add_compile_definitions(VGE_PLATFORM_WIN32)
ELSEIF (LINUX)
	message("WAYLAND!!!!!!!")
	add_compile_definitions(VGE_PLATFORM_WAYLAND)
ELSE ()

ENDIF ()

# -------------------------------------------------------------------------------------------------

# Shaders Compile Section -------------------------------------------------------------------------

file(GLOB_RECURSE SHADERS "shaders/*.frag" "shaders/*.vert" "shaders/*.geom" "shaders/*.comp")
set(SPIRV_OUTPUTS "")

foreach (shader ${SHADERS})
	get_filename_component(FILE_NAME ${shader} NAME)
	get_filename_component(FILE_NAME_WE ${shader} NAME_WE)
	get_filename_component(dir ${shader} DIRECTORY)

	set(SPIRV "${CMAKE_HOME_DIRECTORY}/shaders_bin/${FILE_NAME}.spv")
	message("${CMAKE_HOME_DIRECTORY}/shaders_bin/${FILE_NAME}.spv")
	list(APPEND SPIRV_OUTPUTS ${SPIRV})

	add_custom_command(
			OUTPUT ${SPIRV}
			COMMAND ${GLSLC} ${shader} -o ${SPIRV}
			DEPENDS ${shader}
			COMMENT "Compiling shader ${shader} to SPIR-V ${SPIRV}"
			VERBATIM
	)
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_OUTPUTS})
#--------------------------------------------------------------------------------------------------

# External Deps Section ---------------------------------------------------------------------------
include(cmake/CPM.cmake)
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
# -------------------------------------------------------------------------------------------------

# Add Deps Section --------------------------------------------------------------------------------
file(GLOB_RECURSE SRC_FILES "src/*.c" "src/*.h" "src/*.s")
add_executable(vge main.c ${SRC_FILES})


target_link_libraries(vge PRIVATE cglm m glfw stb Vulkan::Vulkan)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR} ${cglm_SORCE_DIR} ${glfw_INCLUDE_DIR})
add_dependencies(vge CompileShaders)
# -------------------------------------------------------------------------------------------------
