#pragma once

#ifdef DEBUG_ENABLED

#if defined(_MSC_VER)
#pragma warning(disable : 4244)
#endif

#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

#if defined(_MSC_VER)
#pragma warning(default : 4244)
#endif

using namespace godot;

class AssetLibraryUpdateChecker : public RefCounted {
	GDCLASS(AssetLibraryUpdateChecker, RefCounted)

private:
	HTTPRequest *request = nullptr;
	String addon_name;
	String repository_name;
	String root_settings_section;

protected:
	static void _bind_methods();

public:
	void request_completed(int result, int response_code, PackedStringArray headers, PackedByteArray body);
	void init();

	AssetLibraryUpdateChecker();
};

#endif
