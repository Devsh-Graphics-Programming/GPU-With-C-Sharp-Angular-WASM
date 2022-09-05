   
import os
import re
import subprocess
from sys import argv

def launch_emrun(filepath):
    print("launching http://localhost:8000/" + filepath + "\n")
    subprocess.run(emrun_dir + filepath + " --serve_root \""+ serve_root + "\" --port 8000 --kill_exit")


if __name__ == "__main__":     
    serve_root = os.getcwd() 
    bashCommand = "where emrun"
    where_process = subprocess.run(bashCommand, shell=True, capture_output=True)
    emrun_dir = where_process.stdout.decode().splitlines()[-1] +  " " #if error occurs here, it means emrun is not added to path
    print("emrun located at: "+emrun_dir)

    for i in range(1, len(argv)):
        folder = re.search(r"\w+", argv[i]).group()
        if folder == "EmscriptenGLFW":
            configuration = "Release" if (i+1)<len(argv) and argv[i+1] == "R" else "Debug" 
            launch_emrun(f"EmscriptenGLFW/build/WASM-{configuration}/EmscriptenGLFW.html")
        elif folder == "EmscriptenHTML5":
            configuration = "Release" if (i+1)<len(argv) and argv[i+1] == "R" else "Debug" 
            launch_emrun(f"EmscriptenHTML5/build/WASM-{configuration}/EmscriptenHTML5.html")
        elif folder == "EmscriptenHTML5SeparateLibs":
            configuration = "Release" if (i+1)<len(argv) and argv[i+1] == "R" else "Debug" 
            launch_emrun(f"EmscriptenHTML5SeparateLibs/build/WASM-{configuration}/EmscriptenHTML5.html")
        elif folder == "BlazorEmscripten":
            print("Cannot run BlazorEmscripten with run.py. Use Visual Studio instead.")
        elif folder == "Native":
            print("Cannot run Native C++ with run.py. Use Visual Studio instead")
        elif folder == "NativeCSharp":
            print("Cannot run Native C# with run.py. Use Visual Studio instead")

    