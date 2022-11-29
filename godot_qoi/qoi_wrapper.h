#pragma once

#include "qoi_shared.h"

using namespace godot;

class QOI : public RefCounted {
	GDCLASS(QOI, RefCounted)

	friend class QOIImport;

private:
	static int decode_to_image(const PackedByteArray &data, const Ref<Image> &out_image);

protected:
	static void _bind_methods();

public:
	static Ref<Image> read(String path);
	static Ref<Image> decode(const PackedByteArray &data);
	static int write(String path, Ref<Image> img);
	static PackedByteArray encode(Ref<Image> img);
};
