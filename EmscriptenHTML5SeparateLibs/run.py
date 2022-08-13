   
import os
import subprocess

file_dir = "EmscriptenHTML5/build/WASM-Debug/EmscriptenHTML.html"
if __name__ == "__main__":     
    cwd_up = os.getcwd() + "/../" 
    os.chdir(cwd_up)
    print(os.getcwd())
    bashCommand = "where emrun"
    where_process = subprocess.run(bashCommand, shell=True, capture_output=True)
    emrun_dir = where_process.stdout.decode().splitlines()[-1] #if error occurs here, it means emrun is not added to path
    print("emrun located at: "+emrun_dir)
    print("launching http://localhost:8000/" + file_dir)

    subprocess.run(emrun_dir +" "+ file_dir + " --no_browser --serve_root \""+ os.getcwd() + "\" --port 8000")
