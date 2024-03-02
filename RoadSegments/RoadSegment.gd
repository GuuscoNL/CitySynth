class_name RoadSegment

var delay: int
var from_node: RoadNode:
	set(val):
		if from_node: from_node.remove_road(self)
		
		from_node = val
		from_node.add_road(self)
		calc_pos_angle()
var to_node: RoadNode:
	set(val):
		if to_node: to_node.remove_road(self)
		
		to_node = val
		to_node.add_road(self)
		calc_pos_angle()
var angle: float
var pos: Vector2
var color:= Color(255, 255, 255)

func _init(delay1: int, from: RoadNode, to: RoadNode) -> void:
	from_node = from
	to_node = to
	delay = delay1

func calc_pos_angle() -> void:
	if not from_node or not to_node: return
	pos = (from_node.pos + to_node.pos) / 2
	angle = -from_node.pos.angle_to_point(to_node.pos)

func get_connected_roads() -> Array[RoadSegment]:
	var connected_roads:= from_node.connected_roads
	connected_roads.append_array(to_node.connected_roads)
	return connected_roads.filter(func(element: RoadSegment) -> bool: return element != self)

func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE:
		from_node.remove_road(self)
		to_node.remove_road(self)
