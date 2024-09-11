#pragma once

#include "compiler.h"
#include "qoi_shared.h"

using namespace godot;

class QOI : public RefCounted {
	GDCLASS(QOI, RefCounted)

	friend class QOIImport;

private:
	static Error decode_to_image(const PackedByteArray &data, const Ref<Image> &out_image);

protected:
	static void _bind_methods();

public:
	static String _get_version();
	static Ref<Image> read(String path);
	static Ref<Image> decode(const PackedByteArray &data);
	static Error write(String path, Ref<Image> img);
	static PackedByteArray encode(Ref<Image> img);
};
