//Copyright (C) 2016 Sean Lynch
/*
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

#include "catch.hpp"

//Imported functions from loader.cpp in render-engine
void* load_init(const char*);
void* load_func(void*, const char*);
void load_terminate(void*);

TEST_CASE("functions can be loaded through the loader.cpp functions")
{
	void* load_state = nullptr;
#ifdef _WIN32
	//opengl32.dll should be present on every windows machine
	load_state = load_init("opengl32.dll");
#else
	SUCCEED("This test not supported on other platforms than windows yet");
	return;
#endif

	//The function must have state, it should not use global state
	REQUIRE((load_state != nullptr));

	SECTION("able to load a known function from a library")
	{
		//Load an OpenGL 1.1 function that should be present on every windows distribution (XP and up)
		void* func = load_func(load_state, "glClear");

		REQUIRE((func != nullptr));
	}

	load_terminate(load_state);
}
