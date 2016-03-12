
#ifdef _WIN32
#include <Windows.h>
#include <stdlib.h>

namespace
{
	struct win32_state
	{
		HMODULE module;
	};

	void* load_internal(const char* libname)
	{
		win32_state* st = (win32_state*)malloc(sizeof(win32_state));
		st->module = LoadLibraryA(libname);
		return st;
	}
	void terminate_internal(void* st)
	{
		FreeLibrary(((win32_state*)st)->module);
		free(st);
	}

	void* load_internal(void* st, const char* funcname)
	{
		return (void*)GetProcAddress(((win32_state*)st)->module, funcname);
	}
}
#else
#error "this platform currently not supported"
#endif

void* load_init(const char* libname)
{
	return load_internal(libname);
}
void* load_func(void* state, const char* funcname)
{
	return load_internal(state, funcname);
}
void load_terminate(void* state)
{
	terminate_internal(state);
}
