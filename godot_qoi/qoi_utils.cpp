#include "qoi_utils.h"

#include <File.hpp>
#include <Image.hpp>
#include <ProjectSettings.hpp>

const char QOIUtils::qoi_cfg_magic[4] = { 0x71, 0x63, 0x66, 0x67 };

const char *QOIUtils::QOI_IMPORT_ERROR = "error";
const char *QOIUtils::QOI_IMPORT_FLAGS = "flags";
const char *QOIUtils::QOI_IMPORT_SIZE_LIMIT = "process/size_limit";
const char *QOIUtils::QOI_IMPORT_FIX_ALPHA_BORDER = "process/fix_alpha_border";
const char *QOIUtils::QOI_IMPORT_PREMULT_ALPHA = "process/premult_alpha";
const char *QOIUtils::QOI_IMPORT_INVERT_COLORS = "process/invert_color";
const char *QOIUtils::QOI_IMPORT_NORMAL_MAP_INVERT_Y = "process/normal_map_invert_y";

void QOIUtils::_register_methods() {
	register_method("save_resource", &QOIUtils::save_resource);
	register_method("load_resource", &QOIUtils::load_resource);
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

int QOIUtils::add_footer(String target_path, Dictionary options, bool update) {
	Ref<File> file;
	file.instance();
	Error err;

	if (update) {
		err = file->open(target_path, File::READ);
		if ((int)err) {
			PRINT_ERROR("Can't open QOI file for reading: " + target_path);
			return (int)err;
		}

		int64_t file_size = 0;
		file->seek_end(-8);
		file_size = file->get_64();
		file->seek(file_size);

		PoolByteArray magic = file->get_buffer(4);
		PoolByteArray correct_magic = get_qoi_magic();
		PoolByteArray file_content;

		// Try to find correct magic numbers
		if (magic[0] == correct_magic[0] && magic[1] == correct_magic[1] && magic[2] == correct_magic[2] && magic[3] == correct_magic[3]) {
			file->seek(0);
			file_content = file->get_buffer(file_size);
		}
		file->close();

		err = file->open(target_path, file_content.size() > 0 ? File::WRITE : File::READ_WRITE);
		if ((int)err) {
			PRINT_ERROR("Can't open QOI file for writing: " + target_path);
			return (int)err;
		}

		if (file_content.size() > 0) {
			file->store_buffer(file_content);

			err = file->get_error();
			if ((int)err) {
				PRINT_ERROR("Can't rewrite QOI file without config: " + target_path);
				file->close();
				return (int)err;
			}
		}
	} else {
		err = file->open(target_path, File::READ_WRITE);
		if ((int)err) {
			PRINT_ERROR("Can't open QOI file for editing: " + target_path);
			file->close();
			return (int)err;
		}
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
		PRINT_ERROR("Can't write config for QOI: " + target_path);
		file->close();
		return (int)err;
	}

	file->close();
	return (int)err;
}

Dictionary QOIUtils::read_footer(String target_path) {
	Dictionary result;
	Ref<File> file;
	file.instance();

	Error err = file->open(target_path, File::ModeFlags::READ);
	if ((int)err) {
		PRINT_ERROR("Can't open QOI config: " + target_path + ". Error: " + String::num_int64((int)err));
		return result;
	}

	// footer:
	// read comments above

	int64_t file_size = 0;
	file->seek_end(-8);
	file_size = file->get_64();
	file->seek(file_size);

	PoolByteArray magic = file->get_buffer(4);
	PoolByteArray correct_magic = get_qoi_magic();

	if (magic[0] == correct_magic[0] && magic[1] == correct_magic[1] && magic[2] == correct_magic[2] && magic[3] == correct_magic[3]) {
		// whether the size of the cfg data is equal to the size of the current version..
		if (file->get_8() == 2) {
			result[QOI_IMPORT_FLAGS] = (int)file->get_8();
		} else {
			PRINT_ERROR("Incorrect QOI config was found in the .import'ed file: " + target_path + ". Please reimport this file: " + target_path);
			result[QOI_IMPORT_ERROR] = (int)Error::ERR_FILE_CORRUPT;
		}
	} else {
		// just ignore this and use defaults
	}
	file->close();

	return result;
}

void QOIUtils::update_dictionary(Dictionary &data, const Dictionary updates) {
	Array keys = updates.keys();
	for (int i = 0; i < keys.size(); i++) {
		data[keys[i]] = updates[keys[i]];
	}
}

int QOIUtils::save_resource(const String path, const Ref<Resource> resource, const int64_t flags) {
	Ref<Texture> tex = resource;
	if (tex.is_null()) {
		PRINT_ERROR("Texture resource is null");
		return (int)Error::ERR_INVALID_DATA;
	}

	Dictionary options = Dictionary::make(QOIUtils::QOI_IMPORT_FLAGS, tex->get_flags());
	if (path == resource->get_path()) {
		// No need to replace the file by itself
		// Otherwise, the changes after import will be permanent
		// But it is possible to change the qoi parameters in footer
		return (int)add_footer(path, options, true);
	}

	Ref<Image> img = tex->get_data();
	Error err;

	if (img.is_null() || img->is_empty()) {
		PRINT_ERROR("Image resource is empty", __FUNCTION__, __REP_FILE__, __LINE__);
		return (int)Error::ERR_INVALID_DATA;
	}

	if (img->is_compressed()) {
		img = img->duplicate();
		err = img->decompress();
		if ((int)err) {
			PRINT_ERROR("Can't decompress compressed image. Error: " + String::num_int64((int)err));
			return (int)err;
		}
	}

	err = (Error)qoi_wrapper->write(path, img);
	if ((int)err) {
		PRINT_ERROR("Can't save QOI image: " + path + ". Error: " + String::num_int64((int)err));
		return (int)err;
	}

	err = (Error)add_footer(path, options, false);

	if (tex->get_class() != "ImageTexture") {
		PRINT_WARNING("Please reload the project to properly import and update the newly created QOI file.");
	}

	return (int)err;
}

Ref<ImageTexture> QOIUtils::load_resource(const String path, const String original_path) {
	Ref<ImageTexture> tex;
	tex.instance();
	int flags = Texture::Flags::FLAGS_DEFAULT;

	Dictionary footer = read_footer(path);
	if (!footer.has(QOI_IMPORT_ERROR)) {
		flags = (int)footer[QOI_IMPORT_FLAGS];
	}

	Ref<Image> q = qoi_wrapper->read(path);
	if (q.is_null() || q->is_empty()) {
		PRINT_ERROR("The Image was not loaded correctly");
		return tex;
	}

	if (flags & Texture::Flags::FLAG_MIPMAPS) {
		q->generate_mipmaps();
	}

	tex->create_from_image(q, flags);

	return tex;
}
