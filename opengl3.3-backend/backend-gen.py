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
    def __init__(self, name, rettype, args, nargs, largs):
        self.name = name
        self.rettype = rettype
        self.args = args
        self.nargs = nargs
        self.largs = largs

def getlevel():
    global level
    tabs = ""
    for i in range(0, level):
        tabs +="\t"
    return tabs

def write(val = ""):
    global output
    output += getlevel() + val + "\n"

write("#ifndef OPENGL_3_3_BACKEND_IMPLEMENTATION_H")
write("#define OPENGL_3_3_BACKEND_IMPLEMENTATION_H")
write("")
write('#include "interface.h"')
write('#include "sema.h"')
write()
write('#define CALL_CONV ' + namespace.upper() + '_BACKEND_CALL_CONV')
write('#ifdef _WIN32')
write('#define EXPORT __declspec(dllexport)')
write('#else')
write('#define EXPORT')
write('#endif')
write('')
write('typedef DefaultSemaphoreType semaphore;')
write("")
write('using namespace ' + namespace.lower() + ';')
write

#create list of functions
funcs = list()
handles = list()

for handle in root.findall("handle"):
    type = handle.get("type")
    if type == None:
        print("Invalid handle declaration detected")
        continue
    handles.append(type)

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
    largs = list()
    for arg in func.findall("arg"):
       type = arg.get("type")
       if type == None:
           print("Invalid argument declaration in function " + name)
           continue
       if cond:
          args += ", "
       else:
           cond = True
       args += type + ' ' + arg.get("name", "arg" + str(cnt))
       largs.append([type, arg.get("name", "arg" + str(cnt))])
       cnt += 1
    funcs.append(func_t(name, rettype, args, cnt, largs))

write('namespace gl_3_3_backend')
write('{')
level += 1
write('struct state;')
write('typedef void(*command_func)(state& state, void* data);')

write('namespace enums')
write('{')
level += 1

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

level -= 1
write('}')

write('namespace commands')
write('{')
level += 1

for func in funcs:
    if func.name == "sync" or func.name == "swap_buffers":
        write('struct ' + func.name)
        write('{')
        level += 1
        write('semaphore* sema;')
        level -= 1
        write('};')
        continue
    write('struct ' + func.name)
    write('{')
    level += 1
    for arg in func.largs:
        write(arg[0] + ' ' + arg[1] + ';')
    if func.rettype in handles:
        write("detail::handle _handle;")
    elif func.rettype != "void":
        write(func.rettype + " _ret;")
    level -= 1
    write('};')

level -= 1
write('}')

write('struct command')
write('{')
level += 1
write('command_func func;')
write('union')
write('{')
level += 1
#this is so the dispatcher function can easily get a pointer to the data
write('char ptr[1];')
write()

for func in funcs:
    write('commands::' + func.name + ' ' + func.name + ';')

level -= 1;
write('};')

write('command() { }')
write('command(command_func func) : func(func) { }')
level -= 1;
write('};')

for handle in handles:
    write('detail::handle alloc_' + handle + '();')
write('void enqueue(const command&);')

for func in funcs:
    write('void ' + func.name + '(state&, void*);')

level -= 1
write('}')

write('using namespace gl_3_3_backend;')
write('using detail::handle;')

for func in funcs:

    if func.rettype != "void":
        args = ""
        if func.nargs != 0:
            args = ", " + func.args
        write('extern "C" void _' + func.name + '(' + func.rettype + "* _retval" + args + ')')
    else:
        write('extern "C" void _' + func.name + '(' + func.args + ')')
    write('{')
    level += 1

    if func.rettype in handles:
        write('handle _handle = alloc_' + func.rettype + '();')
    write('command cmd(' + func.name + ');')

    for arg in func.largs:
        write('cmd.' + func.name + '.' + arg[1] + ' = ' + arg[1] + ';')

    if func.rettype in handles:
        write("cmd." + func.name + "._handle = _handle;")
        write("_retval->handle = _handle;")

    if func.name == "sync" or func.name == "swap_buffers":
        write("semaphore sema;")
        write("cmd." + func.name + ".sema = &sema;")
    write('enqueue(cmd);')
    if func.name == "sync" or func.name == "swap_buffers":
        write("sema.wait();")
    level -= 1
    write('}')

write()
write("#endif")

with open(outfile, "w") as f:
    f.write(output)

