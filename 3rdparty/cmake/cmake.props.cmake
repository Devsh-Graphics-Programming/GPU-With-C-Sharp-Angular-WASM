<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ImportGroup Label="PropertySheets" />
  <PropertyGroup Label="UserMacros" />
  <PropertyGroup Label="Configuration" Condition="'$(Configuration)|$(Platform)'=='Debug|Emscripten'">
	<CMAKE_EMSCRIPTEN_EMSDK_DIR>@EMSCRIPTEN_EMSDK_DIR@</CMAKE_EMSCRIPTEN_EMSDK_DIR>
	<CMAKE_SWIG_EXECUTABLE>@SWIG_EXECUTABLE@</CMAKE_SWIG_EXECUTABLE>
	<CMAKE_SWIG_LIB_DIRECTORY>@SWIG_LIB_DIRECTORY@</CMAKE_SWIG_LIB_DIRECTORY>
    <CMAKE_PYTHON>@Python3_EXECUTABLE@</CMAKE_PYTHON>
  </PropertyGroup>
  <PropertyGroup Label="Configuration" Condition="'$(Configuration)|$(Platform)'=='Release|Emscripten'">
	<CMAKE_EMSCRIPTEN_EMSDK_DIR>@EMSCRIPTEN_EMSDK_DIR@</CMAKE_EMSCRIPTEN_EMSDK_DIR>
	<CMAKE_SWIG_EXECUTABLE>@SWIG_EXECUTABLE@</CMAKE_SWIG_EXECUTABLE>
	<CMAKE_SWIG_LIB_DIRECTORY>@SWIG_LIB_DIRECTORY@</CMAKE_SWIG_LIB_DIRECTORY>
    <CMAKE_PYTHON>@Python3_EXECUTABLE@</CMAKE_PYTHON>
  </PropertyGroup>
  <PropertyGroup />
  <ItemDefinitionGroup />
  <ItemGroup />
</Project>