#include "qoi_save.h"
#include "qoi_wrapper.h"

#include <godot_cpp/classes/texture2d.hpp>

int64_t QOIResourceSaver::_save(const Ref<Resource> &resource, const String &path, int64_t flags) {
	{
		Ref<Image> img = resource;
		if (img.is_valid())
			return QOI::write(path, img);
	}

	{
		Ref<Texture2D> tex = resource;
		if (tex.is_valid())
			if (tex->get_image().is_valid())
				return QOI::write(path, tex->get_image());
	}

	ERR_FAIL_V_MSG(ERR_INVALID_DATA, "Image cannot be null");
}

bool QOIResourceSaver::_recognize(const Ref<Resource> &resource) const {
	{
		Ref<Image> img = resource;
		if (img.is_valid())
			return true;
	}

	{
		Ref<Texture2D> tex = resource;
		if (tex.is_valid())
			if (tex->get_image().is_valid())
				return true;
	}

	return false;
}

PackedStringArray QOIResourceSaver::_get_recognized_extensions(const Ref<Resource> &resource) const {
	return PackedStringArray(TypedArray<String>::make("qoi"));
}

bool QOIResourceSaver::_recognize_path(const Ref<Resource> &resource, const String &path) const {
	return path.get_extension().nocasecmp_to("qoi") == 0;
}