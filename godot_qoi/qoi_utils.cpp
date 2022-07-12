#include "qoi_utils.h"

#include <File.hpp>

const char QOIUtils::qoi_cfg_magic[4] = { 0x71, 0x63, 0x66, 0x67 };

const char *QOIUtils::QOI_IMPORT_FLAGS = "flags";
const char *QOIUtils::QOI_IMPORT_SIZE_LIMIT = "process/size_limit";
const char *QOIUtils::QOI_IMPORT_FIX_ALPHA_BORDER = "process/fix_alpha_border";
const char *QOIUtils::QOI_IMPORT_PREMULT_ALPHA = "process/premult_alpha";
const char *QOIUtils::QOI_IMPORT_INVERT_COLORS = "process/invert_color";
const char *QOIUtils::QOI_IMPORT_NORMAL_MAP_INVERT_Y = "process/normal_map_invert_y";

void QOIUtils::_register_methods() {
	register_method("add_footer", &QOIUtils::add_footer);

	register_property("QOI_CFG_MAGIC", &QOIUtils::_fake_set_prop_pba, &QOIUtils::_get_qoi_cfg_magic, get_qoi_magic());

	register_property("QOI_IMPORT_FLAGS", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_flags, String(QOI_IMPORT_FLAGS));
	register_property("QOI_IMPORT_SIZE_LIMIT", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_size_limits, String(QOI_IMPORT_SIZE_LIMIT));
	register_property("QOI_IMPORT_FIX_ALPHA_BORDER", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_fix_alpha, String(QOI_IMPORT_FIX_ALPHA_BORDER));
	register_property("QOI_IMPORT_PREMULT_ALPHA", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_premult_alpha, String(QOI_IMPORT_PREMULT_ALPHA));
	register_property("QOI_IMPORT_INVERT_COLORS", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_invert_colors, String(QOI_IMPORT_INVERT_COLORS));
	register_property("QOI_IMPORT_NORMAL_MAP_INVERT_Y", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_normal_map_invert_y, String(QOI_IMPORT_NORMAL_MAP_INVERT_Y));
}

void QOIUtils::_init() {
}

PoolByteArray QOIUtils::get_qoi_magic() {
	static PoolByteArray magic;
	if (!magic.size()) {
		magic.resize(4);
		memcpy(magic.write().ptr(), qoi_cfg_magic, 4);
	}

	return magic;
};

int QOIUtils::add_footer(String target_path, Dictionary options) {
	Ref<File> file;
	file.instance();
	Error err = file->open(target_path, File::READ_WRITE);
	if ((int)err) {
		Godot::print_error("Can't open QOI file in .import for editing", __FUNCTION__, __FILE__, __LINE__);
		return (int)err;
	}

	// footer:
	// qcfg - magic
	// -- data --
	// 8-bit the size of the cfg data in bytes, including these 8 bits
	// 8-bit flags
	// -- data --
	// 64-bit pos of magic

	file->seek_end();
	int64_t pos = file->get_position();
	file->store_buffer(get_qoi_magic());
	file->store_8(1 + 1);
	file->store_8(options[QOI_IMPORT_FLAGS]);
	file->store_64(pos);

	err = file->get_error();
	if ((int)err) {
		Godot::print_error("Can't write config for QOI", __FUNCTION__, __FILE__, __LINE__);
		file->close();
		return (int)err;
	}

	file->close();
	return (int)err;
}
