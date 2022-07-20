   
import os
import subprocess

file_dir = "EmscriptenGLFW/build/WASM-Debug/EmscriptenGLFW.html"
if __name__ == "__main__":     
    cwd_up = os.getcwd() + "/../" 
    os.chdir(cwd_up)
    print(os.getcwd())
    bashCommand = "where emrun"
    where_process = subprocess.run(bashCommand, shell=True, capture_output=True)
    emrun_dir = where_process.stdout.decode().splitlines()[-1] #if error occurs here, it means emrun is not added to path
    print("emrun located at: "+emrun_dir)
    print("launching " + file_dir)

    emrun_process = subprocess.call(emrun_dir +" --no_browser "+ file_dir + " --serve_root "+ os.getcwd() + " --port 8000", shell=True)
    emrun_process = subprocess.call("C:\Program Files\Google\Chrome\Application\chrome.exe --no-sandbox --disable-gpu-watchdog --gpu-startup-dialog http://localhost:8000/"+file_dir, shell=True)
