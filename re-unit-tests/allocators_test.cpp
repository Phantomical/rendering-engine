
#include "catch.hpp"
#include "allocators.h"

using namespace gldr;

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
