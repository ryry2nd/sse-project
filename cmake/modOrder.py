#!/usr/bin/env python3
import sys
from typing import List, Dict, Set
from collections import defaultdict, deque

from parse import *


def topo_sort(modules: List[ModuleConfig]) -> List[ModuleConfig]:
	module_map: Dict[str, ModuleConfig] = {m.id: m for m in modules}

	# build graph
	graph = defaultdict(list)
	indegree = {m.id: 0 for m in modules}

	for m in modules:
		for dep in m.deps:
			if dep not in module_map:
				print(f"ERROR: missing dependency '{dep}' for module '{m.id}'")
				sys.exit(1)

			graph[dep].append(m.id)
			indegree[m.id] += 1

	# queue of ready nodes
	q = deque([mid for mid, deg in indegree.items() if deg == 0])

	result = []

	while q:
		current = q.popleft()
		result.append(module_map[current])

		for nxt in graph[current]:
			indegree[nxt] -= 1
			if indegree[nxt] == 0:
				q.append(nxt)

	# cycle detection
	if len(result) != len(modules):
		print("ERROR: cyclic dependency detected")
		sys.exit(1)

	return result


def main():
	if len(sys.argv) < 2:
		print("Usage: modOrder.py module1.txt module2.txt ...")
		sys.exit(1)

	inputs = sys.argv[1:]

	modules = []

	for p in inputs:
		if not os.path.exists(p):
			print(f"Missing file: {p}")
			sys.exit(1)

		modules.append(parse_config(p))

	ordered = topo_sort(modules)

	# output as CMake list
	print(";".join(m.path for m in ordered))


if __name__ == "__main__":
	main()