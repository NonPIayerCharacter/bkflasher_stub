#!/usr/bin/env python3
import argparse
import gzip
from pathlib import Path

import zopfli.gzip


def main():
    parser = argparse.ArgumentParser(description="Optimize the published gzip stub files.")
    parser.add_argument("directory", type=Path)
    args = parser.parse_args()

    for directory in (args.directory, args.directory / "minimal"):
        images = sorted(directory.glob("*_Stub.bin"))
        if not images:
            raise SystemExit(f"No stub images in {directory}")
        for image in images:
            original = image.read_bytes()
            payload = gzip.decompress(original)
            packed = zopfli.gzip.compress(payload)
            if gzip.decompress(packed) != payload:
                raise SystemExit(f"Repacked payload differs for {image}")
            if len(packed) < len(original):
                image.write_bytes(packed)
            print(f"{image}: {len(original)} -> {min(len(original), len(packed))} bytes")


if __name__ == "__main__":
    main()
