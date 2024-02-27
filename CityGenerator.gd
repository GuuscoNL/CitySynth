
class_name CityGenerator
extends Node3D

@export var road_length := 5.0
@export var highway_branch_chance := 0.04
@export var seed := 63
@export var segment_limit := 1000

var city_gen_thread := Thread.new()
var S_mutex := Mutex.new()

var RNG := RandomNumberGenerator.new()

var Q := PriorityQueue.new()
var S: Array[RoadSegment] = [] # @SPEED: SOA?
var nodes: Array[RoadNode] # @SPEED: PackedVector2Array?

var degree_to_rad: float = PI / 180
var rad_to_degree: float = 180 / PI

@onready var multi_mesh_node := %MultiMeshNode
@onready var multi_mesh_road_segment := %MultiMeshRoadSegment

# Psuedocode
#initialize priority queue Q with a single entry: r(0, r0, q0)
#initialize segment list S to empty
#
#until Q is empty
	  #pop smallest r(ti, ri, qi) from Q
	  #accepted = localConstraints(&r)
	  #if (accepted) {
		#add segment(ri) to S
		#foreach r(tj, rj, qj) produced by globalGoals(ri, qi)
		  #add r(ti + 1 + tj, rj, qj) to Q
  #}

func add_node(pos: Vector2) -> RoadNode:
	var node := RoadNode.new(pos)
	nodes.append(node)
	return node

func reset_city() -> void:
	Q.reset()
	S.clear()
	nodes.clear()
	
	RNG.seed = seed

func generate_city() -> void:
	var start_time := Time.get_ticks_usec()
	var start_node := add_node(Vector2.ZERO)
	var next_node := add_node(Vector2(road_length, 0))
	
	Q.push(RoadSegment.new(0, start_node, next_node))
	
	while not Q.is_empty() and S.size() < segment_limit:
		var cur_road: RoadSegment = Q.pop()
		var accepted: bool = local_constraints(cur_road)
		if accepted:
			S_mutex.lock()
			S.append(cur_road)
			S_mutex.unlock()
			
			var new_roads := global_goals(cur_road)
			
			for new_road in new_roads:
				new_road.delay += cur_road.delay + 1
				Q.push(new_road)
	
	print("alghoritm done")

	multi_mesh_road_segment.multimesh.set_instance_count(S.size())
	var index := 0
	for segment in S:
		segment.validate_nodes()
		multi_mesh_road_segment.multimesh.set_instance_transform(
			index, 
			Transform3D(Basis().rotated(Vector3(0, 1, 0), segment.angle), 
			Vector3(segment.pos.x, multi_mesh_road_segment.multimesh.mesh.size.y/2, segment.pos.y))
			)
		index += 1

	multi_mesh_node.multimesh.set_instance_count(nodes.size())
	index = 0
	for i in range(nodes.size() - 1, 0, -1):
		var node := nodes[i]
		if not node.valid: 
			nodes.remove_at(i)
		else:
			multi_mesh_node.multimesh.set_instance_transform(
				index, 
				Transform3D(Basis(), 
				Vector3(node.pos.x, multi_mesh_node.multimesh.mesh.height/2, node.pos.y))
				)
			index += 1
	
	
	print("Time took: %s ms" % ( (float)(Time.get_ticks_usec() - start_time) / 1000))

func local_constraints(road: RoadSegment) -> bool:
	return true

func global_goals(root_road: RoadSegment) -> Array[RoadSegment]:
	var new_roads: Array[RoadSegment]
	var root_to_node := root_road.to_node
	
	var rand_angle := RNG.randf_range(-10, 10)
	
	var new_to_pos := calc_pos_with_angle(root_to_node.pos, (-root_road.angle * rad_to_degree) + rand_angle, road_length) # TODO: Sample from heatmap
	var new_to_node := add_node(new_to_pos)
	new_roads.append(RoadSegment.new(1, root_to_node, new_to_node))
	
	# Will highway branch to a new highway?
	if RNG.randf() < highway_branch_chance:
		var angle := 90
		if RNG.randf() < 0.5:
			angle = -90
		var branch_node := add_node(calc_pos_with_angle(root_to_node.pos, (-root_road.angle * rad_to_degree) + angle, road_length))
		new_roads.append(RoadSegment.new(1, root_to_node, branch_node))
		
	return new_roads

func calc_pos_with_angle(from: Vector2, angle: int, length: float) -> Vector2:
	var angle_rad := angle * degree_to_rad
	return Vector2(from.x + cos(angle_rad) * length, from.y + sin(angle_rad) * length)

func _on_button_generate_pressed() -> void:
	reset_city()
	city_gen_thread.start(generate_city, Thread.PRIORITY_HIGH)

func update_multimeshes() -> void:
	pass

func _process(delta: float) -> void:
	if city_gen_thread.is_started() and not city_gen_thread.is_alive():
		city_gen_thread.wait_to_finish()
		update_multimeshes()

