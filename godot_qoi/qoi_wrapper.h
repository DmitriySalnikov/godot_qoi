/* qoi_wrapper.h */

#pragma once

#include "qoi_shared.h"
#include <Godot.hpp>
#include <Image.hpp>
#include <Reference.hpp>

using namespace godot;

class QOI : public Reference {
	GODOT_CLASS(QOI, Reference)

private:
	void print_error(String error, String func, String file, int line);

public:
	static void _register_methods();
	void _init();

	bool print_errors = true;

	Ref<Image> read(String path);
	Ref<Image> decode(PoolByteArray data);
	int write(String path, Ref<Image> img);
	PoolByteArray encode(Ref<Image> img);
};
