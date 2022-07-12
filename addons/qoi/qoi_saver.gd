tool
extends ResourceFormatSaver
class_name QOISaver

var qoi = preload("qoi.gdns").new()
var qoi_utils = preload("qoi_utils.gdns").new()


func _init() -> void:
	# Disable error printing by changing the property to False
	qoi.print_errors = true


func get_recognized_extensions(resource: Resource) -> PoolStringArray:
	return PoolStringArray(["qoi"])

func recognize(resource: Resource) -> bool:
	return resource is Texture and resource.get_data() and !resource.get_data().is_empty()

func save(path: String, resource: Resource, _flags: int) -> int:
	var tex : Texture = resource
	if !tex:
		printerr("Texture resource is empty")
		return ERR_INVALID_DATA
	
	var img : Image = resource.get_data()
	if img.is_empty():
		printerr("Image resource is empty")
		return ERR_INVALID_DATA
	
	
	var err = qoi.write(path, img)
	if err:
		printerr("Can't save QOI image")
		return err
	
	var options = { qoi_utils.QOI_IMPORT_FLAGS: tex.flags }
	err = qoi_utils.add_footer(path, options)
	
	# Fix .import file options
	var file := File.new()
	file.open("res://.import/%s-%s" % [path.get_file(), path.md5_text()] + ".qoi_save_as", File.WRITE)
	file.store_var(options)
	file.close()
	
	if tex.get_class() != "ImageTexture":
		push_warning("Please reload the project to properly import and update the newly created QOI file.")
	
	return err
