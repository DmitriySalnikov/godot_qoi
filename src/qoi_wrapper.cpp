#include "qoi_wrapper.h"

#define QOI_NO_STDIO
#define QOI_IMPLEMENTATION

#include "qoi.h"

using namespace godot;

void QOI::_bind_methods() {
	ClassDB::bind_static_method(TEXT(QOI), D_METHOD(TEXT(write), "path", "image"), &QOI::write);
	ClassDB::bind_static_method(TEXT(QOI), D_METHOD(TEXT(encode), "image"), &QOI::encode);
	ClassDB::bind_static_method(TEXT(QOI), D_METHOD(TEXT(read), "path"), &QOI::read);
	ClassDB::bind_static_method(TEXT(QOI), D_METHOD(TEXT(decode), "data"), &QOI::decode);
}

Ref<Image> QOI::read(String path) {
	ERR_FAIL_COND_V_MSG(!FileAccess::file_exists(path), Ref<Image>(), "File does not exist: " + path);

	Ref<FileAccess> f = FileAccess::open(path, FileAccess::READ);
	ERR_FAIL_COND_V_MSG(f.is_null(), Ref<Image>(), "Could not open the file for reading: " + path + ". Error: " + String::num_int64((int)FileAccess::get_open_error()));

	PackedByteArray data = f->get_buffer(f->get_length());
	ERR_FAIL_COND_V_MSG(f->get_error() != Error::OK, Ref<Image>(), "Failed to write data to file " + path + ". Error: " + String::num_int64((int)f->get_error()));
	f.unref();

	return decode(data);
}

Ref<Image> QOI::decode(const PackedByteArray &data) {
	ERR_FAIL_COND_V_MSG(data.size() == 0, Ref<Image>(), "Image data cannot be empty");

	Ref<Image> img;
	img.instantiate();

	if (decode_to_image(data, img) != Error::OK)
		return Ref<Image>();

	return img;
}

Error QOI::decode_to_image(const PackedByteArray &data, const Ref<Image> &out_image) {
	qoi_desc desc;
	void *out;

	out = qoi_decode(data.ptr(), (int)data.size(), &desc, 0);
	ERR_FAIL_COND_V_MSG(out == NULL, ERR_FILE_CORRUPT, "Unable to decode data");

	PackedByteArray img_data;

	int64_t size = desc.channels * desc.width * desc.height;
	img_data.resize(size);

	if (img_data.size() != size) {
		::free(out);
		ERR_FAIL_V_MSG(ERR_OUT_OF_MEMORY, "Unable to resize PackedByteArray");
	}

	memcpy(img_data.ptrw(), out, size);
	::free(out);

	const_cast<Image *>(out_image.ptr())->set_data(desc.width, desc.height, false, desc.channels == 3 ? Image::Format::FORMAT_RGB8 : Image::Format::FORMAT_RGBA8, img_data);
	return Error::OK;
}

Error QOI::write(String path, Ref<Image> img) {
	ERR_FAIL_COND_V_MSG(img.is_null(), Error::ERR_INVALID_PARAMETER, "Image cannot be null");
	ERR_FAIL_COND_V_MSG(img->is_empty(), Error::ERR_INVALID_PARAMETER, "Image cannot be empty");

	PackedByteArray b = encode(img);
	ERR_FAIL_COND_V_MSG(b.size() == 0, Error::FAILED, "Image cannot be null or empty");

	Ref<FileAccess> f = FileAccess::open(path, FileAccess::WRITE);

	ERR_FAIL_COND_V_MSG(f.is_null(), FileAccess::get_open_error(), "Could not open the file for writing: " + path + ". Error: " + String::num_int64((int)FileAccess::get_open_error()));

	f->store_buffer(b);
	auto err = f->get_error();
	f.unref();

	ERR_FAIL_COND_V_MSG(err != Error::OK, f->get_error(), "Failed to write data to file " + path + ". Error: " + String::num_int64((int)err));

	return Error::OK;
}

PackedByteArray QOI::encode(Ref<Image> img) {
	ERR_FAIL_COND_V_MSG(img.is_null(), PackedByteArray(), "Image cannot be null");
	ERR_FAIL_COND_V_MSG(img->is_empty(), PackedByteArray(), "Image cannot be empty");

	bool has_alpha = img->detect_alpha();

	if (img->get_format() != Image::Format::FORMAT_RGB8 && img->get_format() != Image::Format::FORMAT_RGBA8) {
		// try to convert
		img->convert(has_alpha ? Image::Format::FORMAT_RGBA8 : Image::Format::FORMAT_RGB8);

		ERR_FAIL_COND_V_MSG(img->get_format() != Image::FORMAT_RGB8 && img->get_format() != Image::FORMAT_RGBA8, PackedByteArray(), "Unsupported image format");
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

	out = qoi_encode(img->get_data().ptr(), &enc, &len);
	ERR_FAIL_COND_V_MSG(out == NULL, PackedByteArray(), "Unable to encode the image");

	PackedByteArray res;
	res.resize(len);
	if (res.size() != len) {
		::free(out);
		ERR_FAIL_V_MSG(PackedByteArray(), "Unable to resize PackedByteArray");
	}

	memcpy(res.ptrw(), out, len);
	::free(out);

	return res;
}
