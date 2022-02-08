<img src="https://github.com/DmitriySalnikov/godot_qoi/blob/5bd25a2dc2ea907041b4c9a7f4ae12bc0ae19a94/icon.png" width=128/>

# godot_qoi

This is a QOI ([Quite OK Image Format](https://github.com/phoboslab/qoi)) wrapper for Godot Engine. This addon will allow you to read, write, encode and decode images to or from the QOI format.

This is a GDNative library. Includes precompiled binaries for `Windows` and `Linux`, but it must compile for each platform which Godot Engine supports.

## Approximate comparison of QOI encoding speed vs PNG

https://user-images.githubusercontent.com/7782218/152955097-285f81dc-2b65-4f80-bd6a-890b7280d806.mp4

## Support

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/I2I53VZ2D)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://paypal.me/dmitriysalnikov)

## Download

To download, use the [Godot Asset Library](https://godotengine.org/asset-library/asset/5159) or download the archive by clicking the button at the top of the main repository page: `Code -> Download ZIP`, then unzip it to your project folder.

## API

```gdscript
# Enable or disable error printing
# type bool, default true
print_errors

# Read the QOI image from file
# @return null or image
Image read(path : String)

# Decode the QOI image from an encoded byte array
# @return null or image
Image decode(data : PoolByteArray)

# Write the Image as a file in QOI format
# @return code of error
int write(path : String, img : Image)

# Encode the Image in QOI format as an array of bytes
# @return an array of bytes. If an error occurs, it is empty.
PoolByteArray encode(img : Image)
```

### Example

```gdscript
func example():
    var qoi = load("res://addons/qoi/qoi.gdns").new()
    qoi.print_errors = true

    qoi.write("res://example.qoi", load("res://icon.png").get_data())
    var img = qoi.read("res://example.qoi")
    var enc = qoi.encode(img)
    var dec = qoi.decode(enc)

    var tex: = ImageTexture.new()
    tex.create_from_image(dec)
    $TextureRect.texture = tex
```

## Why is there no possibility to use images inside the editor???

1. I didn't find the right way to load images. Every time a scene with QOI image was opened, an error about missing resources appeared.
2. In Godot, there is no way to import a custom image as `.stex` to the `.import` folder.

I know how to do this in a regular module for Godot Engine, but not in GDNative or GDScript.

But you can create a pull request with a solution to these problems.

## License

MIT license
