/* register_types.cpp */

#include "register_types.h"
#include <Godot.hpp>
#include "qoi_wrapper.h"

using namespace godot;

/** GDNative Initialize **/
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
	Godot::gdnative_init(o);
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