#pragma once

#include "compiler.h"

GODOT_WARNING_DISABLE()
#include <godot_cpp/classes/resource_format_saver.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
GODOT_WARNING_RESTORE()
using namespace godot;

class QOIResourceSaver : public ResourceFormatSaver {
	GDCLASS(QOIResourceSaver, ResourceFormatSaver)

protected:
	static void _bind_methods(){};

public:
	virtual Error _save(const Ref<Resource> &resource, const String &path, uint32_t flags) override;
	virtual bool _recognize(const Ref<Resource> &resource) const override;
	virtual PackedStringArray _get_recognized_extensions(const Ref<Resource> &resource) const override;
	virtual bool _recognize_path(const Ref<Resource> &resource, const String &path) const override;
};
