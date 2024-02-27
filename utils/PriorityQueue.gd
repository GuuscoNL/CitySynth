class_name PriorityQueue

var list : Array[Variant] = []

func push(element: Variant) -> void:
	# @speed!!!!
	for i in range(list.size()):
		if list[i].delay > element.delay:
			list.insert(i, element)
			return

	list.append(element)

func pop() -> Variant:
	return list.pop_front()

func is_empty() -> bool:
	return list.is_empty()

func print_queue() -> void:
	for element: Variant in list:
		print(element.delay)

func reset() -> void:
	list.clear()
