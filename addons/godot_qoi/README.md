<img src="/images/icon.png" width=128/>

# godot_qoi

QOI ([Quite OK Image Format](https://github.com/phoboslab/qoi)) integration for Godot Engine 4.x.

This is a GDExtension library. Includes precompiled binaries for `Windows`, `Linux`, `macOS`, `iOS`, `Android` and `Web`, but it must compile for each platform which Godot Engine supports.

[Godot 3 version](https://github.com/DmitriySalnikov/godot_qoi/tree/godot_3)

## Features

* Fastest encoding and fast decoding (when working with the QOI class in scripts)
* Editor integration (use `.qoi` as regular textures)
* Cross-platform
* Simple API

## Disadvantage

* Large file size when working with `.qoi` files directly (compared to PNG or WebP. more than 2-3 times larger)

## Approximate comparison of QOI encoding speed vs PNG

https://user-images.githubusercontent.com/7782218/152955097-285f81dc-2b65-4f80-bd6a-890b7280d806.mp4

## Support me

Your support adds motivation to develop my public projects.

<a href="https://boosty.to/dmitriysalnikov/donate"><img src="/images/boosty.png" alt="Boosty" width=150px/></a>

<img src="/images/USDT-TRC20.png" alt="USDT-TRC20" width=150px/>

<b>USDT-TRC20 TEw934PrsffHsAn5M63SoHYRuZo984EF6v</b>

## Download

To download, use the [Godot Asset Library](https://godotengine.org/asset-library/asset/1619) or use one of the stable versions from the [GitHub Releases](https://github.com/DmitriySalnikov/godot_qoi/releases) page (just download the first archive from the assets, not one of the `Source Code`s).

> [!NOTE]
>
> If you download the addon manually, first go to the folder at the root of the archive, and then copy its contents to your project.

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
