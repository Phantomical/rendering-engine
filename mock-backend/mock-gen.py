from __future__ import print_function
import xml.etree.ElementTree as ET
import sys

file = sys.argv[1]
outfile = sys.argv[2]
outfile2 = sys.argv[3]
namespace = "gldr"

tree = ET.parse(file)
root = tree.getroot()

output = ""
level = 0

class func_t:
    def __init__(self, name, rettype, args, nargs):
        self.name = name
        self.rettype = rettype
        self.args = args
        self.nargs = nargs

def getlevel():
    global level
    tabs = ""
    for i in range(0, level):
        tabs +="\t"
    return tabs

def write(val):
    global output
    output += getlevel() + val + "\n"

write("#include \"interface.h\"")
write("")
write("#define CALL_CONV " + namespace.upper() + "_BACKEND_CALL_CONV")
write("#ifdef _WIN32")
write("#define EXPORT __declspec(dllexport)")
write("#else")
write("#define EXPORT")
write("#endif")
write("")
write("using namespace " + namespace + ";")
write("")

#create list of functions
funcs = list()

for func in root.findall("function"):
    if func.find("no-implement") != None:
        #Don't implement the function if we aren't supposed to
        continue
    name = func.get("name")
    if name == None:
        print("Invalid function declaration detected")
        continue
    ret = func.find("return")
    if ret == None:
        print("No return type provided for the function")
        continue
    rettype = ret.get("type")
    if rettype == None:
        print("Invalid return declaration detected")
        continue
    cond = False
    args = ""
    cnt = 0
    for arg in func.findall("arg"):
       type = arg.get("type")
       if type == None:
           print("Invalid argument declaration in function " + name)
           continue
       if cond:
          args += ", "
       else:
           cond = True
       args += type + " arg" + str(cnt)
       cnt += 1
    funcs.append(func_t(name, rettype, args, cnt))

for func in funcs:
    write("__declspec(dllexport) size_t " + func.name + "_called = 0;")

write("")

for func in funcs:
    write("extern \"C\" " + func.rettype + " EXPORT CALL_CONV _" + func.name + "(" + func.args + ")")
    write("{")
    level += 1
    write(func.name + "_called++;")
    write("return " + func.rettype + "();")
    level -= 1
    write("}")

write("extern \"C\" void EXPORT CALL_CONV _init() { }")
write("extern \"C\" void EXPORT CALL_CONV _terminate() { }")

write("")

with open(outfile, "w") as f:
    f.write(output)

output = ""

write("#ifndef MOCK_DATA_H")
write("#define MOCK_DATA_H")
write("")
write("#ifdef _MSC_VER")
write("#define MOCK_EXPORT __declspec(dllimport)")
write("#else")
write("#define MOCK_EXPORT")
write("#endif")
write("")
for func in funcs:
    write("MOCK_EXPORT extern size_t " + func.name + "_called;")
write("")
write("#endif")

with open(outfile2, "w") as f:
    f.write(output);

