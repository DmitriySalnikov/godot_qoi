#include "qoi_utils.h"

#include <File.hpp>
#include <Image.hpp>
#include <ProjectSettings.hpp>

const char QOIUtils::qoi_cfg_magic[4] = { 0x71, 0x63, 0x66, 0x67 };

const char *QOIUtils::QOI_IMPORT_FLAGS = "flags";
const char *QOIUtils::QOI_IMPORT_SIZE_LIMIT = "process/size_limit";
const char *QOIUtils::QOI_IMPORT_FIX_ALPHA_BORDER = "process/fix_alpha_border";
const char *QOIUtils::QOI_IMPORT_PREMULT_ALPHA = "process/premult_alpha";
const char *QOIUtils::QOI_IMPORT_INVERT_COLORS = "process/invert_color";
const char *QOIUtils::QOI_IMPORT_NORMAL_MAP_INVERT_Y = "process/normal_map_invert_y";

void QOIUtils::_register_methods() {
	register_method("add_footer", &QOIUtils::add_footer);
	register_method("save_resource", &QOIUtils::save_resource);
	register_method("load_resource", &QOIUtils::load_resource);

	register_property("QOI_CFG_MAGIC", &QOIUtils::_fake_set_prop_pba, &QOIUtils::_get_qoi_cfg_magic, get_qoi_magic());

	register_property("QOI_IMPORT_FLAGS", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_flags, String(QOI_IMPORT_FLAGS));
	register_property("QOI_IMPORT_SIZE_LIMIT", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_size_limits, String(QOI_IMPORT_SIZE_LIMIT));
	register_property("QOI_IMPORT_FIX_ALPHA_BORDER", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_fix_alpha, String(QOI_IMPORT_FIX_ALPHA_BORDER));
	register_property("QOI_IMPORT_PREMULT_ALPHA", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_premult_alpha, String(QOI_IMPORT_PREMULT_ALPHA));
	register_property("QOI_IMPORT_INVERT_COLORS", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_invert_colors, String(QOI_IMPORT_INVERT_COLORS));
	register_property("QOI_IMPORT_NORMAL_MAP_INVERT_Y", &QOIUtils::_fake_set_prop_str, &QOIUtils::_get_import_normal_map_invert_y, String(QOI_IMPORT_NORMAL_MAP_INVERT_Y));
}

void QOIUtils::_init() {
	qoi_wrapper.instance();

	qoi_wrapper->print_errors = ProjectSettings::get_singleton()->get_setting("debug/settings/qoi_import/print_debug");
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

int QOIUtils::save_resource(const String path, const Ref<Resource> resource, const int64_t flags) {
	Ref<Texture> tex = resource;
	if (tex.is_null()) {
		Godot::print_error("Texture resource is empty", __FUNCTION__, __FILE__, __LINE__);
		return (int)Error::ERR_INVALID_DATA;
	}

	Ref<Image> img = tex->get_data();
	if (img->is_empty()) {
		Godot::print_error("Image resource is empty", __FUNCTION__, __FILE__, __LINE__);
		return (int)Error::ERR_INVALID_DATA;
	}

	Error err = (Error)qoi_wrapper->write(path, img);
	if ((int)err) {
		Godot::print_error("Can't save QOI image. Error: " + String::num_int64((int)err), __FUNCTION__, __FILE__, __LINE__);
		return (int)err;
	}

	Dictionary options = Dictionary::make(QOIUtils::QOI_IMPORT_FLAGS, tex->get_flags());
	err = (Error)add_footer(path, options);

	// Fix .import file options
	Ref<File> file;
	file.instance();
	file->open(String("res://.import/{0}-{1}").format(Array::make(path.get_file(), path.md5_text())) + ".qoi_save_as", File::ModeFlags::WRITE);
	file->store_var(options);
	file->close();

	if (tex->get_class() != "ImageTexture") {
		Godot::print_warning("Please reload the project to properly import and update the newly created QOI file.", __FUNCTION__, __FILE__, __LINE__);
	}

	return (int)err;
}

Ref<ImageTexture> QOIUtils::load_resource(const String path, const String original_path) {
	Ref<ImageTexture> tex;
	tex.instance();
	int64_t file_size = 0;
	int flags = Texture::Flags::FLAGS_DEFAULT;

	// #################
	// Check QOI config at the end of file
	{

		Ref<File> file;
		file.instance();

		Error err = file->open(path, File::ModeFlags::READ);
		if ((int)err) {
			Godot::print_error("Can't open QOI config. Error: " + String::num_int64((int)err), __FUNCTION__, __FILE__, __LINE__);
			return tex;
		}

		// footer:
		// qcfg - magic
		// -- data --
		// 8-bit the size of the cfg data in bytes, including these 8 bits
		// 8-bit flags
		// -- data --
		// 64-bit pos of magic

		file->seek_end(-8);
		file_size = file->get_64();
		file->seek(file_size);

		PoolByteArray magic = file->get_buffer(4);
		PoolByteArray correct_magic = get_qoi_magic();

		if (magic[0] == correct_magic[0] && magic[1] == correct_magic[1] && magic[2] == correct_magic[2] && magic[3] == correct_magic[3]) {
			// whether the size of the cfg data is equal to the size of the current version..
			if (file->get_8() == 2) {
				flags = file->get_8();
			} else {
				Godot::print_error("Incorrect QOI config was found in the .import'ed file. Please reimport this file: " + path, __FUNCTION__, __FILE__, __LINE__);
			}
		} else {
			// just ignore this and use defaults
		}
		file->close();
	}

	// #################

	Ref<Image> q = qoi_wrapper->read(path);
	if (q->is_empty()) {
		Godot::print_error("The Image was not loaded correctly", __FUNCTION__, __FILE__, __LINE__);
		return tex;
	}

	if (flags & Texture::Flags::FLAG_MIPMAPS) {
		q->generate_mipmaps();
	}

	tex->create_from_image(q, flags);

	return tex;
}
