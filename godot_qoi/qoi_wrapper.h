/* qoi_wrapper.h */

#pragma once

#include <Godot.hpp>
#include <Reference.hpp>
#include <Image.hpp>

using namespace godot;

class QOI : public Reference {
	GODOT_CLASS(QOI, Reference)

private:

public:
	static void _register_methods();
	void _init();

	Ref<Image> read(String path);
	Ref<Image> decode(PoolByteArray data);
	int write(String path, Ref<Image> img);
	PoolByteArray encode(Ref<Image> img);

};
