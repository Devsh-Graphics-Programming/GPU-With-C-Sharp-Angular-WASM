{
  "version": "0.2.1",
  "defaults": {},
  "configurations": [
    {
      "type": "default",
      "project": "@Python3_EXECUTABLE@",
      "name": "Launch on localhost and open browser",
	  "args": 
	  [
        "@EMRUN_PYTHON_SCRIPT_PATH@",
        "${env.ENV_HTML_FILE}",
		"True"
      ]
    },
    {
      "type": "default",
      "project": "@Python3_EXECUTABLE@",
      "name": "Launch on localhost (no browser)",
	  "args": 
	  [
        "@EMRUN_PYTHON_SCRIPT_PATH@",
        "${env.ENV_HTML_FILE}",
		"False"
      ]
    }
  ]
}