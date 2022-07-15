#pragma once

#include "qoi_shared.h"
#include "qoi_wrapper.h"

#include <Godot.hpp>
#include <ImageTexture.hpp>
#include <Reference.hpp>

using namespace godot;

class QOIUtils : public Reference {
	GODOT_CLASS(QOIUtils, Reference)

private:
	Ref<QOI> qoi_wrapper;

public:
	static const char qoi_cfg_magic[4];

	static const char *QOI_IMPORT_ERROR;
	static const char *QOI_IMPORT_FLAGS;
	static const char *QOI_IMPORT_SIZE_LIMIT;
	static const char *QOI_IMPORT_FIX_ALPHA_BORDER;
	static const char *QOI_IMPORT_PREMULT_ALPHA;
	static const char *QOI_IMPORT_INVERT_COLORS;
	static const char *QOI_IMPORT_NORMAL_MAP_INVERT_Y;

	static void _register_methods();
	void _init();

	static PoolByteArray get_qoi_magic();

	int add_footer(String target_path, Dictionary options, bool update = true);
	Dictionary read_footer(String target_path);
	void update_dictionary(Dictionary &data, const Dictionary updates);

	// If it were possible, the resource loading and saving classes would also be written in C++, but this is not possible...
	// Therefore, only the main part of their logic is written in C++ and is used by GDScript scripts.
	int save_resource(const String path, const Ref<Resource> resource, const int64_t flags);
	Ref<ImageTexture> load_resource(const String path, const String original_path);
};
