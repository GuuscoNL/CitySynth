class_name RoadSegment

var delay: int
var from_node: RoadNode:
	set(val):
		from_node = val
		calc_pos_angle()
var to_node: RoadNode:
	set(val):
		to_node = val
		calc_pos_angle()
var angle: float
var pos: Vector2

func _init(delay1: int, from: RoadNode, to: RoadNode) -> void:
	from_node = from
	to_node = to
	delay = delay1

func calc_pos_angle() -> void:
	if not from_node or not to_node: return
	pos = (from_node.pos + to_node.pos) / 2
	angle = -from_node.pos.angle_to_point(to_node.pos)

func validate_nodes() -> void:
	from_node.valid = true
	to_node.valid = true
