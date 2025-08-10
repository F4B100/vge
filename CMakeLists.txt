cmake_minimum_required(VERSION 3.31)
project(vge C ASM)

set(CMAKE_C_STANDARD 23)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

include(cmake/CPM.cmake)

# Parser Compile Section --------------------------------------------------------------------------
find_program(BISON bison REQUIRED)
file(GLOB_RECURSE BISON_FILES "src/parsers/*.y")
set(OUT_BISON "")
foreach (bisonFile ${BISON_FILES})
	get_filename_component(FILE_NAME_WE ${bisonFile} NAME_WE)
	get_filename_component(FILE_NAME ${bisonFile} NAME)
	get_filename_component(BISON_DIR ${bisonFile} DIRECTORY)

	set(OUT_BISON_SOURCE "${FILE_NAME_WE}.tab.c")
	set(OUT_BISON_HEADER "${FILE_NAME_WE}.tab.h")

	add_custom_command(
			OUTPUT ${OUT_BISON_SOURCE};${OUT_BISON_HEADER}
			COMMAND ${BISON} --output=${OUT_BISON_SOURCE} --header=${OUT_BISON_HEADER} ${FILE_NAME}
			WORKING_DIRECTORY ${BISON_DIR}
			DEPENDS ${bisonFile}
			COMMENT "Creating sources for Bison file: ${bisonFile}"
			VERBATIM
	)

	execute_process(
			COMMAND ${BISON} -Wcounterexamples --output=${OUT_BISON_SOURCE} --header=${OUT_BISON_HEADER} ${FILE_NAME}
			WORKING_DIRECTORY ${BISON_DIR}
	)

	list(APPEND OUT_BISON ${OUT_BISON_SOURCE};${OUT_BISON_HEADER})
endforeach ()
message("===================BISON FILES====================")
message("${OUT_BISON}")
message("==================================================")
add_custom_target(GenerateBison ALL DEPENDS ${OUT_BISON})


find_program(FLEX flex REQUIRED)
file(GLOB_RECURSE FLEX_FILES "src/parsers/*.l")
set(OUT_FLEX "")
foreach (flexFile ${FLEX_FILES})
	get_filename_component(FILE_NAME_WE ${flexFile} NAME_WE)
	get_filename_component(FILE_NAME ${flexFile} NAME)
	get_filename_component(FLEX_DIR ${flexFile} DIRECTORY)

	set(OUT_FLEX_SOURCE "${FILE_NAME_WE}.c")
	set(OUT_FLEX_HEADER "${FILE_NAME_WE}.h")

	add_custom_command(
			OUTPUT ${OUT_FLEX_SOURCE};${OUT_FLEX_HEADER}
			COMMAND ${FLEX} --outfile=${OUT_FLEX_SOURCE} --header-file=${OUT_FLEX_HEADER} ${FILE_NAME}
			WORKING_DIRECTORY ${FLEX_DIR}
			DEPENDS ${flexFile}
			COMMENT "Creating sources for Flex file: ${flexFile}"
			VERBATIM
	)

	execute_process(
			COMMAND ${FLEX} --outfile=${OUT_FLEX_SOURCE} --header-file=${OUT_FLEX_HEADER} ${FILE_NAME}
			WORKING_DIRECTORY ${FLEX_DIR}
	)

	list(APPEND OUT_FLEX ${OUT_FLEX_SOURCE};${OUT_FLEX_HEADER})
endforeach ()
message("===================FLEX FILES=====================")
message("${OUT_FLEX}")
message("==================================================")
add_custom_target(GenerateFlex ALL DEPENDS ${OUT_FLEX})
# -------------------------------------------------------------------------------------------------

# Shaders Compile Section -------------------------------------------------------------------------
find_program(GLSLC glslc REQUIRED)

file(GLOB_RECURSE SHADERS "shaders/*.frag" "shaders/*.vert" "shaders/*.geom" "shaders/*.comp")
set(SPIRV_OUTPUTS "")

foreach (shader ${SHADERS})
	get_filename_component(FILE_NAME ${shader} NAME)
	get_filename_component(FILE_NAME_WE ${shader} NAME_WE)
	get_filename_component(dir ${shader} DIRECTORY)

	set(SPIRV "${CMAKE_HOME_DIRECTORY}/shaders_bin/${FILE_NAME}.spv")
	list(APPEND SPIRV_OUTPUTS ${SPIRV})

	add_custom_command(
			OUTPUT ${SPIRV}
			COMMAND ${GLSLC} ${shader} -o ${SPIRV}
			DEPENDS ${shader}
			COMMENT "Compiling shader ${shader} to SPIR-V ${SPIRV}"
			VERBATIM
	)
endforeach()

message("===================SPIRV FILES====================")
message("${SPIRV_OUTPUTS}")
message("==================================================")

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_OUTPUTS})
#--------------------------------------------------------------------------------------------------

# External Deps Section ---------------------------------------------------------------------------

if (WIN32)
	add_compile_definitions(VGE_OS_WINDOWS)
	add_compile_definitions(VGE_PLATFORM_WIN32)
ENDIF ()
IF (LINUX)
	add_compile_definitions(VGE_OS_WAYLAND)
	add_compile_definitions(VGE_PLATFORM_WAYLAND)
ENDIF ()
IF (NOT WIN32)
	CPMAddPackage(
		NAME glfw
		GITHUB_REPOSITORY glfw/glfw
		GIT_TAG 3.4
	)
ENDIF ()

CPMAddPackage(
		NAME cglm
		GITHUB_REPOSITORY recp/cglm
		VERSION 0.9.6
)
CPMAddPackage(
		NAME stb
		GITHUB_REPOSITORY nothings/stb
		GIT_TAG master
		DOWNLOAD_ONLY YES
)
add_library(stb INTERFACE)
find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
	add_compile_definitions(VGE_GRAPHICS_VULKAN)
endif ()
# -------------------------------------------------------------------------------------------------

# Add Deps Section --------------------------------------------------------------------------------
file(GLOB_RECURSE SRC_FILES "src/*.c" "src/*.h" "src/*.s")

message("===================SOURCE FILES===================")
message("${SRC_FILES}")
message("==================================================")

add_executable(vge main.c ${SRC_FILES})

IF (WIN32)
	target_link_options(vge PRIVATE -static -static-libgcc -lwinpthread)
ENDIF ()

IF (LINUX)
	target_link_options(vge PRIVATE -lwayland-client)
ENDIF ()

target_link_libraries(vge PRIVATE cglm m stb Vulkan::Vulkan)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR} ${cglm_SORCE_DIR})
add_dependencies(vge CompileShaders)
add_dependencies(vge GenerateBison)
add_dependencies(vge GenerateFlex)
# -------------------------------------------------------------------------------------------------
