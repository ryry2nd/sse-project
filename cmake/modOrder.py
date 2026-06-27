#!/usr/bin/env python3
import sys
import os
from dataclasses import dataclass
from typing import List, Dict, Set
from collections import defaultdict, deque


@dataclass
class ModuleConfig:
	path: str = ""
	id: str = ""
	short_name: str = ""
	version: str = ""
	deps: List[str] = None


def parse_config(path: str):
    cfg = ModuleConfig(path=os.path.dirname(path), deps=[])

    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()

            if not line or line.startswith("#"):
                continue

            if "=" not in line:
                continue

            key, value = line.split("=", 1)
            key = key.strip()
            value = value.strip()

            if key == "ID":
                cfg.id = value

            elif key == "SHORT_NAME":
                cfg.short_name = value

            elif key == "VERSION":
                cfg.version = value

            elif key == "DEPS":
                cfg.deps = [d for d in value.split(";") if d]

    return cfg


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