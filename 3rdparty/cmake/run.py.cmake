import os
import re
import subprocess
import webbrowser
from sys import argv

def launchEmrun(htmlRequestFilePath, openBrowser):
    serveRootPath = '@SERVE_ROOT_PATH@'
    urlToHtmlFile = str(os.path.relpath(htmlRequestFilePath, serveRootPath))
    browserFullUrl = 'http://localhost:8000/' + urlToHtmlFile
    os.chdir(str(serveRootPath))

    print("launching " + browserFullUrl)
    command = "\"@EMRUN_PATH@\" " + urlToHtmlFile + " --serve_root \"" + serveRootPath + "\" --port 8000 --kill_exit"
    subprocess.run(command)

    if openBrowser:
        webbrowser.open(browserFullUrl)

if __name__ == "__main__":
    htmlRequestFilePath = argv[1]

    launchEmrun(htmlRequestFilePath, argv[2])