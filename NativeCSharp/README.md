#  C# Native Sample

## Motivation

Before embarking on attempting to make a hybrid C++ library used by a C# application in Blazor, we wanted to see how a Native application coded only in C# would look like.

This would serve as a useful benchmark to evaluate:
1. the intrusiveness and added complexity of using a C++ library in the middle of a C# project
2. any unnatural behaviour from the Mono-WASM runtime or Blazor [since what features are missing is largely undocumented](https://github.com/dotnet/aspnetcore/issues/42850)

To allow for fair comparisons, this Native C# example uses GLFW3 and GL ES bindings installed via the NuGet package manager.
```xml
    <PackageReference Include="Arqan.Windows" Version="2.2.2" />
    <PackageReference Include="System.Drawing.Common" Version="6.0.0" />
```

We made sure that the C# code follow the Native C++ in structure.

## Building

Open the the provided .sln file and build.

## Debugging

Use the VS2022 Debugger by pressing `F5`.
