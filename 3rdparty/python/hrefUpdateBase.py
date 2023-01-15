from sys import argv
import os
import re

requestedHTMLFile = str(argv[1])
requestedBaseHref = str(argv[2])

f = open(requestedHTMLFile, "r")
oldBuffer = str(f.read())
f.close()

regex = f"<base href=.* />"
pattern = re.compile(regex)
newBuffer = re.sub(pattern, "<base href=\"" + requestedBaseHref + "\" />", oldBuffer) # replace base href

if newBuffer != oldBuffer:
  f = open(requestedHTMLFile, "w")
  f.write(newBuffer)
  f.close()