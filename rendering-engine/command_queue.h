#pragma once

#include "drawstate.h"

#include <vector>

namespace gldr
{
	struct draw_command
	{

	};

	class command_queue
	{
	private:
		drawstate state;
		std::vector<draw_command> commands;

	public:

	};
}
