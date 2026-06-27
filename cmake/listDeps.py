#!/usr/bin/env python3
import sys, os
from parse import *


def main():
	if len(sys.argv) < 2:
		print("Usage: listDeps.py getDeps.txt module1.txt module2.txt ...")
		sys.exit(1)

	in1 = sys.argv[1]
	inputs = sys.argv[2:]

	if not os.path.exists(in1):
		print(f"Missing file: {in1}")
		sys.exit(1)

	module1 = parse_config(in1)

	modules = []

	for p in inputs:
		mod = parse_config(p)
		if (mod.id in module1.deps):
			modules.append(mod.path)

	print(";".join(m for m in modules))


if __name__ == "__main__":
	main()