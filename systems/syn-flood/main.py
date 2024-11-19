import struct

with open("synflood.pcap", "rb") as f:
    # https://docs.python.org/3/library/struct.html#format-strings
    magic, major, minor, _, _, _, ll_header_type = struct.unpack('<IHHIIII', f.read(24))

    assert magic == 0xa1b2c3d4 # check little endian
    assert ll_header_type == 0 # check loopback
    print(f"pcap proto version {major}.{minor}") # expect 2.4

    packet_cnt, initiated, ack_cnt = 0, 0, 0
    while len(packet_header := f.read(16)) != 0:
        _, _, packet_len, nontruncated_len = struct.unpack('<IIII', packet_header)
        assert packet_len == nontruncated_len
        packet_cnt += 1
        packet = f.read(packet_len)
        
        ll_header = packet[:4] # tells if ipv4 or ipv6 
        assert struct.unpack("<I", ll_header)[0] == 2 # assume all packets ipv4
        
        # https://en.wikipedia.org/wiki/IPv4
        ip_n_bytes = (packet[4] & 0x0f) << 2 # ihl * 4 (number of bytes)
        assert ip_n_bytes == 20 # assume no options

        src, dst, _, _, flags = struct.unpack(">HHIIH", packet[24:38])
        syn = flags & 0x0002 > 0
        ack = flags & 0x0010 > 0

        if dst == 80 and syn:
            initiated += 1

        if src == 80 and ack:
            ack_cnt += 1
        # print(f'{src} -> {dst} {syn and "SYN" or ""} {ack and "ACK" or ""}')


    print(f'{packet_cnt} packets parsed')
    print(f'{initiated} cnx initiated')
    print(f'{ack_cnt} ack packets')
    print(f'{ack_cnt/initiated:0.2f} packets acknowledged')

