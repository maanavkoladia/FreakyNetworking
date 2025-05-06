from scapy.all import Ether, IP, UDP, BOOTP, DHCP, Raw, wrpcap, rdpcap, mac2str, raw
import random
import os

def random_mac():
    return ":".join(f"{random.randint(0, 255):02x}" for _ in range(6))

def write_hex_format(packet_bytes, filename):
    with open(filename, "w") as f:
        for i in range(0, len(packet_bytes), 16):
            chunk = packet_bytes[i:i+16]
            hex_bytes = ' '.join(f"{b:02x}" for b in chunk)
            f.write(f"{i:06x}  {hex_bytes}\n")

def generate_random_frame():
    src_mac = random_mac()
    dst_mac = "D8:47:8F:87:18:3F"
    ether_type = 0x0800  # IPv4

    payload_size = 1000
    payload = bytes(random.getrandbits(8) for _ in range(payload_size))

    src_ip = f"192.168.{random.randint(0, 255)}.{random.randint(0, 255)}"
    dst_ip = "192.168.0.100"
    src_port = random.randint(1024, 65535)
    dst_port = random.randint(1024, 65535)

    udp_layer = UDP(sport=src_port, dport=dst_port)
    ip_layer = IP(src=src_ip, dst=dst_ip, flags="DF", frag=0)

    packet = Ether(src=src_mac, dst=dst_mac, type=ether_type) / ip_layer / udp_layer / Raw(payload)
    
    os.makedirs("temp", exist_ok=True)
    wrpcap("temp/inbytes.pcap", [packet])
    write_hex_format(raw(packet), "temp/inbytes.txt")

    print("Wrote random Ethernet frame to 'temp/inbytes.pcap' and 'temp/inbytes.txt'.")

def dhcp_disc_offer():
    disc_pkt = rdpcap("temp/dhcp_disc.pcap")[0]

    client_mac = disc_pkt[Ether].src
    transaction_id = disc_pkt[BOOTP].xid

    offer_ip = "192.168.1.100"
    server_ip = "192.168.1.1"

    ether = Ether(dst=client_mac, src="ab:cd:12:34:56")
    ip = IP(src=server_ip, dst=offer_ip, flags="DF", frag=0)
    udp = UDP(sport=67, dport=68)
    bootp = BOOTP(
        op=2,
        yiaddr=offer_ip,
        siaddr=server_ip,
        chaddr=mac2str(client_mac),
        xid=transaction_id,
        flags=0x8000
    )
    dhcp = DHCP(options=[
        ("message-type", "offer"),
        ("server_id", server_ip),
        ("lease_time", 3600),
        ("subnet_mask", "255.255.255.0"),
        ("router", server_ip),
        ("name_server", "8.8.8.8"),
        "end"
    ])

    offer_pkt = ether / ip / udp / bootp / dhcp

    os.makedirs("temp", exist_ok=True)
    wrpcap("temp/dhcp_offer.pcap", [offer_pkt])
    write_hex_format(raw(offer_pkt), "temp/dhcp_offer.txt")

    print("Wrote DHCP OFFER to 'temp/dhcp_offer.pcap' and 'temp/dhcp_offer.txt'.")

def dhcp_req_ack():
    req_pkt = rdpcap("temp/dhcp_req.pcap")[0]

    client_mac = req_pkt[Ether].src
    transaction_id = req_pkt[BOOTP].xid

    offer_ip = "192.168.1.100"
    server_ip = "192.168.1.1"

    ether = Ether(dst=client_mac, src="AB:CD:12:34:56")
    ip = IP(src=server_ip, dst=offer_ip, flags="DF", frag=0)
    udp = UDP(sport=67, dport=68)
    bootp = BOOTP(
        op=2,
        yiaddr=offer_ip,
        siaddr=server_ip,
        chaddr=mac2str(client_mac),
        xid=transaction_id,
        flags=0x8000
    )
    dhcp = DHCP(options=[
        ("message-type", "ack"),   # <--- fixed to ACK
        ("server_id", server_ip),
        ("lease_time", 3600),
        ("subnet_mask", "255.255.255.0"),
        ("router", server_ip),
        ("name_server", "8.8.8.8"),
        "end"
    ])

    ack_pkt = ether / ip / udp / bootp / dhcp

    os.makedirs("temp", exist_ok=True)
    wrpcap("temp/dhcp_ack.pcap", [ack_pkt])
    write_hex_format(raw(ack_pkt), "temp/dhcp_ack.txt")

    print("Wrote DHCP ACK to 'temp/dhcp_ack.pcap' and 'temp/dhcp_ack.txt'.")

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("type", choices=["random_frame", "dhcp_offer", "dhcp_ack"])
    args = parser.parse_args()

    if args.type == "random_frame":
        generate_random_frame()
    elif args.type == "dhcp_offer":
        dhcp_disc_offer()
    elif args.type == "dhcp_ack":
        dhcp_req_ack()
    else:
        print("Invalid option.")
