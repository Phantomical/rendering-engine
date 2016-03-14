#Copyright (C) 2016 Sean Lynch

#	This file is part of rendering-engine.
#	
#	rendering-engine is free software : you can redistribute it and / or modify
#	it under the terms of the GNU Lesser General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#	
#	rendering-engine is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
#	GNU Lesser General Public License for more details.
#	
#	You should have received a copy of the GNU Lesser General Public License
#	along with rendering-engine. If not, see <http://www.gnu.org/licenses/>.

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

def getlevel():
    global level
    tabs = ""
    for i in range(0, level):
        tabs +="\t"
    return tabs

def write(val):
    global output
    output += getlevel() + val + "\n"

write("""/*
    This file was generated by interface-gen.py as part of the project build.

	This file is part of rendering-engine.
	
	rendering-engine is free software : you can redistribute it and / or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	rendering-engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with rendering-engine. If not, see <http://www.gnu.org/licenses/>.
*/
""")

write("#ifndef RE_INTERFACE_H")
write("#define RE_INTERFACE_H")
write("")

for include in root.findall("include"):
    write("#include \"" + include.text + "\"")
write("#include \"allocators.h\"")

for libinc in root.findall("lib-include"):
    write("#include <" + libinc.text + ">")

write("")
write("#define " + namespace.upper() + "_DECLARE_HANDLE(name) \\")
level += 2
write("struct name { \\")
level += 1
write("detail::handle handle; \\")
write("name() = default; \\")
write("name(const detail::handle& h) : handle(h) { } \\")
write("bool operator ==(const name& o) { return handle == o.handle; } \\")
write("bool operator !=(const name& o) { return handle != o.handle; } \\")
level -= 1
write("}")
level -= 2
#On windows we need to specify the calling convention of the backend
#so we don't have the backend using a different calling convention
write("//This is the calling convention that MUST be used when creating renderer backends")
write("#ifdef _WIN32")
write("#\tdefine " + namespace.upper() + "_BACKEND_CALL_CONV __cdecl")
write("#else")
write("#\tdefine " + namespace.upper() + "_BACKEND_CALL_CONV")
write("#endif")
write("")
write("namespace " + namespace)
write("{")
level += 1

for handle in root.findall("handle"):
    type = handle.get("type")
    if type != None:
        write(namespace.upper() + "_DECLARE_HANDLE(" + type + ");")
    else:
        print("Invalid handle declaration detected\n")

write("")

for enum in root.findall("enum"):
    name = enum.get("name")
    if name == None:
        print("Invalid enum declaration detected")
    else:
        write("enum " + name + ' : uint8_t')
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

for struct in root.findall("struct"):
    name = struct.get("name")
    if name == None:
        print("Invalid struct declaration detected")
        continue
    if struct.find("no-def") != None:
        write("struct " + name + ";")
        continue
    write("struct " + name)
    write("{")
    level += 1
    for member in struct.findall("member"):
        type = member.get("type")
        memname = member.get("name")
        if type == None or memname == None:
            print("Invalid member declaration detected in " + name)
            continue
        write(type + " " + memname + ";")
    level -= 1
    write("};")

write("")

write("class backend")
write("{")
write("private:")
level += 1
write("struct state;")
write("state* _state;")

write("void init(const std::string& backend_lib);")
write("void terminate();")
write("void sync_callback();")
write("allocators::linear_atomic* allocator();")

write("")
level -= 1
write("public:")
level += 1
write("backend(const std::string& backend_lib);")
write("~backend();")
write("backend(const backend&) = delete;")
write("void operator =(const backend&) = delete;")
write("")
write("bool is_valid() const;")
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
        default = arg.get("default")
        ref = arg.get("ref", "false")
        if type == None:
            print("Invalid argument declaration in function " + name)
            continue
        if cond:
           decl += ", "
        else:
            cond = True
        if default == None:
            decl += type + " " + name
        else:
            decl += type + " " + name + " = " + default
    write(decl + ");")

level -= 1
write("};")

level -= 1
write("}")
write("")
write("#endif")

with open(outfile, "w") as out:
    out.write(output)

