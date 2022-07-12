tool
extends ResourceFormatLoader
class_name QOILoader

var qoi = preload("qoi.gdns").new()
var qoi_utils = preload("qoi_utils.gdns").new()


func _init() -> void:
	# Disable error printing by changing the property to False
	qoi.print_errors = true


func get_recognized_extensions() -> PoolStringArray:
	return PoolStringArray(["qoi", "qoi_import"])

func get_resource_type(path: String) -> String:
	return "ImageTexture"

func handles_type(typename: String) -> bool:
	return typename == "ImageTexture"

func load(path: String, original_path: String):
	var tex = ImageTexture.new()
	var file_size = 0
	var flags = Texture.FLAGS_DEFAULT
	var is_webp = false
	
	#var total_elapsed_time = OS.get_ticks_usec()
	
	##################
	# Check QOI config at the end of file
	if true:
		var file = File.new()
		var err = file.open(path, File.READ)
		if err:
			return err
		
		# footer:
		# qcfg - magic
		# -- data --
		# 8-bit the size of the cfg data in bytes, including these 8 bits
		# 8-bit flags
		# -- data --
		# 64-bit pos of magic
		
		file.seek_end(-8)
		file_size = file.get_64()
		file.seek(file_size)
		
		var magic = file.get_buffer(4)
		if magic[0] == qoi_utils.QOI_CFG_MAGIC[0] && magic[1] == qoi_utils.QOI_CFG_MAGIC[1] \
			&& magic[2] == qoi_utils.QOI_CFG_MAGIC[2] && magic[3] == qoi_utils.QOI_CFG_MAGIC[3]:
				if file.get_8() == 2: # is cfg data size is equal to current version..
					flags = file.get_8()
				else:
					printerr("Incorrect QOI config was found in the .import'ed file.")
		else:
			# just ignore this and use defaults
			pass
		file.close()
	
	##################
	
	var q : Image = qoi.read(path)
	if q.is_empty():
		printerr("The Image was not loaded correctly")
		return tex
	
	if flags & Texture.FLAG_MIPMAPS:
		q.generate_mipmaps()
	
	tex.create_from_image(q, flags)
	
	#print("Total time: %d ms" % [(OS.get_ticks_usec() - total_elapsed_time) / 1000.0] )
	
	return tex
