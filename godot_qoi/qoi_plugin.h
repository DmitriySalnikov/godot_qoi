#pragma once

#include "qoi_import.h"

#include <EditorPlugin.hpp>
#include <Godot.hpp>

using namespace godot;

class QOIPlugin : public EditorPlugin {
	GODOT_CLASS(QOIPlugin, EditorPlugin)

	Ref<QOIImport> qoi_importer;

public:
	static void _register_methods();
	void _init();

	void _enter_tree();
	void _exit_tree();
};
