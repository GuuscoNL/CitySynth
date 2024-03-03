class_name PriorityQueue

var list : Array[RoadSegment] = []

func push(element: RoadSegment) -> void:
	# @speed!!!!
	for i in range(list.size()):
		if list[i].delay < element.delay:
			list.insert(i, element)
			return

	list.push_back(element)

func pop() -> RoadSegment:
	return list.pop_back()

func is_empty() -> bool:
	return list.is_empty()

func print_queue() -> void:
	for element: RoadSegment in list:
		print(element.delay)

func reset() -> void:
	list.clear()
