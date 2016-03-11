
#include "catch.hpp"
#include "mock-data.h"
#include "interface.h"

using namespace gldr;

#define TEST_BACKEND_METHOD(method, ...) \
	SECTION("testing that" #method "is called correctly when corresponding method is called in the backend") \
	{ bck.method(__VA_ARGS__); REQUIRE(method##_called != 0); }

TEST_CASE("backend methods are called correctly when the backend interface methods are called")
{
#if _WIN32
	backend bck{ "mock-backend.dll" };
#else
	backend bck{ "mock-backend.a" };
#endif

	REQUIRE(bck.is_valid());

	auto array = std::array<const void*, 6>{};
	for (auto& v : array)
		v = nullptr;

	TEST_BACKEND_METHOD(create_buffer, 0, nullptr, buffer_usage::STATIC_DRAW);
	TEST_BACKEND_METHOD(create_shader, 0, nullptr);
	TEST_BACKEND_METHOD(create_shader, 0, {});
	TEST_BACKEND_METHOD(create_texture_2d, 0, 0, internal_format::R8, image_format::RED, data_type::UNSIGNED_BYTE, nullptr);
	TEST_BACKEND_METHOD(create_texture_3d, 0, 0, 0, internal_format::R8, image_format::RED, data_type::UNSIGNED_BYTE, nullptr);
	TEST_BACKEND_METHOD(create_texture_cubemap, 0, 0, internal_format::R8, image_format::RED, data_type::UNSIGNED_BYTE, nullptr);
	TEST_BACKEND_METHOD(create_texture_cubemap, 0, 0, internal_format::R8, image_format::RED, data_type::UNSIGNED_BYTE, array)
	TEST_BACKEND_METHOD(delete_mesh, mesh_handle());
	TEST_BACKEND_METHOD(delete_buffer, buffer_handle());
	TEST_BACKEND_METHOD(delete_shader, shader_handle());
	TEST_BACKEND_METHOD(delete_texture, texture_handle());
	TEST_BACKEND_METHOD(delete_render_target, render_target_handle());
	TEST_BACKEND_METHOD(set_buffer_data, buffer_handle(), 0, nullptr);
	TEST_BACKEND_METHOD(sync);
	TEST_BACKEND_METHOD(swap_buffers);
}
