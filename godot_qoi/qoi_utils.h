#pragma once

#include <Godot.hpp>
#include <Reference.hpp>

using namespace godot;

class QOIUtils : public Reference {
	GODOT_CLASS(QOIUtils, Reference)

private:
	void _fake_set_prop_pba(PoolByteArray str){};
	PoolByteArray _get_qoi_cfg_magic() { return get_qoi_magic(); };

	void _fake_set_prop_str(String str){};
	String _get_import_flags() { return QOI_IMPORT_FLAGS; };
	String _get_import_size_limits() { return QOI_IMPORT_SIZE_LIMIT; };
	String _get_import_fix_alpha() { return QOI_IMPORT_FIX_ALPHA_BORDER; };
	String _get_import_premult_alpha() { return QOI_IMPORT_PREMULT_ALPHA; };
	String _get_import_invert_colors() { return QOI_IMPORT_INVERT_COLORS; };
	String _get_import_normal_map_invert_y() { return QOI_IMPORT_NORMAL_MAP_INVERT_Y; };

public:
	static const char qoi_cfg_magic[4];

	static const char *QOI_IMPORT_FLAGS;
	static const char *QOI_IMPORT_SIZE_LIMIT;
	static const char *QOI_IMPORT_FIX_ALPHA_BORDER;
	static const char *QOI_IMPORT_PREMULT_ALPHA;
	static const char *QOI_IMPORT_INVERT_COLORS;
	static const char *QOI_IMPORT_NORMAL_MAP_INVERT_Y;

	static void _register_methods();
	void _init();

	static PoolByteArray get_qoi_magic();

	int add_footer(String target_path, Dictionary options);
};
