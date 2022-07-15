#pragma once

#include <Godot.hpp>

#define PRINT_ERROR(text) Godot::print_error(text, __FUNCTION__, get_file_name_in_repository(__FILE__), __LINE__)
#define PRINT_WARNING(text) Godot::print_warning(text, __FUNCTION__, get_file_name_in_repository(__FILE__), __LINE__)

namespace godot {
static String get_file_name_in_repository(String name) {
	if (name != "") {
		int idx = name.find("godot_qoi");
		if (idx != -1)
			name = name.substr(name.find("godot_qoi"), name.length());
	}
	return name;
}
} // namespace godot