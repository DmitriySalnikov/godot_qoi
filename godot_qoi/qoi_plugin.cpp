#include "qoi_plugin.h"

#include <ProjectSettings.hpp>

void QOIPlugin::_register_methods() {
	register_method("_enter_tree", &QOIPlugin::_enter_tree);
	register_method("_exit_tree", &QOIPlugin::_exit_tree);
}

void QOIPlugin::_init() {
	qoi_importer.instance();

	String prnt_dbg = "debug/settings/qoi_import/print_debug";
	if (!ProjectSettings::get_singleton()->has_setting(prnt_dbg))
		ProjectSettings::get_singleton()->set_setting(prnt_dbg, true);
}

void QOIPlugin::_enter_tree() {
	add_import_plugin(qoi_importer);
}

void QOIPlugin::_exit_tree() {
	remove_import_plugin(qoi_importer);
}
