/* register_types.cpp */

#include "qoi_import.h"
#include "qoi_save.h"
#include "qoi_wrapper.h"

#include "qoi_shared.h"

#include <godot_cpp/classes/project_settings.hpp>

using namespace godot;

Ref<QOIImport> qoi_import_plugin;
Ref<QOIResourceSaver> qoi_resource_saver;

/** GDNative Initialize **/
extern "C" void GDN_EXPORT initialize_godot_qoi_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<QOI>();
	ClassDB::register_class<QOIImport>();
	ClassDB::register_class<QOIResourceSaver>();

	ProjectSettings *ps = ProjectSettings::get_singleton();

#define DEFINE_SETTING_AND_GET(var, path, def, type) \
	{                                                \
		if (!ps->has_setting(path)) {                \
			ps->set_setting(path, def);              \
		}                                            \
		Dictionary info;                             \
		info["name"] = path;                         \
		info["type"] = type;                         \
		ps->add_property_info(info);                 \
		ps->set_initial_value(path, def);            \
	}                                                \
	var = ps->get_setting(path)

	DEFINE_SETTING_AND_GET(bool importer_enabled, "rendering/textures/qoi/enable_qoi_importer", true, Variant::BOOL);
	DEFINE_SETTING_AND_GET(bool saver_enabled, "rendering/textures/qoi/enable_qoi_saver", true, Variant::BOOL);

	if (importer_enabled) {
		qoi_import_plugin.instantiate();
		qoi_import_plugin->add_format_loader();
	}

	if (saver_enabled) {
		qoi_resource_saver.instantiate();
		ResourceSaver::get_singleton()->add_resource_format_saver(qoi_resource_saver, false);
	}
}

/** GDNative Uninitialize **/
extern "C" void GDN_EXPORT uninitialize_godot_qoi_module(ModuleInitializationLevel p_level) {
	if (qoi_import_plugin.is_valid())
		qoi_import_plugin->remove_format_loader();
	qoi_import_plugin.unref();

	if (qoi_resource_saver.is_valid())
		ResourceSaver::get_singleton()->remove_resource_format_saver(qoi_resource_saver);
	qoi_resource_saver.unref();
}

/** GDNative Initialize **/
extern "C" {
GDNativeBool GDN_EXPORT godot_qoi_library_init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_initializer(initialize_godot_qoi_module);
	init_obj.register_terminator(uninitialize_godot_qoi_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
