#pragma once

#if defined(_MSC_VER)
#pragma warning(disable : 4244)
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
#pragma warning(default : 4244)
#endif

#define TEXT(t) #t
#define PRINT(text) godot::UtilityFunctions::print(godot::Variant(text))
