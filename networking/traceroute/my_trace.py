import socket, struct

sender = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
rcver = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP) 
rcver.settimeout(1)

max_hop, target_port = 64, 33434

for ttl in range(1, max_hop):
  sender.setsockopt(socket.SOL_IP, socket.IP_TTL, ttl)
  sender.sendto(b'\x1234' * 4, ('icann.org', target_port))
  finished = False
  while True:
    try:
      data, (res_ip, _) = rcver.recvfrom(4096)
    except socket.timeout:
      print(f'*')
      break

    if data[9] != 1: continue # not ICMP message

    res_header_len = (data[0] & 0x0f) << 2

    if data[res_header_len] == 3: # dest unreachable, found server
      print(f'{ttl=}, {res_ip=}')
      finished = True
      break

    if not (data[res_header_len] == 11 and 
            data[res_header_len + 1] == 0):
      continue

    emb_ip_header = (res_header_len) + 8
    emb_udp_header = emb_ip_header + ((data[emb_ip_header] & 0x0f) << 2) + 2
    dst_port = struct.unpack("!H", data[emb_udp_header:emb_udp_header + 2])[0]

    if dst_port == target_port: continue

    print(f'{ttl=}, {res_ip=}')
    break
  if finished: break
  target_port += 1
