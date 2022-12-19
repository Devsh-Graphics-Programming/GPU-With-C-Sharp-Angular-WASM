import os
import re
import subprocess
import webbrowser
from sys import argv

def launchEmrun(htmlRequestFilePath, openBrowser):
	serveRootPath = '@SERVE_ROOT_PATH@'
	urlToHtmlFile = os.path.relpath(htmlRequestFilePath, serveRootPath)
    print(f"launching http://localhost:8000/{urlToHtmlFile}\n")
	
	command = f"\"@EMRUN_PATH@\" {urlToHtmlFile} --serve_root \"{serveRootPath}\" --port 8000 --kill_exit"
    subprocess.run(command)
	
	if openBrowser:
		webbrowser.open("http://localhost:8000/{urlToHtmlFile}")
    
if __name__ == "__main__":     
    baseOutputPath = argv[1]
    assemblyName = argv[2]
	openBrowser = argv[3]
	
    launchEmrun(f"{baseOutputPath}/{assemblyName}.html", openBrowser)