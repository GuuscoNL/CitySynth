
class_name CityGenerator
extends Node3D

@export var road_length := 5.0
@export var highway_branch_chance := 0.02
@export var highway_angle := 20.0
@export var rng_seed := 63
@export var segment_limit := 500
@export var minimum_road_length := 1.5
@export var close_crossing := 1.5
@export var close_road := 1.5

var city_gen_thread := Thread.new()
var S_mutex := Mutex.new()

var RNG := RandomNumberGenerator.new()

var Q := PriorityQueue.new()
var S: Array[RoadSegment] = [] # @SPEED: SOA?
var nodes: Array[RoadNode] = [] # @SPEED: PackedVector2Array?

@onready var multi_mesh_node: MultiMeshInstance3D = %MultiMeshNode
@onready var multi_mesh_road_segment: MultiMeshInstance3D = %MultiMeshRoadSegment
@onready var world_floor: MeshInstance3D = %Floor

## Result struct for roads_collide()
class RoadsCollidedResult:
	var collided: bool ## Did the road collide?
	var pos: Vector2 = Vector2.ZERO ## Position of the collision if it happenend
	
	func _init(collided1: bool, pos1: Vector2) -> void:
		collided = collided1
		pos = pos1

## Result struct for local_constraints()
class LocalConstraintResult:
	var accepted: bool ## Should the road be accepted?
	var should_continue: bool ## Should this  road create new roads?
	
	func _init(accepted1: bool, should_continue1: bool) -> void:
		accepted = accepted1
		should_continue = should_continue1

## Result struct for dist_node_to_road()
class DistNodeToRoadResult:
	var closest_point_pos: Vector2 ## Closest point on the road
	var distance_sqr: float ## Distance to the closest point
	
	func _init(closest_point1: Vector2, distance_sqr1: float) -> void:
		closest_point_pos = closest_point1
		distance_sqr = distance_sqr1

"""
Psuedocode
initialize priority queue Q with a single entry: r(0, r0, q0)
initialize segment list S to empty

until Q is empty
	  pop smallest r(ti, ri, qi) from Q
	  accepted = localConstraints(&r)
	  if (accepted) {
		add segment(ri) to S
		foreach r(tj, rj, qj) produced by globalGoals(ri, qi)
		  add r(ti + 1 + tj, rj, qj) to Q
  }
"""

func add_node(pos: Vector2) -> RoadNode:
	var node := RoadNode.new(pos)
	nodes.append(node)
	return node

func reset_city() -> void:
	Q.reset()
	S.clear()
	nodes.clear()
	
	RNG.seed = rng_seed

func generate_city() -> void:
	var start_time := Time.get_ticks_usec()
	var start_node := add_node(Vector2.ZERO)

	var r_node := add_node(Vector2(road_length, 0))
	var l_node := add_node(Vector2(-road_length, 0))
	
	Q.push(RoadSegment.new(0, start_node, r_node))
	Q.push(RoadSegment.new(0, start_node, l_node))
	
	while not Q.is_empty() and S.size() < segment_limit:
		var cur_road: RoadSegment = Q.pop()
		var result: LocalConstraintResult = local_constraints(cur_road)
		if result.accepted:
			S_mutex.lock()
			S.append(cur_road)
			S_mutex.unlock()
			
			if not result.should_continue:
				continue
			
			var new_roads := global_goals(cur_road)
			
			for new_road in new_roads:
				new_road.delay += cur_road.delay + 1
				Q.push(new_road)
	
	print("alghoritm done")
	
	# Cleanup: Remove roads that were not accepted and remove the road from the nodes
	for segment: RoadSegment in Q.list:
		segment.to_node.remove_road(segment)
		segment.from_node.remove_road(segment)
	Q.reset()

	multi_mesh_road_segment.multimesh.set_instance_count(S.size())
	var index := 0
	for segment in S:
		var length := segment.from_node.pos.distance_to(segment.to_node.pos)
		multi_mesh_road_segment.multimesh.set_instance_transform(
			index, 
			Transform3D(Basis().scaled(Vector3(length, 1, 1)).rotated(Vector3(0, 1, 0), segment.angle), 
			Vector3(segment.pos.x, multi_mesh_road_segment.multimesh.mesh.size.y/2, segment.pos.y))
			)

		multi_mesh_road_segment.multimesh.set_instance_color(index, segment.color.clamp())
		
		index += 1

	multi_mesh_node.multimesh.set_instance_count(nodes.size())
	index = 0
	for i in range(nodes.size() - 1, -1, -1):
		var node := nodes[i]
		
		if node.connected_roads.size() == 0: 
			nodes.remove_at(i)
			continue

		multi_mesh_node.multimesh.set_instance_transform(
			index, 
			Transform3D(Basis(), 
			Vector3(node.pos.x, multi_mesh_node.multimesh.mesh.height/2, node.pos.y))
			)

		multi_mesh_node.multimesh.set_instance_color(index, node.color.clamp())
		index += 1
	
	
	print("Time took: %s ms" % ( (float)(Time.get_ticks_usec() - start_time) / 1000))
	print("Segment count: %s" % S.size())

func local_constraints(org_road: RoadSegment) -> LocalConstraintResult:
	var floor_size: Vector2 = world_floor.mesh.size
	var to_pos := org_road.to_node.pos
	
	if (to_pos.x >=  floor_size.x / 2 or 
		to_pos.x <= -floor_size.x / 2 or 
		to_pos.y >=  floor_size.y / 2 or 
		to_pos.y <= -floor_size.y / 2):
		print("REJECTED: OUT OF BOUNDS")
		return LocalConstraintResult.new(false, false)
	
	if local_constraint_intersecting(org_road):
		return LocalConstraintResult.new(true, false)
	
	if local_constraint_close_node(org_road):
		return LocalConstraintResult.new(true, false)

	if local_constraint_close_road(org_road):
		return LocalConstraintResult.new(true, false)
	
	return LocalConstraintResult.new(true, true)

func local_constraint_intersecting(org_road: RoadSegment) -> bool:
	var connected_roads := org_road.get_connected_roads()
	
	# @SPEED: QuadTree
	for road in S:
		# Don't check roads that are connected to this road, since they will always be intersecting.
		if road in connected_roads:
			continue
			
		var collided := roads_collide(org_road, road)
		
		if collided.collided:
			var intersection_node := add_intersection(road, org_road, collided.pos)
			intersection_node.color = Color(0, 255, 0)
			org_road.color = Color(0, 255, 0)
			return true
		
	return false

func local_constraint_close_node(org_road: RoadSegment) -> bool:
	
	var closest_node: RoadNode = null

	var to_node_pos := org_road.to_node.pos
	var smallest_distance := 9999999999.0

	# @SPEED: QuadTree
	for node in nodes:
		
		if node == org_road.to_node:
			continue
		
		var distance := to_node_pos.distance_squared_to(node.pos)
		if distance < smallest_distance:
			smallest_distance = distance
			closest_node = node
	
	if smallest_distance < close_crossing ** 2 and closest_node != null:
		org_road.to_node = closest_node
		closest_node.color = Color(255, 0, 0)
		org_road.color = Color(255, 0, 0)
		return true
	
	return false

func local_constraint_close_road(org_road: RoadSegment) -> bool:

	var smallest_distance := 9999999999.0
	var closest_road: RoadSegment = null
	var closest_node_pos: Vector2 = Vector2.ZERO

	# @SPEED: QuadTree
	for road in S:
		
		if road == org_road:
			continue
		
		var result := dist_node_to_road(road, org_road.to_node)

		if result.distance_sqr < smallest_distance:
			smallest_distance = result.distance_sqr
			closest_road = road
			closest_node_pos = result.closest_point_pos
		
		
	if smallest_distance < close_road ** 2 and closest_road != null:
		var intersection_node := add_intersection(closest_road, org_road, closest_node_pos)
		intersection_node.color = Color(0, 0, 255)
		org_road.color = Color(0, 0, 255)
		return true
	
	return false

func dist_node_to_road(org_road: RoadSegment, node: RoadNode) -> DistNodeToRoadResult:
	var a := org_road.from_node.pos
	var b := org_road.to_node.pos
	var p := node.pos

	var ab := b - a
	var ap := p - a

	var d := ap.dot(ab) / ab.length_squared()

	var result := DistNodeToRoadResult.new(Vector2.ZERO, 0)

	# If the point is outside the line segment, the closest point is one of the end points
	if d < 0:
		result.closest_point_pos = a
	elif d > 1:
		result.closest_point_pos = b
	
	# Otherwise, the closest point is on the line segment
	else:
		result.closest_point_pos = a + ab * d
	

	result.distance_sqr = p.distance_squared_to(result.closest_point_pos)

	return result

func roads_collide(road1: RoadSegment, road2: RoadSegment) -> RoadsCollidedResult:
	var p1 := road1.from_node.pos
	var p2 := road1.to_node.pos
	var p3 := road2.from_node.pos
	var p4 := road2.to_node.pos
	
	var dir1 := p2 - p1
	var dir2 := p4 - p3

	# Calculate the cross product of the direction vectors
	var cross := dir1.cross(dir2)

	# Check if the lines are parallel (cross product is zero)
	if cross == 0:
		return RoadsCollidedResult.new(false, Vector2.ZERO)

	# Calculate the parameter values for the lines
	var t1 := ((p3.x - p1.x) * dir2.y - (p3.y - p1.y) * dir2.x) / cross
	var t2 := ((p3.x - p1.x) * dir1.y - (p3.y - p1.y) * dir1.x) / cross

	# Check if the parameter values are within the range [0, 1]
	if t1 >= 0 and t1 <= 1 and t2 >= 0 and t2 <= 1:
		var intersection := Vector2(p1.x + t1 * dir1.x, p1.y + t1 * dir1.y)
		return RoadsCollidedResult.new(true, intersection)
	else:
		return RoadsCollidedResult.new(false, Vector2.ZERO)

func add_intersection(road_to_split: RoadSegment, road_to_add: RoadSegment, intersection_pos: Vector2) -> RoadNode:
	var split_from_node := road_to_split.from_node
	var split_to_node := road_to_split.to_node
	
	if split_from_node.pos.distance_squared_to(intersection_pos) < minimum_road_length ** 2:
		road_to_add.to_node = split_from_node
		
		return split_from_node
	
	if split_to_node.pos.distance_squared_to(intersection_pos) < minimum_road_length ** 2:
		road_to_add.to_node = split_to_node
		
		return split_to_node
	
	var intersection_node := add_node(intersection_pos)
	
	# TODO: roadTypes
	road_to_add.to_node = intersection_node
	
	S.append(RoadSegment.new(1, intersection_node, split_to_node))
	road_to_split.to_node = intersection_node
	
	return intersection_node

func global_goals(root_road: RoadSegment) -> Array[RoadSegment]:
	var new_roads: Array[RoadSegment] = []
	var root_to_node := root_road.to_node
	
	var rand_angle := RNG.randf_range(-highway_angle, highway_angle)
	
	var new_to_pos := calc_pos_with_angle(root_to_node.pos, (rad_to_degree(-root_road.angle)) + rand_angle, road_length) # TODO: Sample from heatmap
	var new_to_node := add_node(new_to_pos)
	new_roads.append(RoadSegment.new(1, root_to_node, new_to_node))
	
	# Will highway branch to a new highway?
	if RNG.randf() < highway_branch_chance:
		var angle := 90.0
		if RNG.randf() < 0.5:
			angle = -90.0
		var branch_node := add_node(calc_pos_with_angle(root_to_node.pos, (rad_to_degree(-root_road.angle)) + angle, road_length))
		new_roads.append(RoadSegment.new(1, root_to_node, branch_node))
		
	return new_roads

func calc_pos_with_angle(from: Vector2, angle: float, length: float) -> Vector2:
	var angle_rad := degree_to_rad(angle)
	return Vector2(from.x + cos(angle_rad) * length, from.y + sin(angle_rad) * length)

func _on_button_generate_pressed() -> void:
	reset_city()
	if city_gen_thread.is_started():
		print("WARNING: City Gen Thread already started")
	else:
		city_gen_thread.start(generate_city, Thread.PRIORITY_HIGH)

func update_multimeshes() -> void:
	pass

func _process(_delta: float) -> void:
	if city_gen_thread.is_started() and not city_gen_thread.is_alive():
		city_gen_thread.wait_to_finish()
		update_multimeshes()

## Used to convert degrees to radians
func degree_to_rad(x: float) -> float:
	return x * PI / 180

##Used to convert radians to degrees
func rad_to_degree(x: float) -> float:
	return x * 180 / PI
