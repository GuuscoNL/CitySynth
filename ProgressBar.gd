extends ProgressBar

@onready var city_gen := %CityGenerator

@export var update_frequency := 100 ##ms

var last_updated := 0

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	
	if Time.get_ticks_msec() - last_updated > update_frequency and city_gen.city_gen_thread.is_started():
		if city_gen.S_mutex.try_lock():
			last_updated = Time.get_ticks_msec()
			value = ((float)(city_gen.S.size()) / city_gen.segment_limit)

			city_gen.S_mutex.unlock()
	elif not city_gen.city_gen_thread.is_started():
		value = 1
