#include "interface.h"
#include "allocators.h"

namespace gldr
{
	void backend::sync_callback()
	{
		//Reset the memory pool because there shouldn't be anyone adding commands
		allocator()->reset();
	}

	shader_handle backend::create_shader(const std::initializer_list<std::pair<shader_stage, const char*>>& stages, const std::initializer_list<const char*>& tf_varyings)
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
		char** varyings = nullptr;
		size_t nvars = 0;
		if (tf_varyings.size() != 0)
		{
			varyings = (char**)alloc->alloc(sizeof(char*) * tf_varyings.size());

			for (const char* str : tf_varyings)
			{
				if (str != nullptr)
				{
					size_t sz = std::char_traits<char>::length(str) + 1;
					char* buf = (char*)alloc->alloc(sz);
					memcpy(buf, str, sz - 1);
					buf[sz - 1] = 0;
					varyings[nvars] = buf;
					++nvars;
				}				
			}
		}

		return create_shader(stages.size(), array, nvars, varyings);
	}
	texture_handle backend::create_texture_cubemap(uint16_t width, uint16_t height, internal_format iformat, image_format format, data_type type, const std::array<const void*, 6>& data)
	{
		auto buf = (const void**)allocator()->alloc(sizeof(const void*) * 6);
		memcpy(buf, data.data(), sizeof(const void*) * 6);

		return create_texture_cubemap(width, height, iformat, format, type, buf);
	}
}
