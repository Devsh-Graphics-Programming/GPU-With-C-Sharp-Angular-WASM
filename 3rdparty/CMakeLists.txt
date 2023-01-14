include(ExternalProject)
include(FetchContent)
set(FC_EXTERNAL_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external")

find_package (Python3 COMPONENTS Interpreter REQUIRED)

# Swig
if(DEFINED SWIG_DIR)
	if(NOT EXISTS "${SWIG_DIR}")
		message(FATAL_ERROR "SWIG_DIR filepath doesn't exist!")
	endif()
	
	set(SWIG_UNPACK_DIR "${SWIG_DIR}" CACHE INTERNAL "")
else()
	set(SWIG_BINARY_DIR "${FC_EXTERNAL_BINARY_DIR}/swig" CACHE INTERNAL "")
	set(SWIG_DOWNLOAD_DIR "${SWIG_BINARY_DIR}/download" CACHE INTERNAL "")
	set(SWIG_UNPACK_DIR "${SWIG_BINARY_DIR}/unpack" CACHE INTERNAL "")

	FetchContent_Declare(
		SWIG
		URL 			  "https://artifactory.devsh.eu/Swig/swig.tar.bz2"
		DOWNLOAD_DIR      "${SWIG_DOWNLOAD_DIR}"
		SOURCE_DIR		  "${SWIG_UNPACK_DIR}"
		BINARY_DIR        "${SWIG_BINARY_DIR}"
	)

	if(NOT DEFINED FC_SWIG_POPULATED OR NOT ${FC_SWIG_POPULATED})
		message(STATUS "Populating SWIG...")
		
		FetchContent_MakeAvailable(SWIG)
		FetchContent_GetProperties(SWIG POPULATED _FC_SWIG_POPULATED_)
		
		if(${_FC_SWIG_POPULATED_})
			set(FC_SWIG_POPULATED ON CACHE INTERNAL "")
		else()
			set(FC_SWIG_POPULATED OFF CACHE INTERNAL "")
		endif()
		
		unset(_FC_SWIG_POPULATED_)
	endif()
endif()

set(SWIG_INSTALL_DIR "${SWIG_UNPACK_DIR}" CACHE INTERNAL "")
set(SWIG_EXECUTABLE "${SWIG_INSTALL_DIR}/swig.exe" CACHE STRING "")

if(NOT EXISTS "${SWIG_EXECUTABLE}")
	message(FATAL_ERROR "SWIG_EXECUTABLE has been specified but the path doesn't exist!")
endif()

if(DEFINED SWIG_LIB_DIRECTORY)
	if(NOT EXISTS "${SWIG_LIB_DIRECTORY}")
		message(FATAL_ERROR "SWIG_LIB_DIRECTORY has been specified but the path doesn't exist!")
	endif()
else()
	cmake_path(GET SWIG_EXECUTABLE PARENT_PATH _SWIG_LIB_DIRECTORY_)
	set(SWIG_LIB_DIRECTORY "${_SWIG_LIB_DIRECTORY_}" CACHE STRING "")
endif()

# Emscripten
option(FORCE_NET_EMSCRIPTEN_MATCH "" OFF)
# define FORCE_NET_EMSCRIPTEN_VERSION to request specific Emscripten version, eg. "3.1.12"
# define FORCE_NET_EMSCRIPTEN_NUGET_PACKAGES_VERSION to request specific NuGet version of Emscripten packages, eg. "7.0.1"

unset(_EMSCRIPTEN_RUNTIME_PACKAGES_VERSION_)
unset(_EMSCRIPTEN_SDK_PACKAGE_VERSION_)
unset(_EMSCRIPTEN_PYTHON_PACKAGE_VERSION_)
unset(_EMSCRIPTEN_NODE_PACKAGE_VERSION_)

if(DEFINED EMSCRIPTEN_EMSDK_DIR)
	if(NOT EXISTS "${EMSCRIPTEN_EMSDK_DIR}")
		message(FATAL_ERROR "EMSCRIPTEN_EMSDK_DIR filepath doesn't exist!")
	endif()
else()
	# Dotnet
	find_program(DOTNET_EXE "dotnet.exe" REQUIRED)
	cmake_path(GET DOTNET_EXE PARENT_PATH DOTNET_DIR_EXE)
	set(DOTNET_PACKS_DIR "${DOTNET_DIR_EXE}/packs")

	execute_process(COMMAND "${DOTNET_EXE}" --version
		RESULT_VARIABLE DOTNET_RESULT
		OUTPUT_VARIABLE DOTNET_VERSION
	)

	if(NOT "${DOTNET_RESULT}" STREQUAL "0")
		message(FATAL_ERROR "'${DOTNET_EXE}' --version failed. Inspect your executable!")
	endif()

	string(STRIP "${DOTNET_VERSION}" DOTNET_VERSION)

	message(STATUS "[NET] Found '${DOTNET_EXE}' with '${DOTNET_VERSION}' version")

	if(FORCE_NET_EMSCRIPTEN_MATCH)
		if(NOT DEFINED FORCE_NET_EMSCRIPTEN_VERSION OR "${FORCE_NET_EMSCRIPTEN_VERSION}" STREQUAL "")
			set(FORCE_NET_EMSCRIPTEN_VERSION "*")
		endif()
		
		file(GLOB DOTNET_EMSCRIPTEN_RUNTIME_DIR_NAMES LIST_DIRECTORIES ON RELATIVE "${DOTNET_PACKS_DIR}" "${DOTNET_PACKS_DIR}/Microsoft.NET.Runtime.Emscripten.${FORCE_NET_EMSCRIPTEN_VERSION}.*.win-x64")
		list(LENGTH DOTNET_EMSCRIPTEN_RUNTIME_DIR_NAMES _ERDS_LENGTH_)
		
		if("${_ERDS_LENGTH_}" STREQUAL "0")
			message(FATAL_ERROR "Could not find any Emscripten runtime with requested '${FORCE_NET_EMSCRIPTEN_VERSION}' version. Please check your Visual Studio '.NET WebAssembly build tools' package installation!")
		endif()
		
		foreach(EMSCRIPTEN_RUNTIME_DIR_NAME IN LISTS DOTNET_EMSCRIPTEN_RUNTIME_DIR_NAMES)
			string(FIND "${EMSCRIPTEN_RUNTIME_DIR_NAME}" "Sdk" FOUND_EMSCRIPTEN_SDK_PACKAGE REVERSE)
			string(FIND "${EMSCRIPTEN_RUNTIME_DIR_NAME}" "Python" FOUND_EMSCRIPTEN_PYTHON_PACKAGE REVERSE)
			string(FIND "${EMSCRIPTEN_RUNTIME_DIR_NAME}" "Node" FOUND_EMSCRIPTEN_NODE_PACKAGE REVERSE)

			string(REPLACE "Microsoft.NET.Runtime.Emscripten." "" EMSCRIPTEN_RUNTIME_CURRENT_VERSION "${EMSCRIPTEN_RUNTIME_DIR_NAME}")
			string(REGEX REPLACE "\.[A-Za-z]*\.win-x64$" "" EMSCRIPTEN_RUNTIME_CURRENT_VERSION "${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}")

			if(NOT "${FOUND_EMSCRIPTEN_SDK_PACKAGE}" STREQUAL "-1") # Sdk package
				if(NOT DEFINED _EMSCRIPTEN_SDK_PACKAGE_VERSION_)
					set(_EMSCRIPTEN_SDK_PACKAGE_VERSION_ "${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}")
				endif()
				
				if("${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}" VERSION_GREATER "${_EMSCRIPTEN_SDK_PACKAGE_VERSION_}")
					set(_EMSCRIPTEN_SDK_PACKAGE_VERSION_ "${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}")
				endif()
			elseif(NOT "${FOUND_EMSCRIPTEN_PYTHON_PACKAGE}" STREQUAL "-1") # Python package
				if(NOT DEFINED _EMSCRIPTEN_PYTHON_PACKAGE_VERSION_)
					set(_EMSCRIPTEN_PYTHON_PACKAGE_VERSION_ "${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}")
				endif()
				
				if("${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}" VERSION_GREATER "${_EMSCRIPTEN_PYTHON_PACKAGE_VERSION_}")
					set(_EMSCRIPTEN_PYTHON_PACKAGE_VERSION_ "${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}")
				endif()
			elseif(NOT "${FOUND_EMSCRIPTEN_NODE_PACKAGE}" STREQUAL "-1") # Node package
				if(NOT DEFINED _EMSCRIPTEN_NODE_PACKAGE_VERSION_)
					set(_EMSCRIPTEN_NODE_PACKAGE_VERSION_ "${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}")
				endif()
				
				if("${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}" VERSION_GREATER "${_EMSCRIPTEN_NODE_PACKAGE_VERSION_}")
					set(_EMSCRIPTEN_NODE_PACKAGE_VERSION_ "${EMSCRIPTEN_RUNTIME_CURRENT_VERSION}")
				endif()
			endif()
		endforeach()
		
		if(NOT DEFINED _EMSCRIPTEN_SDK_PACKAGE_VERSION_)
			message(FATAL_ERROR "Could not find any 'Microsoft.NET.Runtime.Emscripten.${FORCE_NET_EMSCRIPTEN_VERSION}.Sdk.win-x64' package. Make sure your '.NET WebAssembly build tools' Visual Studio package is installed and is complete or use 'EMSCRIPTEN_EMSDK_DIR' variable!")
		endif()

		if(NOT DEFINED _EMSCRIPTEN_PYTHON_PACKAGE_VERSION_)
			message(FATAL_ERROR "Could not find any 'Microsoft.NET.Runtime.Emscripten.${FORCE_NET_EMSCRIPTEN_VERSION}.Python.win-x64' package. Make sure your '.NET WebAssembly build tools' Visual Studio package is installed and is complete or use 'EMSCRIPTEN_EMSDK_DIR' variable!")
		endif()

		if(NOT DEFINED _EMSCRIPTEN_NODE_PACKAGE_VERSION_)
			message(FATAL_ERROR "Could not find any 'Microsoft.NET.Runtime.Emscripten.${FORCE_NET_EMSCRIPTEN_VERSION}.Node.win-x64' package. Make sure your '.NET WebAssembly build tools' Visual Studio package is installed and is complete or use 'EMSCRIPTEN_EMSDK_DIR' variable!")
		endif()

		set(_EMSCRIPTEN_RUNTIME_PACKAGES_VERSION_ "${_EMSCRIPTEN_SDK_PACKAGE_VERSION_}")
		if("${_EMSCRIPTEN_SDK_PACKAGE_VERSION_}" VERSION_EQUAL "${_EMSCRIPTEN_PYTHON_PACKAGE_VERSION_}" AND "${_EMSCRIPTEN_PYTHON_PACKAGE_VERSION_}" VERSION_EQUAL "${_EMSCRIPTEN_NODE_PACKAGE_VERSION_}")
			message(STATUS "[NET] Found Emscripten '${_EMSCRIPTEN_RUNTIME_PACKAGES_VERSION_}'")
		else()
			message(FATAL_ERROR "Found all Emscripten required packages but their versions are not same. Please make sure your '.NET WebAssembly build tools' Visual Studio package is installed and is complete or use 'EMSCRIPTEN_EMSDK_DIR' variable!\n_EMSCRIPTEN_SDK_PACKAGE_VERSION_='${_EMSCRIPTEN_SDK_PACKAGE_VERSION_}'\n_EMSCRIPTEN_PYTHON_PACKAGE_VERSION_='${_EMSCRIPTEN_PYTHON_PACKAGE_VERSION_}'\n_EMSCRIPTEN_NODE_PACKAGE_VERSION_='${_EMSCRIPTEN_NODE_PACKAGE_VERSION_}'")
		endif()
		
		unset(DOTNET_EMSCRIPTEN_BINARYEN_ROOT)
		unset(DOTNET_EMSCRIPTEN_LLVM_ROOT)
		unset(DOTNET_EMSCRIPTEN_NODE_JS)

		if(NOT DEFINED FORCE_NET_EMSCRIPTEN_NUGET_PACKAGES_VERSION)
			file(GLOB DOTNET_EMSCRIPTEN_SDK_PACKAGE_DIRS LIST_DIRECTORIES ON RELATIVE "${DOTNET_PACKS_DIR}/Microsoft.NET.Runtime.Emscripten.${_EMSCRIPTEN_SDK_PACKAGE_VERSION_}.Sdk.win-x64" "${DOTNET_PACKS_DIR}/Microsoft.NET.Runtime.Emscripten.${_EMSCRIPTEN_SDK_PACKAGE_VERSION_}.Sdk.win-x64/*")
		
			list(LENGTH DOTNET_EMSCRIPTEN_SDK_PACKAGE_DIRS _TMP_LENGTH_)
			if(NOT "${_TMP_LENGTH_}" STREQUAL "1")
				message(FATAL_ERROR "Expected only one version of dotnet package for the Sdk runtime installed by Visual Studio! Make sure your '.NET WebAssembly build tools' Visual Studio package is installed and is complete or use 'EMSCRIPTEN_EMSDK_DIR' variable!\n\nIf for some reason there are more NuGet package versions of Emscripten then define FORCE_NET_EMSCRIPTEN_NUGET_PACKAGES_VERSION to point to proper one!")
			endif()

			set(FORCE_NET_EMSCRIPTEN_NUGET_PACKAGES_VERSION "${DOTNET_EMSCRIPTEN_SDK_PACKAGE_DIRS}")
		endif()
		
		set(_VS_EMSCRIPTEN_EMSDK_DIR_ "${DOTNET_PACKS_DIR}/Microsoft.NET.Runtime.Emscripten.${_EMSCRIPTEN_SDK_PACKAGE_VERSION_}.Sdk.win-x64/${FORCE_NET_EMSCRIPTEN_NUGET_PACKAGES_VERSION}/tools/emscripten")
		cmake_path(GET _VS_EMSCRIPTEN_EMSDK_DIR_ PARENT_PATH DOTNET_EMSCRIPTEN_BINARYEN_ROOT)
		set(DOTNET_EMSCRIPTEN_LLVM_ROOT "${DOTNET_EMSCRIPTEN_BINARYEN_ROOT}/bin")
		set(DOTNET_EMSCRIPTEN_NODE_JS "${DOTNET_PACKS_DIR}/Microsoft.NET.Runtime.Emscripten.${_EMSCRIPTEN_SDK_PACKAGE_VERSION_}.Node.win-x64/${FORCE_NET_EMSCRIPTEN_NUGET_PACKAGES_VERSION}/tools/bin")
			
		if(NOT EXISTS "${DOTNET_EMSCRIPTEN_BINARYEN_ROOT}")
			message(FATAL_ERROR "DOTNET_EMSCRIPTEN_BINARYEN_ROOT:'${DOTNET_EMSCRIPTEN_BINARYEN_ROOT}' has been requested but it doesn't exist!")
		endif()
		
		if(NOT EXISTS "${DOTNET_EMSCRIPTEN_LLVM_ROOT}")
			message(FATAL_ERROR "DOTNET_EMSCRIPTEN_LLVM_ROOT:'${DOTNET_EMSCRIPTEN_LLVM_ROOT}' has been requested but it doesn't exist!")
		endif()
		
		if(NOT EXISTS "${DOTNET_EMSCRIPTEN_NODE_JS}")
			message(FATAL_ERROR "DOTNET_EMSCRIPTEN_NODE_JS:'${DOTNET_EMSCRIPTEN_NODE_JS}' has been requested but it doesn't exist!")
		endif()
		
		message(STATUS "[NET] Found NuGet Emscripten's '${FORCE_NET_EMSCRIPTEN_NUGET_PACKAGES_VERSION}' packages & runtimes")
		
		set(EMSCRIPTEN_ENV_SET_BAT_FILE "${CMAKE_CURRENT_BINARY_DIR}/emscripten/env/setEnvs.cmd" CACHE FILEPATH INTERNAL FORCE)	
		set(EMSCRIPTEN_ENV_SET_BAT_WARPPER_CODE
"set \"DOTNET_EMSCRIPTEN_BINARYEN_ROOT=${DOTNET_EMSCRIPTEN_BINARYEN_ROOT}\"\n\
set \"DOTNET_EMSCRIPTEN_LLVM_ROOT=${DOTNET_EMSCRIPTEN_LLVM_ROOT}\"\n\
set \"DOTNET_EMSCRIPTEN_NODE_JS=${DOTNET_EMSCRIPTEN_NODE_JS}\""
		)
			
		file(WRITE "${EMSCRIPTEN_ENV_SET_BAT_FILE}" "${EMSCRIPTEN_ENV_SET_BAT_WARPPER_CODE}")	
	else()
		set(_EMSCRIPTEN_RUNTIME_PACKAGES_VERSION_ latest)
	endif()
	
	set(EMSCRIPTEN_BINARY_DIR "${FC_EXTERNAL_BINARY_DIR}/emscripten" CACHE INTERNAL "")
	set(EMSCRIPTEN_DOWNLOAD_DIR "${EMSCRIPTEN_BINARY_DIR}/download" CACHE INTERNAL "")
	set(EMSCRIPTEN_UNPACK_DIR "${EMSCRIPTEN_BINARY_DIR}/unpack" CACHE INTERNAL "")
	set(EMSCRIPTEN_INSTALL_DIR "${EMSCRIPTEN_UNPACK_DIR}" CACHE INTERNAL "")
	FetchContent_Declare(
		Emscripten
		GIT_REPOSITORY    "https://github.com/Devsh-Graphics-Programming/emsdk.git"
		GIT_TAG 		  e41b8c68a248da5f18ebd03bd0420953945d52ff # 3.1.24, but use it as a hash!
		DOWNLOAD_DIR      "${EMSCRIPTEN_DOWNLOAD_DIR}"
		SOURCE_DIR		  "${EMSCRIPTEN_UNPACK_DIR}"
		BINARY_DIR        "${EMSCRIPTEN_INSTALL_DIR}"
	)
	
	if(NOT DEFINED FC_EMSCRIPTEN_POPULATED OR NOT ${FC_EMSCRIPTEN_POPULATED})
		message(STATUS "Populating Emscripten...")
		
		FetchContent_MakeAvailable(Emscripten)
		FetchContent_GetProperties(Emscripten POPULATED _FC_EMSCRIPTEN_POPULATED_)
		
		if(${_FC_EMSCRIPTEN_POPULATED_})
			set(FC_EMSCRIPTEN_POPULATED ON CACHE INTERNAL "")
		
			execute_process(
				COMMAND "${EMSCRIPTEN_UNPACK_DIR}/emsdk.bat" install ${_EMSCRIPTEN_RUNTIME_PACKAGES_VERSION_}
				OUTPUT_QUIET
				ERROR_QUIET
			)
			
			execute_process(
				COMMAND "${EMSCRIPTEN_UNPACK_DIR}/emsdk.bat" activate ${_EMSCRIPTEN_RUNTIME_PACKAGES_VERSION_}
				OUTPUT_QUIET
				ERROR_QUIET
			)
		else()
			set(FC_EMSCRIPTEN_POPULATED OFF CACHE INTERNAL "")
		endif()
		
		unset(_FC_EMSCRIPTEN_POPULATED_)
	endif()
	
	set(EMSCRIPTEN_EMSDK_DIR "${EMSCRIPTEN_UNPACK_DIR}/upstream/emscripten" CACHE PATH "" FORCE)
endif()

find_file(EMSCRIPTEN_TOOLCHAIN_FILE "Emscripten.cmake" 
	HINTS "${_EMSCRIPTEN_TOOLCHAIN_FILE_HINT_DIR_}" # define this variable in case find_file cannot find your toolchain file
	HINTS "${EMSCRIPTEN_EMSDK_DIR}"
	HINTS "${EMSCRIPTEN_EMSDK_DIR}/cmake/Modules/Platform"
	HINTS "${EMSCRIPTEN_EMSDK_DIR}/upstream/emscripten/cmake/Modules/Platform"
	REQUIRED
)

find_file(EMSCRIPTEN_EMSDK_ENV_BAT_FILE "emsdk_env.bat"
	HINTS "${EMSCRIPTEN_EMSDK_DIR}"
	HINTS "${EMSCRIPTEN_EMSDK_DIR}/../../"
	REQUIRED
)

find_file(EMSCRIPTEN_EMCC_BAT_FILE "emcc.bat"
	HINTS "${EMSCRIPTEN_EMSDK_DIR}"
	REQUIRED
)

find_file(EMSCRIPTEN_EMAR_BAT_FILE "emar.bat"
	HINTS "${EMSCRIPTEN_EMSDK_DIR}"
	REQUIRED
)


if(NOT "${EMSCRIPTEN_EMSDK_DIR}/emcc.bat" STREQUAL "${EMSCRIPTEN_EMCC_BAT_FILE}")
	message(FATAL_ERROR "EMSCRIPTEN_EMSDK_DIR location is inproper!")
endif()

# VC vars64
get_filename_component(
	MY_COMPILER_DIR
	${CMAKE_CXX_COMPILER} DIRECTORY
)
find_file(
	VCVARS64_BAT vcvars64.bat
	"${MY_COMPILER_DIR}/../../../../../../Auxiliary/Build"
	REQUIRED
)

# we cannot rely on MSBuild corresponding for certain arch, therefore using CMAKE_VS_MSBUILD_COMMAND is a bad idea
find_file(
	VCMSBUILD_EXE MSBuild.exe
	HINTS "${MY_COMPILER_DIR}/../../../../../../../MSBuild/Current/Bin"
	REQUIRED
)

find_file(
	VSDEVENV_EXE devenv.exe
	HINTS "${MY_COMPILER_DIR}/../../../../../../../Common7/IDE"
	REQUIRED
)

set(EMRUN_PYTHON_SCRIPT_PATH "${CMAKE_CURRENT_BINARY_DIR}/cmake/run.py" CACHE INTERNAL "" FORCE)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/run.py.cmake" "${EMRUN_PYTHON_SCRIPT_PATH}")
string(REPLACE "/" "\\\\" Python3_EXECUTABLE_NP "${Python3_EXECUTABLE}")
set(Python3_EXECUTABLE "${Python3_EXECUTABLE_NP}" CACHE INTERNAL "" FORCE)

# CMake utilities
function(ADD_EMSCRIPTEN_PROJECT _CMAKE_SETTINGS_JSON_PATH_)
	cmake_path(GET _CMAKE_SETTINGS_JSON_PATH_ PARENT_PATH _PROJECT_DIRECTORY_)
	cmake_path(GET _PROJECT_DIRECTORY_ FILENAME _PROJECT_TARGET_NAME_)

	set(_THIS_PROJECT_BUILD_DIRECTORY_ "${_PROJECTS_BUILD_DIRECTORY_}/${_PROJECT_TARGET_NAME_}")
	configure_file("${_CMAKE_SETTINGS_JSON_PATH_}" "${_PROJECT_DIRECTORY_}/CMakeSettings.json")
	configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake/launch.vs.json.cmake" "${_PROJECT_DIRECTORY_}/.vs/launch.vs.json" @ONLY)
	
	if(GENERATE_BUILD_ONLY_TARGETS)
		set(_PROJECT_EXT_SOURCE_DIR_ "${_PROJECT_DIRECTORY_}")
		set(_PROJECT_EXT_BINARY_DIR_ "${_THIS_PROJECT_BUILD_DIRECTORY_}/ext")
		set(_PROJECT_EXT_GE_BINARY_DIR_ "${_PROJECT_EXT_BINARY_DIR_}/build/WASM-$<CONFIG>")
		
		list(APPEND PROJECT_CMAKE_ARGS "-DCMAKE_BUILD_TYPE:STRING=WASM-$<CONFIG>") 
		list(APPEND PROJECT_CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=${_PROJECT_EXT_GE_BINARY_DIR_}/install")
		list(APPEND PROJECT_CMAKE_ARGS "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${EMSCRIPTEN_TOOLCHAIN_FILE}")
		list(APPEND PROJECT_CMAKE_ARGS "-DGENERATE_BUILD_ONLY_TARGETS:BOOL=${GENERATE_BUILD_ONLY_TARGETS}")
		
		ExternalProject_Add(${_PROJECT_TARGET_NAME_}
			DOWNLOAD_COMMAND  ""
			SOURCE_DIR        "${_PROJECT_EXT_SOURCE_DIR_}"
			BINARY_DIR        "${_PROJECT_EXT_BINARY_DIR_}"
			BUILD_COMMAND     "${CMAKE_COMMAND}" --build "${_PROJECT_EXT_GE_BINARY_DIR_}"
			CONFIGURE_COMMAND "${CMAKE_COMMAND}" -S "${_PROJECT_EXT_SOURCE_DIR_}" -B "${_PROJECT_EXT_GE_BINARY_DIR_}" -G "Ninja" ${PROJECT_CMAKE_ARGS}
			INSTALL_COMMAND	  ""
			TEST_COMMAND      ""
			BUILD_ALWAYS  	  1
			LOG_INSTALL		  1
		)
		
		execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory "${_THIS_PROJECT_BUILD_DIRECTORY_}"
				COMMAND "${CMAKE_COMMAND}" -E copy_directory "${SERVE_ROOT_PATH}/data" "${_THIS_PROJECT_BUILD_DIRECTORY_}/data"
		)
		
		unset(PROJECT_CMAKE_ARGS)
	endif()
	
	unset(_THIS_PROJECT_BUILD_DIRECTORY_)
endfunction()

function(ADD_BLAZOR_EMSCRIPTEN_PROJECT _PROJECT_PATH_)
	if(GENERATE_BUILD_ONLY_TARGETS)
		cmake_path(GET _PROJECT_PATH_ FILENAME _PROJECT_TARGET_NAME_)

		set(_THIS_PROJECT_BUILD_DIRECTORY_ "${_PROJECTS_BUILD_DIRECTORY_}/${_PROJECT_TARGET_NAME_}")
		set(NET_VERSION "net6.0")
	
		add_custom_target(${_PROJECT_TARGET_NAME_} ALL
			COMMAND "${VCMSBUILD_EXE}" ${_PROJECT_TARGET_NAME_}.sln -t:restore -p:RestorePackagesConfig=true
			COMMAND "${VCMSBUILD_EXE}" ${_PROJECT_TARGET_NAME_}.sln -p:Configuration=$<IF:$<CONFIG:Debug>,Debug,Release>
			COMMAND "${VSDEVENV_EXE}" BlazorEmscripten.sln $<IF:$<CONFIG:Debug>,Debug,Release> /Deploy
			COMMAND "${CMAKE_COMMAND}" -E copy_directory "./Client/wwwroot" "./Client/bin/$<IF:$<CONFIG:Debug>,Debug,Release>/${NET_VERSION}/wwwroot"
			COMMAND "${CMAKE_COMMAND}" -E copy_directory "${SERVE_ROOT_PATH}/data" "./Client/bin/$<IF:$<CONFIG:Debug>,Debug,Release>/${NET_VERSION}/wwwroot/data"
			COMMAND "${CMAKE_COMMAND}" -E copy_directory "./ClientNoRazor/wwwroot" "./ClientNoRazor/bin/$<IF:$<CONFIG:Debug>,Debug,Release>/${NET_VERSION}/wwwroot"
			COMMAND "${CMAKE_COMMAND}" -E copy_directory "${SERVE_ROOT_PATH}/data" "./ClientNoRazor/bin/$<IF:$<CONFIG:Debug>,Debug,Release>/${NET_VERSION}/wwwroot/data"
			WORKING_DIRECTORY "${_PROJECT_PATH_}"
			VERBATIM
		)
		configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake/cmake.props.cmake" "${_PROJECT_PATH_}/cmake.props")
		
		execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory "${_PROJECT_PATH_}/Client/SWIG")
		execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory "${_PROJECT_PATH_}/ClientNoRazor/SWIG")
	endif()
endfunction()

function(ADD_NATIVE_PROJECT _PROJECT_PATH_ _IS_CPP_)
	if(GENERATE_BUILD_ONLY_TARGETS)
		cmake_path(GET _PROJECT_PATH_ FILENAME _PROJECT_TARGET_NAME_)

		set(_THIS_PROJECT_BUILD_DIRECTORY_ "${_PROJECTS_BUILD_DIRECTORY_}/${_PROJECT_TARGET_NAME_}")
	
		if(_IS_CPP_) # C++ Native
			add_subdirectory("${_PROJECT_PATH_}" "${_THIS_PROJECT_BUILD_DIRECTORY_}")
		else() # C-Sharp Native
			add_custom_target(${_PROJECT_TARGET_NAME_} ALL
				COMMAND "${DOTNET_EXE}" build ${_PROJECT_TARGET_NAME_}.csproj -c $<IF:$<CONFIG:Debug>,Debug,Release> -o "${_THIS_PROJECT_BUILD_DIRECTORY_}"
				WORKING_DIRECTORY "${_PROJECT_PATH_}"
				VERBATIM
			)
			
			configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake/Directory.Build.props.cmake" "${_PROJECT_PATH_}/Directory.Build.props")
		endif()
	endif()
endfunction()

# do not specify configuration for a component - we are assuming that all of them will be built while invoking this
install(DIRECTORY "${_PROJECTS_BUILD_DIRECTORY_}"
		DESTINATION "${CMAKE_BINARY_DIR}/ci/wwwroot"
		COMPONENT CI_DEPLOY
		PATTERN "Native*" EXCLUDE
)

install(DIRECTORY "${SERVE_ROOT_PATH}/BlazorEmscripten/Client/bin"
		DESTINATION "${CMAKE_BINARY_DIR}/ci/wwwroot/projects/BlazorEmscripten/Client"
		COMPONENT CI_DEPLOY
)

install(DIRECTORY "${SERVE_ROOT_PATH}/BlazorEmscripten/ClientNoRazor/bin"
		DESTINATION "${CMAKE_BINARY_DIR}/ci/wwwroot/projects/BlazorEmscripten/ClientNoRazor"
		COMPONENT CI_DEPLOY
)