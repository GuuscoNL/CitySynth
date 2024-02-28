class_name RoadNode

var pos := Vector2.ZERO
var valid := false
var connected_roads: Array[RoadSegment]
var color := Color(255, 255, 255)

func _init(pos1: Vector2) -> void:
	pos = pos1

func add_road(road: RoadSegment) -> void:
	connected_roads.append(road)

func remove_road(road: RoadSegment) -> void:
	connected_roads.erase(road)
