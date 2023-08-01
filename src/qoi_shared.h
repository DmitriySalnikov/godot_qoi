#pragma once

#if defined(_MSC_VER)
#pragma warning(disable : 4244 4267)
#endif

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/builtin_types.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>

#if defined(_MSC_VER)
#pragma warning(default : 4244 4267)
#endif

#define NAMEOF(t) #t

#define REG_METHOD(name) ClassDB::bind_method(D_METHOD(#name), &REG_CLASS_NAME::name)
#define REG_METHOD_ARGS(name, ...) ClassDB::bind_method(D_METHOD(#name, __VA_ARGS__), &REG_CLASS_NAME::name)

#define PRINT(text) godot::UtilityFunctions::print(godot::Variant(text))
#define PRINT_ERROR(text) godot::_err_print_error(__FUNCTION__, godot::get_file_name_in_repository(__FILE__).utf8().get_data(), __LINE__, godot::Variant(text).stringify())
#define PRINT_WARNING(text) godot::_err_print_error(__FUNCTION__, godot::get_file_name_in_repository(__FILE__).utf8().get_data(), __LINE__, godot::Variant(text).stringify(), false, true)

namespace godot {
static String get_file_name_in_repository(const String &name) {
	if (name != "") {
		int64_t idx = name.find("src");
		if (idx != -1)
			return name.substr(idx, name.length());
	}
	return name;
}
} // namespace godot

#define IS_EDITOR_HINT() Engine::get_singleton()->is_editor_hint()
#define SCENE_TREE() Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop())
#define SCENE_ROOT() (SCENE_TREE()->get_root())

#define PS() ProjectSettings::get_singleton()
#define DEFINE_SETTING_AND_GET(var, path, def, type) \
	{                                                \
		if (!PS()->has_setting(path)) {              \
			PS()->set_setting(path, def);            \
		}                                            \
		Dictionary info;                             \
		info["name"] = path;                         \
		info["type"] = type;                         \
		PS()->add_property_info(info);               \
		PS()->set_initial_value(path, def);          \
	}                                                \
	var = PS()->get_setting(path)
#define DEFINE_SETTING_READ_ONLY(path, def, type)  \
	{                                              \
		PS()->set_setting(path, def);              \
		Dictionary info;                           \
		info["name"] = path;                       \
		info["type"] = type;                       \
		/* Does not work in the ProjectSettings */ \
		info["usage"] = PROPERTY_USAGE_READ_ONLY;  \
		PS()->add_property_info(info);             \
		PS()->set_initial_value(path, def);        \
	}
