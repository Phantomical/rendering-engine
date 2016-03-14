#include "interface.h"
#include "allocators.h"

namespace gldr
{
	void backend::sync_callback()
	{
		//Reset the memory pool because there shouldn't be anyone adding commands
		allocator()->reset();
	}

	shader_handle backend::create_shader(const std::initializer_list<std::pair<shader_stage, const char*>>& stages)
	{
		auto alloc = allocator();

		auto array = (std::pair<shader_stage, const char*>*)alloc->alloc(sizeof(std::pair<shader_stage, const char*>) * stages.size());
		for (size_t i = 0; i < stages.size(); ++i)
		{
			char* buf = nullptr;
			if (stages.begin()[i].second != nullptr)
			{
				size_t sz = std::char_traits<char>::length(stages.begin()[i].second) + 1;
				buf = (char*)alloc->alloc(sz);
				memcpy(buf, stages.begin()[i].second, sz - 1);
				buf[sz - 1] = 0;
			}

			new(&array[i]) std::pair<shader_stage, const char*>(stages.begin()[i].first, buf);
		}

		return create_shader(stages.size(), array);
	}
	texture_handle backend::create_texture_cubemap(uint16_t width, uint16_t height, internal_format iformat, image_format format, data_type type, const std::array<const void*, 6>& data)
	{
		auto buf = (const void**)allocator()->alloc(sizeof(const void*) * 6);
		memcpy(buf, data.data(), sizeof(const void*) * 6);

		return create_texture_cubemap(width, height, iformat, format, type, buf);
	}
}
