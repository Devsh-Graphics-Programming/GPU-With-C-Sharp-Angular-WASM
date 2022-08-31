import os

cwd = os.path.dirname(__file__)
os.chdir(cwd)
f = open("LibRendererPINVOKE.cs", "r")
lines = f.readlines()
f.close()

try: 
    if (lines[13].find("SWIGExceptionHelper") is not -1 ):
        for x in range(13, 189):
            lines[x]=  ""
        pass
except Exception as ex:
    print(ex)

f = open("LibRendererPINVOKE.cs", "w")
f.writelines(lines)
f.close()