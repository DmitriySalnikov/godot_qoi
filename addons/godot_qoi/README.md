<img src="https://github.com/DmitriySalnikov/godot_qoi/blob/5bd25a2dc2ea907041b4c9a7f4ae12bc0ae19a94/icon.png" width=128/>

# godot_qoi

QOI ([Quite OK Image Format](https://github.com/phoboslab/qoi)) integration for Godot Engine 4.x.

This is a GDExtension library. Includes precompiled binaries for `Windows`, `Linux`, `macOS` and `Android`, but it must compile for each platform which Godot Engine supports.

[Godot 3 version](https://github.com/DmitriySalnikov/godot_qoi/tree/godot_3)

## Features

* Fastest encoding and fast decoding (when working with the QOI class)
* Editor integration (use .qoi like regular textures)
* Cross-platform
* Simple API

## Disadvantage

* Large file size when working with `.qoi` files directly (compared to PNG or WebP. more than 2-3 times larger)

## Approximate comparison of QOI encoding speed vs PNG

https://user-images.githubusercontent.com/7782218/152955097-285f81dc-2b65-4f80-bd6a-890b7280d806.mp4

## Support

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/I2I53VZ2D)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://paypal.me/dmitriysalnikov)

[<img src="https://upload.wikimedia.org/wikipedia/commons/8/8f/QIWI_logo.svg" alt="qiwi" width=90px/>](https://qiwi.com/n/DMITRIYSALNIKOV)

## Download

To download, use the [Godot Asset Library](https://godotengine.org/asset-library/asset/1619) or download the archive by clicking the button at the top of the main repository page: `Code -> Download ZIP`, then unzip it to your project folder. Or use one of the stable versions from the [GitHub Releases](https://github.com/DmitriySalnikov/godot_qoi/releases) page (just download one of the "Source Codes" in assets).

## Editor integration

By default, after installation, QOI importer and QOI saver will be active.

The importer allows you to import `.qoi` files and use them as regular textures in your project.

The saver allows you to save `Image` and `Texture2D` resources as `.qoi` files using standard Godot tools.

But if you don't need these integrations and you want to use only the `QOI` class, then you can disable them in the project settings. These settings are located here: `rendering/textures/qoi/*`

![image](https://user-images.githubusercontent.com/7782218/204029440-edc7e6a1-0dcf-46ab-bdca-8c85490de499.png)

## API

The entire API is in the `QOI` class. It can be found in the documentation inside the engine.

```gdscript
# Read the QOI image from file
# @return null or Image
Image read(path : String) static

# Decode the QOI image from an encoded byte array
# @return null or Image
Image decode(data : PackedByteArray) static

# Write the Image as a file in QOI format
# @return code of Error
Error write(path : String, image : Image) static

# Encode the Image in QOI format as an array of bytes
# @return an array of bytes. If an error occurs, it is empty.
PackedByteArray encode(image : Image) static
```

### Example

```gdscript
func example():
	QOI.write("user://example.qoi", load("res://icon.svg").get_image())
	var img = QOI.read("user://example.qoi")
	var enc = QOI.encode(img)
	var dec = QOI.decode(enc)
	
	var tex: = ImageTexture.create_from_image(dec)
	$TextureRect.texture = tex
```

## License

MIT license
