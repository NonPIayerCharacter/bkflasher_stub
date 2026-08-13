#!/usr/bin/env python3
import argparse, os, time, zlib

def _int_to_bytes(v: int, l: int) -> bytes:
	return v.to_bytes(l, byteorder="big")

def build_header(data_size: int, start_addr: int, data_crc: int) -> bytearray:
	hdr = bytearray(0x40)
	hdr[0:4] = _int_to_bytes(0x02262018, 4)
	hdr[8:12] = _int_to_bytes(int(time.time()), 4)
	hdr[12:16] = _int_to_bytes(data_size, 4)
	hdr[16:20] = _int_to_bytes(0x40, 4)
	hdr[20:24] = _int_to_bytes(start_addr, 4)
	hdr[24:28] = _int_to_bytes(data_crc, 4)
	hdr[28:32] = _int_to_bytes(0x18140F00, 4)
	hdr[32:32 + len(b"boot-version-debug")] = b"boot-version-debug"
	crc_header = zlib.crc32(hdr) & 0xffffffff
	hdr[4:8] = _int_to_bytes(crc_header, 4)
	return hdr

def compute_payload_crc(path: str) -> int:
	with open(path, "rb") as f:
		data = f.read()
	return zlib.crc32(data) & 0xffffffff

def main() -> None:
	parser = argparse.ArgumentParser(description="Generate a TR6260 boot header.")
	parser.add_argument("payload", metavar="PAYLOAD")
	parser.add_argument("--addr", type=lambda v: int(v, 0), default=0x0001C000, help="Start address. Defaults to 0x1C000.")
	parser.add_argument("-o", "--output", metavar="FILE", help=("Write header+payload to a file, header hex dump is printed if not specified."))
	args = parser.parse_args()

	payload_crc = compute_payload_crc(args.payload)
	data_size = os.path.getsize(args.payload)
	hdr = build_header(data_size, args.addr, payload_crc)

	if args.output:
		with open(args.output, "wb") as f:
			f.write(hdr)
			with open(args.payload, "rb") as f1:
				f.write(f1.read())
		print(f"Written to {args.output}")
	else:
		print(" ".join(f"{b:02x}" for b in hdr))

if __name__ == "__main__":
	main()
