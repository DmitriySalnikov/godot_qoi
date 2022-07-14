/* qoi_wrapper.cpp */

#include "qoi_wrapper.h"
#include <Directory.hpp>
#include <File.hpp>

#define QOI_NO_STDIO
#define QOI_IMPLEMENTATION

#include "qoi.h"

using namespace godot;

void QOI::print_error(String error, String func, String file, int line) {
	if (print_errors)
		Godot::print_error(error, func, file, line);
}

void QOI::_register_methods() {
	register_method("write", &QOI::write);
	register_method("encode", &QOI::encode);
	register_method("read", &QOI::read);
	register_method("decode", &QOI::decode);
	register_property("print_errors", &QOI::print_errors, true);
}

Ref<Image> QOI::read(String path) {
	Ref<File> f;
	f.instance();

	if (!f->file_exists(path)) {
		print_error("File does not exist: " + path, __FUNCTION__, __FILE__, __LINE__);
		return Ref<Image>();
	}

	auto err = f->open(path, File::ModeFlags::READ);
	if (err != Error::OK) {
		print_error("Could not open the file for reading: " + path + ". Error: " + String::num_int64((int)err), __FUNCTION__, __FILE__, __LINE__);
		return Ref<Image>();
	}

	PoolByteArray data = f->get_buffer(f->get_len());
	return decode(data);
}

Ref<Image> QOI::decode(PoolByteArray data) {
	if (data.size() == 0) {
		print_error("Image data cannot be empty", __FUNCTION__, __FILE__, __LINE__);
		return Ref<Image>();
	}

	qoi_desc dec;
	void *out;

	out = qoi_decode(data.read().ptr(), data.size(), &dec, 0);
	if (out == NULL) {
		print_error("Unable to decode data", __FUNCTION__, __FILE__, __LINE__);
		return Ref<Image>();
	}

	PoolByteArray img_data;

	img_data.resize(dec.channels * dec.width * dec.height);

	memcpy(img_data.write().ptr(), out, img_data.size());
	::free(out);

	Ref<Image> img;
	img.instance();
	img->create_from_data(dec.width, dec.height, false, dec.channels == 3 ? Image::Format::FORMAT_RGB8 : Image::Format::FORMAT_RGBA8, img_data);

	return img;
}

int QOI::write(String path, Ref<Image> img) {
	if (img.is_null() || img->is_empty()) {
		print_error("Image cannot be null or empty", __FUNCTION__, __FILE__, __LINE__);
		return (int)Error::ERR_INVALID_PARAMETER;
	}

	Ref<File> f;
	f.instance();

	auto err = f->open(path, File::ModeFlags::WRITE);
	if (err != Error::OK) {
		print_error("Could not open the file for writing: " + path + ". Error: " + String::num_int64((int)err), __FUNCTION__, __FILE__, __LINE__);
		return (int)err;
	}

	auto b = encode(img);
	if (b.size() == 0)
		return (int)Error::FAILED;

	f->store_buffer(b);
	err = f->get_error();
	if (err != Error::OK) {
		print_error("Failed to write data to file " + path + ". Error: " + String::num_int64((int)err), __FUNCTION__, __FILE__, __LINE__);
		return (int)err;
	}

	f->close();

	return (int)Error::OK;
}

PoolByteArray QOI::encode(Ref<Image> img) {
	if (img.is_null() || img->is_empty()) {
		print_error("Image cannot be null or empty", __FUNCTION__, __FILE__, __LINE__);
		return PoolByteArray();
	}

	bool has_alpha = img->detect_alpha();

	if (img->get_format() != Image::Format::FORMAT_RGB8 && img->get_format() != Image::Format::FORMAT_RGBA8) {
		// try to convert
		img->convert(has_alpha ? Image::Format::FORMAT_RGBA8 : Image::Format::FORMAT_RGB8);

		if (img->get_format() != Image::Format::FORMAT_RGB8 && img->get_format() != Image::Format::FORMAT_RGBA8) {
			print_error("Unsupported image format", __FUNCTION__, __FILE__, __LINE__);
			return PoolByteArray();
		}
	} else {
		img->convert(has_alpha ? Image::Format::FORMAT_RGBA8 : Image::Format::FORMAT_RGB8);
	}

	qoi_desc enc = {
		(uint32_t)img->get_width(),
		(uint32_t)img->get_height(),
		(uint8_t)(img->get_format() == Image::Format::FORMAT_RGB8 ? 3 : 4),
		QOI_SRGB
	};

	int len = 0;
	void *out;

	img->lock();
	out = qoi_encode(img->get_data().read().ptr(), &enc, &len);
	if (out == NULL) {
		print_error("Unable to encode the image", __FUNCTION__, __FILE__, __LINE__);
		return PoolByteArray();
	}
	img->unlock();

	PoolByteArray res;
	res.resize(len);

	memcpy(res.write().ptr(), out, len);
	::free(out);

	return res;
}

void QOI::_init() {
	// initialize any variables here
}
