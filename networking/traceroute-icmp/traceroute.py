import socket, struct

asdb = []

with open("ip2asn.tsv", "r") as f:
  while line := f.readline():
    start, end, asn_no, _, desc = line.split("\t")
    start = struct.unpack("!I", socket.inet_aton(start))[0]
    end = struct.unpack("!I", socket.inet_aton(end))[0]
    desc = desc.strip()
    asdb.append((start, end, asn_no, desc))

sorted_asdb = sorted(asdb, key = lambda x: x[0])

s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)
s.settimeout(1)

def find_asn(ip):
  ip_int = struct.unpack("!I", socket.inet_aton(ip))[0]
  for start, end, asn_no, desc in sorted_asdb:
    if start <= ip_int <= end:
      return f"{asn_no} {desc}"

for ttl in range(1, 65):
  s.setsockopt(socket.SOL_IP, socket.IP_TTL, ttl)
  msg = struct.pack("!BBHHH", 8, 0, 0, 0, ttl)
  checksum = sum(struct.unpack("!HHHH", msg))
  checksum = (checksum >> 16) + (checksum & 0xffff)
  checksum = ~checksum & 0xffff
  msg = struct.pack("!BBHHH", 8, 0, checksum, 0, ttl)
  s.sendto(msg, ('yahoo.co.jp', 0))

  finished = False
  while True:
    try:
      res, (res_ip, _) = s.recvfrom(4096)
    except TimeoutError:
      print('*')
      break

    if res[9] != 1:  # not an ICMP response
      continue

    res_header_length = (res[0] & 0x0f) << 2

    if res[res_header_length] in {0, 3}:  # reply destination unreachable
      finished = True
      print(ttl, find_asn(res_ip), res_ip)
      break

    if not (res[res_header_length] == 11 and
            res[res_header_length + 1] == 0):  # TTL expired in transit
      continue

    print(ttl, find_asn(res_ip), res_ip)
    break

  if finished:
    break