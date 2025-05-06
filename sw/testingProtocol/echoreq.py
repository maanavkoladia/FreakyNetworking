from scapy.all import Ether, IP, ICMP, raw
import random

outfile = "temp/echoreq.txt"

src_ip = "10.145.10.232"
dst_ip = "192.168.0.100"

# Build the IP layer separately if you want
ip_layer = IP(src=src_ip, dst=dst_ip, flags="DF", frag=0)

# Build the full packet
packet = (
    Ether(dst="D8:47:8F:87:18:3F", src="f8:b5:4d:64:81:29") /
    ip_layer /
    ICMP(type=8, code=0) /  # type=8 is Echo Request
    b"Ping request payload"
)

# Get raw bytes
packet_bytes = raw(packet)

# Write to file with offsets
with open(outfile, "w") as f:
    for i in range(0, len(packet_bytes), 16):
        chunk = packet_bytes[i:i+16]
        hex_bytes = ' '.join(f"{b:02x}" for b in chunk)
        f.write(f"{i:04x} {hex_bytes}\n")

print(f"Packet written to {outfile}")
