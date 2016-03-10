from __future__ import print_function
import xml.etree.ElementTree as ET
import sys

file = sys.argv[1]
outfile = sys.argv[2]
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

write("#include \"allocators.h\"")
write("#include \"interface.h\"")
write("")
write("#ifdef _WIN32")
write("#\tdefine CALL_CONV __cdecl")
write("#endif")
write("")
write("void* load_init(const char*);")
write("void* load_func(void*, const char*);")
write("void* load_terminate(void*);")
write("")
write("namespace " + namespace)
write("{")
level += 1

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

#declare typedefs
for func in funcs:
    write("typedef " + func.rettype + "(CALL_CONV*" + func.name + "_proc)(" + func.args + ");")

write("")

#declare variables
for func in funcs:
    write(func.name + "_proc " + func.name +"_func;")

write("")

#declare prototypes
for func in funcs:
    write(func.rettype + " " + func.name + "(" + func.args + ")")
    write("{")
    level += 1
    cond = False
    decl = ""
    for i in range(func.nargs):
        if cond:
            decl += ", "
        else:
            cond = True
        decl += "arg" + str(i)
    write("return " + func.name + "_func(" + decl + ");")
    level -= 1
    write("}")

write("")

#Backend terminate function, we don't store the init function
write("void(*terminate_func)();")
write("void* handle;");

#init method
write("void init(const std::string& lib)")
write("{")
level += 1

write("handle = load_init(lib.c_str());")

for func in funcs:
    write(func.name + "_func = static_cast<decltype(" + func.name 
          + "_func)>(load_func(handle, \"" + func.name + "\"));")

write("terminate_func = static_cast<decltype(terminate_func)>(load_func(handle, \"terminate\"));")
write("static_cast<void(*)()>(load_func(handle, \"init\"))();")

level -= 1
write("}")

write("void terminate()")
write("{")
level += 1
write("terminate_func();")
write("load_terminate(handle);")
level -= 1
write("}")


level -= 1
write("}")
    
with open(outfile, "w") as f:
    f.write(output)
