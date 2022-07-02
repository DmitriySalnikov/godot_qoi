/* register_types.cpp */

#include "register_types.h"
#include <Godot.hpp>
#include <Directory.hpp>
#include <File.hpp>

#include "qoi_wrapper.h"

using namespace godot;

/** GDNative Initialize **/
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
	Godot::gdnative_init(o);

	// Custom register and init for only needed classes
	// Works only with my patches
	godot::_TagDB::register_global_type("Image", typeid(Image).hash_code(), typeid(Resource).hash_code());
	godot::_TagDB::register_global_type("Object", typeid(Object).hash_code(), 0);
	godot::_TagDB::register_global_type("Reference", typeid(Reference).hash_code(), typeid(Object).hash_code());
	godot::_TagDB::register_global_type("_Directory", typeid(Directory).hash_code(), typeid(Reference).hash_code());
	godot::_TagDB::register_global_type("_File", typeid(File).hash_code(), typeid(Reference).hash_code());

	Image::___init_method_bindings();
	Object::___init_method_bindings();
	Reference::___init_method_bindings();
	Directory::___init_method_bindings();
	File::___init_method_bindings();
}

/** GDNative Terminate **/
extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
	Godot::gdnative_terminate(o);
}

/** GDNative Singleton **/
extern "C" void GDN_EXPORT godot_gdnative_singleton() {
}

/** NativeScript Initialize **/
extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
	Godot::nativescript_init(handle);
	
	register_class<QOI>();
}