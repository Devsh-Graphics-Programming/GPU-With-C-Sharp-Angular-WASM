# https://stackoverflow.com/questions/2074144/generate-c-sharp-project-using-cmake
project(AngularBlazorProject VERSION 0.1.0 LANGUAGES CSharp)

# add_executable(AngularBlazorProject
# App.config
# App.xaml
# App.xaml.cs
# MainWindow.xaml
# MainWindow.xaml.cs
# Properties/AssemblyInfo.cs
# Properties/Resources.Designer.cs
# Properties/Resources.resx
# Properties/Settings.Designer.cs
# Properties/Settings.settings)

set_property(TARGET AngularBlazorProject PROPERTY VS_DOTNET_REFERENCES
"Microsoft.CSharp"
"PresentationCore"
"PresentationFramework"
"System"
"System.Core"
"System.Data"
"System.Data.DataSetExtensions"
"System.Net.Http"
"System.Xaml"
"System.Xml"
"System.Xml.Linq"
"WindowsBase")