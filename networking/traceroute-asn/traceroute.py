import socket
import struct

sender = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
receiver = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)
receiver.settimeout(1)
target_port = 33434

asdb = []

with open("ip2asn.tsv", "r") as f:
    while line := f.readline():
        start, end, asn_no, _, desc = line.split("\t")
        start = struct.unpack("!I", socket.inet_aton(start))[0]
        end = struct.unpack("!I", socket.inet_aton(end))[0]
        desc = desc.strip()
        asdb.append((start, end, asn_no, desc))

sorted_asdb = sorted(asdb, key = lambda x: x[0])

def find_asn(ip):
    ip_int = struct.unpack("!I", socket.inet_aton(ip))[0]
    for start, end, asn_no, desc in sorted_asdb:
        if start <= ip_int <= end:
            return f"{asn_no} {desc}"

for ttl in range(1, 65):
    sender.setsockopt(socket.SOL_IP, socket.IP_TTL, ttl)
    sender.sendto(b'\x00' * 24, ('111.222.123.101', target_port))

    finished = False
    while True:
        try:
            res, (res_ip, _) = receiver.recvfrom(4096)
        except TimeoutError:
            print('*')
            break

        if res[9] != 1:  # not an ICMP response
            continue

        res_header_length = (res[0] & 0x0f) << 2

        if res[res_header_length] == 3:  # destination unreachable
            finished = True
            print(ttl, res_ip)
            break

        if not (res[res_header_length] == 11 and
                res[res_header_length + 1] == 0):  # TTL expired in transit
            continue

        x = (res_header_length) + 8  # start of embedded IP header
        xx = x + ((res[x] & 0x0f) << 2) + 2  # start of embedded UDP dest port
        challenge_port = struct.unpack('!H', res[xx:xx+2])[0]

        if challenge_port != target_port:  # not response to our probe
            continue
        
        print(ttl, find_asn(res_ip), res_ip)
        break

    if finished:
        break
    target_port += 1
