from __future__ import print_function
import xml.etree.ElementTree as ET
import sys

file = sys.argv[1]
outfile = sys.argv[2]

tree = ET.parse(file)
root = tree.getroot()

output = ""
level = 0

def getlevel():
    global level
    tabs = ""
    for i in range(0, level):
        tabs +="\t"
    return tabs

def write(val):
    global output
    output += getlevel() + val + "\n"

write("#ifndef RE_INTERFACE_H")
write("#define RE_INTERFACE_H")
write("")

for include in root.findall("include"):
    write("#include \"" + include.text + "\"")

for libinc in root.findall("lib-include"):
    write("#include <" + libinc.text + ">")

write("")
write("#define GLDR_DECLARE_HANDLE(name) struct name { detail::handle handle; }")
write("")
write("namespace gldr")
write("{")
level += 1

for handle in root.findall("handle"):
    type = handle.get("type")
    if type != None:
        write("GLDR_DECLARE_HANDLE(" + type + ");")
    else:
        print("Invalid handle declaration detected\n")

write("")

for enum in root.findall("enum"):
    name = enum.get("name")
    if name == None:
        print("Invalid enum declaration detected")
    else:
        write("enum " + name)
        write("{")
        level += 1
        for value in enum.findall("value"):
            valname = value.get("name")
            val = value.get("value")
            if valname == None:
                print("Invalid value declaration detected in enum " + name + "\n")
            else:
                if val == None:
                    #No value was provided, this is fine
                    write(valname + ",")
                else:
                    write(valname + " = " + val + ",")
        level -= 1
        write("};")

write("")

for func in root.findall("function"):
    name = func.get("name")
    if name == None:
        print("Invalid function declaration detected")
        continue
    ret = func.find("return")
    if ret == None:
        print("Function declaration " + name + " has no return declaration")
        continue
    rettype = ret.get("type")
    if rettype == None:
        print("Return declaration for function " + name + " is invalid")
        continue
    decl = rettype + " " + name + "("
    cond = False
    for arg in func.findall("arg"):
       type = arg.get("type")
       name = arg.get("name", "")
       if type == None:
           print("Invalid argument declaration in function " + name)
           continue
       if cond:
          decl += ", "
       else:
           cond = True
       decl += type + " " + name
    write(decl + ");")

level -= 1
write("}")
write("")
write("#endif")

with open(outfile, "wb") as out:
    out.write(output)

