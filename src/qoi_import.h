#pragma once

#if defined(_MSC_VER)
#pragma warning(disable : 4244)
#endif

#include <godot_cpp/classes/image_format_loader_extension.hpp>

#if defined(_MSC_VER)
#pragma warning(default : 4244)
#endif

using namespace godot;

class QOIImport : public ImageFormatLoaderExtension {
	GDCLASS(QOIImport, ImageFormatLoaderExtension)

protected:
	static void _bind_methods(){};

public:
	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual Error _load_image(const Ref<Image> &image, const Ref<FileAccess> &fileaccess, BitField<ImageFormatLoader::LoaderFlags> flags, double scale) override;
};
