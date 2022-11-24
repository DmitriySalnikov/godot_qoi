#pragma once

#if defined(_MSC_VER)
#pragma warning(disable : 4244)
#endif

#include <godot_cpp/classes/resource_format_saver.hpp>
#include <godot_cpp/classes/resource_saver.hpp>

#if defined(_MSC_VER)
#pragma warning(default : 4244)
#endif

using namespace godot;

class QOIResourceSaver : public ResourceFormatSaver {
	GDCLASS(QOIResourceSaver, ResourceFormatSaver)

protected:
	static void _bind_methods(){};

public:
	virtual int64_t _save(const Ref<Resource> &resource, const String &path, int64_t flags) override;
	virtual bool _recognize(const Ref<Resource> &resource) const override;
	virtual PackedStringArray _get_recognized_extensions(const Ref<Resource> &resource) const override;
	virtual bool _recognize_path(const Ref<Resource> &resource, const String &path) const override;
};
