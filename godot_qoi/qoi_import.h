#pragma once

#include "qoi_utils.h"
#include "qoi_wrapper.h"

#include <EditorImportPlugin.hpp>
#include <Godot.hpp>

using namespace godot;

class QOIImport : public EditorImportPlugin {
	GODOT_CLASS(QOIImport, EditorImportPlugin)

	Ref<QOI> qoi_wrapper;
	Ref<QOIUtils> qoi_utils;

public:
	void _update_config_file(String path, Dictionary options);

	static void _register_methods();
	void _init();

	String get_importer_name();
	String get_visible_name();
	Array get_recognized_extensions();
	String get_save_extension();
	String get_resource_type();
	int64_t get_preset_count();
	String get_preset_name(const int64_t preset);
	Array get_import_options(const int64_t preset);
	bool get_option_visibility(const String option, const Dictionary options);
	int64_t import(const String source_file, const String save_path, const Dictionary options, const Array platform_variants, const Array gen_files);
};
