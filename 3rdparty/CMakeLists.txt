# 3rdparty/reflecting-emscripten settings
set(FORCE_BLAZOR_EMSCRIPTEN_MATCH ON CACHE BOOL "")
set(FORCE_EMSCRIPTEN_VERSION "2.0.23" CACHE INTERNAL "")

add_subdirectory(reflecting-emscripten reflecting-emscripten)

set(WRAPPER_FILE_CONTENT [=[
set(PROJECT_SETTINGS_JSON_OVERRIDE
"{\n\
  \"CurrentProjectSetting\": \"${_CONFIG_}\"\n\
}"
)
file(WRITE "${_PROJECT_DIRECTORY_}/.vs/ProjectSettings.json" "${PROJECT_SETTINGS_JSON_OVERRIDE}")
]=])
	
set(WRAPPER_FILE_PATH "${CMAKE_CURRENT_BINARY_DIR}/cmake/wrapper/override_project_json.cmake" CACHE INTERNAL "")
file(WRITE "${WRAPPER_FILE_PATH}" "${WRAPPER_FILE_CONTENT}")

function(ADD_EMSCRIPTEN_PROJECT _CMAKE_SETTINGS_JSON_PATH_)
	cmake_path(GET _CMAKE_SETTINGS_JSON_PATH_ PARENT_PATH _PROJECT_DIRECTORY_)
	cmake_path(GET _PROJECT_DIRECTORY_ FILENAME _PROJECT_TARGET_NAME_)

	set(_THIS_PROJECT_BUILD_DIRECTORY_ "${_PROJECTS_BUILD_DIRECTORY_}/${_PROJECT_TARGET_NAME_}")
	configure_file("${_CMAKE_SETTINGS_JSON_PATH_}" "${_PROJECT_DIRECTORY_}/CMakeSettings.json")
	unset(_THIS_PROJECT_BUILD_DIRECTORY_)

	# TODO: "C:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\2022\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe"  -G "Ninja"  -DCMAKE_BUILD_TYPE:STRING="Release" -DCMAKE_INSTALL_PREFIX:PATH="C:/private/work/programming/current/GPU-With-C-Sharp-Angular-WASM/build/projects/EmscriptenHTML5/install/WASM-Release" -DCMAKE_TOOLCHAIN_FILE:FILEPATH="C:/private/work/programming/current/GPU-With-C-Sharp-Angular-WASM/build/3rdparty/reflecting-emscripten/3rdparty/external/emscripten/unpack/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" -DCMAKE_EXE_LINKER_FLAGS:STRING="-O3" -DCMAKE_EXE_LINKER_FLAGS_RELEASE:STRING="-O3"  -DCMAKE_MAKE_PROGRAM="C:\PROGRAM FILES\MICROSOFT VISUAL STUDIO\2022\COMMUNITY\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe" "C:\private\work\programming\current\GPU-With-C-Sharp-Angular-WASM\EmscriptenHTML5" 2>&1"

	add_custom_target(${_PROJECT_TARGET_NAME_} ALL
		COMMAND "${CMAKE_COMMAND}" "-D_PROJECT_DIRECTORY_=${_PROJECT_DIRECTORY_}" "-D_CONFIG_=WASM-$<CONFIG>" -P "${WRAPPER_FILE_PATH}" # update project's ProjectSettings.json in order to target proper configuration while requesting the build
		COMMAND "${VCVARS64_BAT}" # initialize vc environment
		COMMAND "devenv.exe" "${_PROJECT_DIRECTORY_}" # configure Visual Studio's CMake project
		COMMAND "${CMAKE_COMMAND}" -E rm -R "${_PROJECT_DIRECTORY_}/.vs"
	)
endfunction()