#!/usr/bin/env python3

from dataclasses import dataclass
from typing import List, Dict, Set
import os


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