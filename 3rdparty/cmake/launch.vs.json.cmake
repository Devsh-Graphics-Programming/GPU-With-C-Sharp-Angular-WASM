{
  "version": "0.2.1",
  "defaults": {},
  "configurations": [
    {
      "type": "python",
      "interpreterArguments": "",
      "scriptArguments": "",
      "env": {},
      "project": "@EMRUN_PYTHON_SCRIPT_PATH@ $(BaseOutputPath) $(AssemblyName) True",
      "projectTarget": "CMakeLists.txt",
      "name": "Launch on localhost and open browser",
      "noDebug": true
    },
    {
      "type": "python",
      "interpreterArguments": "",
      "scriptArguments": "",
      "env": {},
      "project": "@EMRUN_PYTHON_SCRIPT_PATH@ $(BaseOutputPath) $(AssemblyName) False",
      "projectTarget": "CMakeLists.txt",
      "name": "Launch on localhost (no browser)",
      "noDebug": true
    }
  ]
}