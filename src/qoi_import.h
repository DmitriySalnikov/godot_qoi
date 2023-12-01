#pragma once

#include "compiler.h"

GODOT_WARNING_DISABLE()
#include <godot_cpp/classes/image_format_loader_extension.hpp>
GODOT_WARNING_RESTORE()
using namespace godot;

class QOIImport : public ImageFormatLoaderExtension {
	GDCLASS(QOIImport, ImageFormatLoaderExtension)

protected:
	static void _bind_methods(){};

public:
	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual Error _load_image(const Ref<Image> &image, const Ref<FileAccess> &fileaccess, BitField<ImageFormatLoader::LoaderFlags> flags, double scale) override;
};
