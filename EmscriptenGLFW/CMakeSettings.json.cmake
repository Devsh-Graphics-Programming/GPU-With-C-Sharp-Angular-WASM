{
  "configurations": [
    {
      "buildCommandArgs": "",
      "buildRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@/WASM-RelWithDebInfo",
      "cmakeCommandArgs": "",
      "cmakeToolchain": "@EMSCRIPTEN_TOOLCHAIN_FILE@",
      "configurationType": "RelWithDebInfo",
      "ctestCommandArgs": "",
      "generator": "Ninja",
      "inheritEnvironments": [ "linux_x64" ],
      "installRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@/install/WASM-RelWithDebInfo",
      "name": "WASM-RelWithDebInfo",
      "variables": [
        {
          "name": "CMAKE_EXE_LINKER_FLAGS",
          "value": "-O1 -g --emrun -gsplit-dwarf -gseparate-dwarf=DWARF_debug_symbols.dbg.wasm -gdwarf-5",
          "type": "STRING"
        }
      ],
	  "environments": [
        {
          "ENV_HTML_FILE": "@_THIS_PROJECT_BUILD_DIRECTORY_@/WASM-RelWithDebInfo/@_PROJECT_TARGET_NAME_@.html"
        }
      ]
    },
    {
      "buildCommandArgs": "",
      "buildRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@/WASM-Release",
      "cmakeCommandArgs": "",
      "cmakeToolchain": "@EMSCRIPTEN_TOOLCHAIN_FILE@",
      "configurationType": "Release",
      "ctestCommandArgs": "",
      "generator": "Ninja",
      "inheritEnvironments": [ "linux_x64" ],
      "installRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@/install/WASM-Release",
      "name": "WASM-Release",
      "variables": [
        {
          "name": "CMAKE_EXE_LINKER_FLAGS",
          "value": "-O3",
          "type": "STRING"
        }
      ],
	  "environments": [
        {
          "ENV_HTML_FILE": "@_THIS_PROJECT_BUILD_DIRECTORY_@/WASM-Release/@_PROJECT_TARGET_NAME_@.html"
        }
      ]
    },
    {
      "name": "WASM-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "buildRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@/WASM-Debug",
      "installRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@/install/WASM-Debug",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": "",
      "cmakeToolchain": "@EMSCRIPTEN_TOOLCHAIN_FILE@",
      "inheritEnvironments": [ "linux_x64" ],
      "variables": [
        {
          "name": "CMAKE_EXE_LINKER_FLAGS",
          "value": "-O0 -g --emrun -gdwarf-5 -gsplit-dwarf -gseparate-dwarf=DWARF_debug_symbols.dbg.wasm",
          "type": "STRING"
        }
      ],
	  "environments": [
        {
          "ENV_HTML_FILE": "@_THIS_PROJECT_BUILD_DIRECTORY_@/WASM-Debug/@_PROJECT_TARGET_NAME_@.html"
        }
      ]
    }
  ]
}