@tool
extends Control

const img_dir = "res://testsuite/images/"
@export var run_tests = false
@export var run_benchmark = true
@export_range(0, 256) var bench_runs := 5
@export_range(1, 256) var frames_to_render := 99
@export_range(1, 8192) var image_width := 1920
@export_range(1, 8192) var image_height := 1080
@export var render_test_images : bool = false:
	set = set_render

var image_to_test_save := Image.create_from_data(1, 1, false,Image.FORMAT_RGB8, [128, 1, 1])
var tex_to_test_save := ImageTexture.create_from_image(Image.create_from_data(1, 1, false, Image.FORMAT_RGB8, [1, 128, 1]))

@onready var grid := $GridContainer


func _ready() -> void:
	if not Engine.is_editor_hint():
		$ViewportContainer.queue_free()
		test_api()
		if run_benchmark:
			await get_tree().process_frame
			start_bench()


func test_api():
	@warning_ignore(return_value_discarded)
	QOI.write("user://example.qoi", load("res://icon.svg").get_image())
	var img = QOI.read("user://example.qoi")
	var enc = QOI.encode(img)
	var dec = QOI.decode(enc)
	
	var tex: = ImageTexture.create_from_image(dec)
	$TextureRect.texture = tex
	
	# More tests
	
	if !run_tests:
		return
	
	if tex.get_image().is_empty():
		printerr("Tests failed. Example is not working...")
		return
	
	#################
	# prepare
	var test_dir = "user://tests/"
	#var locked_file = "locked.qoi"
	var good_qoi = "good.qoi"
	var broken_qoi = "broken.qoi"
	var good_data = PackedByteArray()
	var good_image : Image = load("res://icon.svg").get_image()
	var good_tex : ImageTexture = ImageTexture.new()
	
	if DirAccess.dir_exists_absolute(test_dir):
		var files_to_delete = DirAccess.get_files_at(test_dir)
		for f in files_to_delete:
			@warning_ignore(return_value_discarded)
			DirAccess.remove_absolute(test_dir.path_join(f))
	
	@warning_ignore(return_value_discarded)
	DirAccess.make_dir_recursive_absolute(test_dir)
	
	@warning_ignore(return_value_discarded)
	QOI.write(test_dir.path_join(good_qoi), good_image)
	var file = FileAccess.open(test_dir.path_join(broken_qoi), FileAccess.WRITE)
	file.store_buffer("definitely broken qoi data...".to_utf8_buffer())
	file = null
	
	file = FileAccess.open(test_dir.path_join(good_qoi), FileAccess.READ)
	good_data = file.get_buffer(file.get_length())
	file = null
	good_tex.set_image(good_image)
	
	#################
	### qoi_wrapper
	
	# read/decode
	assert(!QOI.read(test_dir.path_join("not existing file.qoi")), "Image must be empty when loading not existing file.")
	assert(!QOI.read(test_dir.path_join(broken_qoi)), "Image must be empty when loading broken qoi file.")
	assert(!QOI.decode(PackedByteArray()), "Image must be empty when loading from empty array.")
	assert(QOI.read(test_dir.path_join(good_qoi)), "Image must be loaded correctly from correct qoi file.")
	assert(QOI.decode(good_data), "Image must be loaded correctly from correct qoi data.")
	
	# write/encode
	
	assert(QOI.write("not matter", null) == ERR_INVALID_PARAMETER, "Write null Image can't be performed.")
	assert(QOI.write("not matter", Image.new()) == ERR_INVALID_PARAMETER, "Write empty Image can't be performed.")
	
	assert(QOI.encode(null).size() == 0, "Encode null Image can't be performed.")
	assert(QOI.encode(Image.new()).size() == 0, "Encode empty Image can't be performed.")
	
	if !OS.has_feature("mobile"):
		var unsupported_image : Image = good_image.duplicate()
		@warning_ignore(return_value_discarded)
		unsupported_image.compress(Image.COMPRESS_S3TC, Image.COMPRESS_SOURCE_GENERIC, 0.5)
		assert(QOI.write(test_dir.path_join("unsupported format.qoi"), unsupported_image) != OK, "Write can't be performed with usupported Image format.")
	
	assert(QOI.encode(good_image).size() != 0, "Encode correct Image must be performed.")
	
	#################
	### qoi_save
	
	@warning_ignore(return_value_discarded)
	ResourceSaver.save(image_to_test_save, test_dir.path_join("1x1_img.qoi"))
	@warning_ignore(return_value_discarded)
	ResourceSaver.save(tex_to_test_save, test_dir.path_join("1x1_tex.qoi"))
	assert(QOI.read(test_dir.path_join("1x1_img.qoi")), "Image must be saved to QOI via ResourceSaver and loaded correctly.")
	assert(QOI.read(test_dir.path_join("1x1_tex.qoi")), "Texture must be saved to QOI via ResourceSaver and loaded correctly.")


func start_bench():
	var result = {}
	var dir = img_dir.path_join("%dx%d_%d" % [image_width, image_height, frames_to_render])
	
	for ext in ["png", "qoi"]:
		result[ext] = []
		
		for i in bench_runs:
			var start_time = Time.get_ticks_usec()
			fill_grid(dir, ext)
			var time = Time.get_ticks_usec() - start_time
			result[ext].append(time)
			print("Run: %d, Ext: %s, %.3f ms" % [i, ext, time / 1000.0])
			
			# show images for 1 frame
			await get_tree().process_frame
			# Clear
			var ch = grid.get_children()
			for c in ch:
				c.queue_free()
			# wait 1 sec to make sure the resources have been released
			await get_tree().create_timer(1).timeout
	
	# textures can be imported as png or webp or as vram compressed
	var is_vram = false
	var vram_format = ""
	if ProjectSettings.has_setting("importer_defaults/texture"):
		is_vram = ProjectSettings.get_setting("importer_defaults/texture")["compress/mode"] == 2
		if is_vram:
			if FileAccess.file_exists(dir.path_join("0.png")):
				var f = load(dir.path_join("0.png")).get_data().get_format()
				vram_format = f
				
				if f in [Image.FORMAT_DXT1, Image.FORMAT_DXT3, Image.FORMAT_DXT5]:
					vram_format = "s3tc"
				elif f in [Image.FORMAT_BPTC_RGBA, Image.FORMAT_BPTC_RGBF, Image.FORMAT_BPTC_RGBFU]:
					vram_format = "bptc"
				elif OS.has_feature("Android"):
					vram_format = "etc2"
				elif OS.has_feature("iOS"):
					vram_format = "pvrtc"
	
	var is_webp
	var is_lossy
	if ProjectSettings.has_setting("rendering/misc/lossless_compression/force_png"):
		is_lossy = ProjectSettings.has_setting("importer_defaults/texture") && ProjectSettings.get_setting("importer_defaults/texture")["compress/mode"] == 1
		is_webp = !ProjectSettings.get_setting("rendering/misc/lossless_compression/force_png") || is_lossy
	
	if is_vram:
		print("Note 'importer_defaults/texture[compress/mode]' is not equal to 0. PNG was imported as VRAM compressed texture inside .import folder")
	else:
		if is_webp:
			print("Note 'rendering/misc/lossless_compression/force_png' is off or 'importer_defaults/texture[compress/mode]' is lossy. PNG was imported as WebP inside .import folder")
	
	print("Platform: %s" % OS.get_name())
	print("Avg for %d runs, with %dx%d %d frames" % [bench_runs, image_width, image_height, frames_to_render])
	for ext in ["png", "qoi"]:
		var sum = 0
		for t in result[ext]:
			sum += t
		var e = ext if ext != "png" else (vram_format if is_vram else (("webp lossy" if is_lossy else "webp") if is_webp else "png" ))
		print("%s:\t%.3f ms" % [e, (sum / float(result[ext].size())) / 1000.0])


func render_images():
	var fin_dir = img_dir.path_join("%dx%d_%d" % [image_width, image_height, frames_to_render])
	@warning_ignore(return_value_discarded)
	DirAccess.make_dir_recursive_absolute(fin_dir)
	
	$ViewportContainer/Viewport.size = Vector2(image_width, image_height)
	$ViewportContainer/Viewport/TextureRect.size = Vector2(image_width, image_height)
	
	$ViewportContainer/Viewport/AnimationPlayer.stop()
	await get_tree().process_frame
	await get_tree().process_frame
	
	var tex : ViewportTexture = $ViewportContainer/Viewport.get_texture()
	$ViewportContainer/Viewport/AnimationPlayer.play("Noise")
	
	print("Hold ESC to cancel rendering")
	for i in frames_to_render:
		# save png and qoi for tests
		var format = "%d.%s"
		@warning_ignore(return_value_discarded)
		tex.get_image().save_png(fin_dir.path_join(format % [i, "png"]))
		@warning_ignore(return_value_discarded)
		QOI.write(fin_dir.path_join(format % [i, "qoi"]), tex.get_image())
		await get_tree().process_frame
		print("%d/%d" % [i+1, frames_to_render])
		
		if Input.is_key_pressed(KEY_ESCAPE):
			break
	
	$ViewportContainer/Viewport.size = Vector2(128, 128)
	$ViewportContainer/Viewport/TextureRect.size = Vector2(128, 128)
	$ViewportContainer/Viewport/AnimationPlayer.stop()


func fill_grid(path, ext):
	var ch = grid.get_children()
	for c in ch:
		c.queue_free()
	
	var total_found = 0
	
	var dir = DirAccess.open(path)
	if dir:
		@warning_ignore(return_value_discarded)
		dir.list_dir_begin()
		var file_name = dir.get_next()
		while file_name != "":
			if not dir.current_is_dir():
				if file_name.ends_with(ext + ".import" if OS.has_feature("standalone") else ext):
					var tmp = TextureRect.new()
					tmp.texture = load(path.path_join(file_name.replace(".import", "")))
					tmp.ignore_texture_size = true
					tmp.size_flags_horizontal = SIZE_EXPAND_FILL
					tmp.size_flags_vertical = SIZE_EXPAND_FILL
					grid.add_child(tmp)
					tmp.owner = self
					total_found += 1
			file_name = dir.get_next()
		
		grid.columns = int(sqrt(nearest_po2(total_found)))


func set_render(_val):
	if _val && Engine.is_editor_hint():
		render_images()
