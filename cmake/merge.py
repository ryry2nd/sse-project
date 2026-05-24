#!/usr/bin/env python3
import json
import sys
import os

def load_json(path):
	with open(path, "r") as f:
		return json.load(f)

def main():
	if len(sys.argv) < 3:
		print("Usage: python merge.py output.json db1.json db2.json ...")
		sys.exit(1)

	OUTPUT_FILE = sys.argv[1]
	inputs = sys.argv[2:]

	for p in inputs:
		if not os.path.exists(p):
			print(f"Missing file: {p}")
			sys.exit(1)

	merged = []

	for path in inputs:
		data = load_json(path)

		if not isinstance(data, list):
			print(f"Skipping {path}: not a compile_commands.json list")
			continue

		merged.extend(data)

	seen = set()
	unique = []

	for entry in merged:
		key = (entry.get("directory"), entry.get("file"))
		if key in seen:
			continue
		seen.add(key)
		unique.append(entry)

	with open(OUTPUT_FILE, "w") as f:
		json.dump(unique, f, indent=2)

	print(f"Merged {len(inputs)} files -> {OUTPUT_FILE}")
	print(f"Total entries: {len(unique)}")

if __name__ == "__main__":
	main()