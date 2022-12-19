{
  "version": "0.2.1",
  "defaults": {},
  "configurations": [
    {
      "type": "python",
      "project": "@EMRUN_PYTHON_SCRIPT_PATH@ $(BaseOutputPath) $(AssemblyName) True",
      "name": "Launch on localhost and open browser"
    },
    {
      "type": "python",
      "project": "@EMRUN_PYTHON_SCRIPT_PATH@ $(BaseOutputPath) $(AssemblyName) False",
      "name": "Launch on localhost (no browser)"
    }
  ]
}