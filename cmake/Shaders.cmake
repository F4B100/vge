find_program(GLSLC glslc REQUIRED)

function(CompileSpirVShaders SHADERS)

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
endfunction()