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
#include "allocators.h"

using namespace RE_NAMESPACE;

TEST_CASE("test that allocators::linear_atomic correctly allocates memory")
{
	//Create allocator with a buffer of 512 bytes
	allocators::linear_atomic alloc{ 512 };

	SECTION("test that the allocator returns a valid pointer for a allocation with enough size")
	{
		void* mem = alloc.alloc(128);
		REQUIRE(mem != nullptr);
	}
	SECTION("the allocator should return a null pointer for an allocation that is too large")
	{
		void* mem = alloc.alloc(1024);
		REQUIRE(mem == nullptr);
	}
}

TEST_CASE("test that allocators::linear correctly allocates memory")
{
	//Create allocator with a buffer of 512 bytes
	allocators::linear alloc{ 512 };

	SECTION("test that the allocator returns a valid pointer for a allocation with enough size")
	{
		void* mem = alloc.alloc(128);
		REQUIRE(mem != nullptr);
	}
	SECTION("the allocator should return a null pointer for an allocation that is too large")
	{
		void* mem = alloc.alloc(1024);
		REQUIRE(mem == nullptr);
	}
}

TEST_CASE("test that allocators::standard correctly allocates memory")
{
	allocators::standard alloc;

	SECTION("the allocator should return a valid pointer with a valid size")
	{
		void* mem = alloc.alloc(128);
		REQUIRE(mem != nullptr);
		alloc.dealloc(mem);
	}
	SECTION("the allocator should return a null pointer for an allocation with too large of a size")
	{
		//Do an allocation with the maximum possible size
		//No allocator should have enough space for this to work
		void* mem = alloc.alloc(SIZE_MAX);
		REQUIRE(mem == nullptr);
	}
}
