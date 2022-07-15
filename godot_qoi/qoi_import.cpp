#include "qoi_import.h"

#include <ConfigFile.hpp>
#include <Directory.hpp>
#include <File.hpp>
#include <GlobalConstants.hpp>
#include <ProjectSettings.hpp>

void QOIImport::_register_methods() {
	register_method("_update_config_file", &QOIImport::_update_config_file);

	register_method("get_importer_name", &QOIImport::get_importer_name);
	register_method("get_visible_name", &QOIImport::get_visible_name);
	register_method("get_recognized_extensions", &QOIImport::get_recognized_extensions);
	register_method("get_save_extension", &QOIImport::get_save_extension);
	register_method("get_resource_type", &QOIImport::get_resource_type);
	register_method("get_preset_count", &QOIImport::get_preset_count);
	register_method("get_preset_name", &QOIImport::get_preset_name);
	register_method("get_import_options", &QOIImport::get_import_options);
	register_method("get_option_visibility", &QOIImport::get_option_visibility);
	register_method("import", &QOIImport::import);
}

void QOIImport::_init() {
	qoi_wrapper.instance();
	qoi_utils.instance();

	qoi_wrapper->print_errors = ProjectSettings::get_singleton()->get_setting("debug/settings/qoi_import/print_debug");
}

String QOIImport::get_importer_name() {
	return "qoi_importer";
}

String QOIImport::get_visible_name() {
	return "QOI";
}

Array QOIImport::get_recognized_extensions() {
	static Array a = Array::make("qoi");
	return a;
}

String QOIImport::get_save_extension() {
	return "qoi_import";
}

String QOIImport::get_resource_type() {
	return "ImageTexture";
}

int64_t QOIImport::get_preset_count() {
	return 3;
}

String QOIImport::get_preset_name(const int64_t preset) {
	static Array presets = Array::make("Default", "Pixel", "3D");
	return presets[(int)preset];
}

Array QOIImport::get_import_options(const int64_t preset) {
	static char flags_map[3] = { 4, 0, 7 };
	return Array::make(
			Dictionary::make(
					"name", QOIUtils::QOI_IMPORT_FLAGS,
					"default_value", flags_map[preset],
					"property_hint", GlobalConstants::PROPERTY_HINT_FLAGS,
					"hint_string", "Mipmaps,Repeat,Filter,Anisotropic,sRGB,Mirrored Repeat"),
			Dictionary::make(
					"name", QOIUtils::QOI_IMPORT_SIZE_LIMIT,
					"default_value", 0,
					"property_hint", GlobalConstants::PROPERTY_HINT_RANGE,
					"hint_string", "0,8192"),
			Dictionary::make(
					"name", QOIUtils::QOI_IMPORT_FIX_ALPHA_BORDER,
					"default_value", preset != 2 ? true : false,
					"property_hint", GlobalConstants::PROPERTY_HINT_NONE,
					"hint_string", ""),
			Dictionary::make(
					"name", QOIUtils::QOI_IMPORT_PREMULT_ALPHA,
					"default_value", false,
					"property_hint", GlobalConstants::PROPERTY_HINT_NONE,
					"hint_string", ""),
			Dictionary::make(
					"name", QOIUtils::QOI_IMPORT_INVERT_COLORS,
					"default_value", false,
					"property_hint", GlobalConstants::PROPERTY_HINT_NONE,
					"hint_string", ""),
			Dictionary::make(
					"name", QOIUtils::QOI_IMPORT_NORMAL_MAP_INVERT_Y,
					"default_value", false,
					"property_hint", GlobalConstants::PROPERTY_HINT_NONE,
					"hint_string", ""));
}

bool QOIImport::get_option_visibility(const String option, const Dictionary options) {
	return true;
}

int64_t QOIImport::import(const String source_file, const String save_path, const Dictionary options, const Array platform_variants, const Array gen_files) {
	String target_path = save_path + "." + get_save_extension();
	Dictionary fin_options = options;

	Error err = Error::OK;
	Ref<Image> image = qoi_wrapper->read(source_file);

	if (image.is_null() || image->is_empty()) {
		PRINT_ERROR("Can't open QOI image: " + source_file);
		return (int)err;
	}

	// Get options from the footer if it exists but '[file].import' doesn't exists
	{
		Ref<Directory> dir;
		dir.instance();
		String import_file = source_file + ".import";
		if (!dir->file_exists(import_file)) {
			Dictionary footer = qoi_utils->read_footer(source_file);
			if (!footer.has(QOIUtils::QOI_IMPORT_ERROR)) {
				qoi_utils->update_dictionary(fin_options, footer);
			}

			// Forced update of the .import file according to the new options
			call_deferred("_update_config_file", import_file, fin_options);
		}
	}

	// ------------------------------------------
	// adapted from godot/editor/import/resource_importer_texture.cpp
	int size_limit = fin_options[QOIUtils::QOI_IMPORT_SIZE_LIMIT];
	bool fix_alpha_border = fin_options[QOIUtils::QOI_IMPORT_FIX_ALPHA_BORDER];
	bool premult_alpha = fin_options[QOIUtils::QOI_IMPORT_PREMULT_ALPHA];
	bool invert_color = fin_options[QOIUtils::QOI_IMPORT_INVERT_COLORS];
	bool normal_map_invert_y = fin_options[QOIUtils::QOI_IMPORT_NORMAL_MAP_INVERT_Y];

	if (size_limit > 0 && (image->get_width() > size_limit || image->get_height() > size_limit)) {
		// limit size
		if (image->get_width() >= image->get_height()) {
			int new_width = size_limit;
			int new_height = (int)image->get_height() * new_width / (int)image->get_width();

			image->resize(new_width, new_height, Image::INTERPOLATE_CUBIC);
		} else {
			int new_height = size_limit;
			int new_width = (int)image->get_width() * new_height / (int)image->get_height();

			image->resize(new_width, new_height, Image::INTERPOLATE_CUBIC);
		}
	}

	if (fix_alpha_border) {
		image->fix_alpha_edges();
	}

	if (premult_alpha) {
		image->premultiply_alpha();
	}

	if (invert_color) {
		int height = (int)image->get_height();
		int width = (int)image->get_width();

		image->lock();
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				image->set_pixel(i, j, image->get_pixel(i, j).inverted());
			}
		}
		image->unlock();
	}

	if (normal_map_invert_y) {
		// Inverting the green channel can be used to flip a normal map's direction.
		// There's no standard when it comes to normal map Y direction, so this is
		// sometimes needed when using a normal map exported from another program.
		// See <http://wiki.polycount.com/wiki/Normal_Map_Technical_Details#Common_Swizzle_Coordinates>.
		const int height = (int)image->get_height();
		const int width = (int)image->get_width();

		image->lock();
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				const Color color = image->get_pixel(i, j);
				image->set_pixel(i, j, Color(color.r, 1 - color.g, color.b));
			}
		}
		image->unlock();
	}

	// ------------------------------------------

	err = (Error)qoi_wrapper->write(target_path, image);
	if ((int)err) {
		PRINT_ERROR("Can't save QOI in .import folder: " + target_path + ". Error: " + String::num_int64((int)err));
		return (int)err;
	}

	return qoi_utils->add_footer(target_path, fin_options, false);
}

// Hack to sync options in the .import file after the Save As... operation
void QOIImport::_update_config_file(String path, Dictionary options) {
	Ref<ConfigFile> cfg;
	cfg.instance();
	Error err = cfg->load(path);
	if ((int)err) {
		return;
	}

	Array keys = options.keys();
	for (int i = 0; i < options.size(); i++) {
		cfg->set_value("params", keys[i], options[keys[i]]);
	}

	cfg->save(path);
}
