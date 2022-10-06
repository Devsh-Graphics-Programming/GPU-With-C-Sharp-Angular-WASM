{
  "configurations": [
    {
      "buildCommandArgs": "",
      "buildRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@\\WASM-RelWithDebInfo",
      "cmakeCommandArgs": "",
      "cmakeToolchain": "@EMSCRIPTEN_TOOLCHAIN_FILE@",
      "configurationType": "RelWithDebInfo",
      "ctestCommandArgs": "",
      "generator": "Ninja",
      "inheritEnvironments": [ "linux_x64" ],
      "installRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@\\install\\WASM-RelWithDebInfo",
      "name": "WASM-RelWithDebInfo",
      "variables": [
        {
          "name": "CMAKE_EXE_LINKER_FLAGS",
          "value": "-O1 -g -gsplit-dwarf -gseparate-dwarf=EmscriptenHTML5.dbg.wasm --emrun",
          "type": "STRING"
        },
        {
          "name": "CMAKE_EXE_LINKER_FLAGS_RWDI",
          "value": "-O1 -g -gsplit-dwarf -gseparate-dwarf=EmscriptenHTML5.dbg.wasm --emrun",
          "type": "STRING"
        }
      ]
    },
    {
      "buildCommandArgs": "",
      "buildRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@\\WASM-Release",
      "cmakeCommandArgs": "",
      "cmakeToolchain": "@EMSCRIPTEN_TOOLCHAIN_FILE@",
      "configurationType": "Release",
      "ctestCommandArgs": "",
      "generator": "Ninja",
      "inheritEnvironments": [ "linux_x64" ],
      "installRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@\\install\\WASM-Release",
      "name": "WASM-Release",
      "variables": [
        {
          "name": "CMAKE_EXE_LINKER_FLAGS",
          "value": "-O3",
          "type": "STRING"
        },
        {
          "name": "CMAKE_EXE_LINKER_FLAGS_RELEASE",
          "value": "-O3",
          "type": "STRING"
        }
      ]
    },
    {
      "name": "WASM-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "buildRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@\\WASM-Debug",
      "installRoot": "@_THIS_PROJECT_BUILD_DIRECTORY_@\\install\\WASM-Debug",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": "",
      "cmakeToolchain": "@EMSCRIPTEN_TOOLCHAIN_FILE@",
      "inheritEnvironments": [ "linux_x64" ],
      "variables": [
        {
          "name": "CMAKE_C_FLAGS_DEBUG",
          "value": "-O0 -g --emrun",
          "type": "STRING"
        },
        {
          "name": "CMAKE_EXE_LINKER_FLAGS",
          "value": "-O0 -g -gsplit-dwarf -gseparate-dwarf=EmscriptenHTML5.dbg.wasm --emrun",
          "type": "STRING"
        }
      ]
    }
  ]
}