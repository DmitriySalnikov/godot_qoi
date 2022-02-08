/* qoi_wrapper.cpp */

#include "qoi_wrapper.h"
#include <Directory.hpp>
#include <File.hpp>

#define QOI_NO_STDIO
#define QOI_IMPLEMENTATION

#include "qoi.h"

using namespace godot;

void QOI::_register_methods() {
	register_method("write", &QOI::write);
	register_method("encode", &QOI::encode);
	register_method("read", &QOI::read);
	register_method("decode", &QOI::decode);
}

Ref<Image> QOI::read(String path) {
	Ref<File> f;
	f.instance();

	if (!f->file_exists(path))
		return Ref<Image>();

	auto err = f->open(path, File::ModeFlags::READ);
	if (err != Error::OK)
		return Ref<Image>();

	PoolByteArray data = f->get_buffer(f->get_len());
	return decode(data);
}

Ref<Image> QOI::decode(PoolByteArray data) {
	qoi_desc dec;
	void *out;

	{
		auto read = data.read();
		out = qoi_decode(read.ptr(), data.size(), &dec, 0);

		if (out == NULL)
			return Ref<Image>();
	}

	PoolByteArray img_data;
	img_data.resize(dec.channels * dec.width * dec.height);

	{
		auto write = img_data.write();
		memcpy(write.ptr(), out, img_data.size());
		::free(out);
	}

	Ref<Image> img;
	img.instance();
	img->create_from_data(dec.width, dec.height, false, dec.channels == 3 ? Image::Format::FORMAT_RGB8 : Image::Format::FORMAT_RGBA8, img_data);

	return img;
}

int QOI::write(String path, Ref<Image> img) {
	Ref<File> f;
	f.instance();

	auto err = f->open(path, File::ModeFlags::WRITE);
	if (err != Error::OK)
		return (int)err;

	auto b = encode(img);
	if (b.size() == 0)
		return (int)Error::FAILED;

	f->store_buffer(b);
	err = f->get_error();
	if (err != Error::OK)
		return (int)err;

	f->close();
	if (err != Error::OK)
		return (int)err;

	return (int)Error::OK;
}

PoolByteArray QOI::encode(Ref<Image> img) {
	if (img->get_format() != Image::Format::FORMAT_RGB8 && img->get_format() != Image::Format::FORMAT_RGBA8) {
		// try to convert
		img->convert(Image::Format::FORMAT_RGBA8);

		if (img->get_format() != Image::Format::FORMAT_RGB8 && img->get_format() != Image::Format::FORMAT_RGBA8) {
			Godot::print_error("Unsupported image format.", __FUNCTION__, __FILE__, __LINE__);
			return PoolByteArray();
		}
	}

	qoi_desc enc = {
		(uint32_t)img->get_width(),
		(uint32_t)img->get_height(),
		(uint8_t)(img->get_format() == Image::Format::FORMAT_RGB8 ? 3 : 4),
		QOI_SRGB
	};

	int len = 0;
	void *out;
	{
		img->lock();
		auto data = img->get_data();
		auto read = data.read();
		out = qoi_encode(read.ptr(), &enc, &len);
		if (out == NULL)
			return PoolByteArray();
		img->unlock();
	}

	PoolByteArray res;
	res.resize(len);

	{
		auto write = res.write();
		memcpy(write.ptr(), out, len);
		::free(out);
	}

	return res;
}

void QOI::_init() {
	// initialize any variables here
}
