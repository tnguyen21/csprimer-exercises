import struct

with open("lossy.pcap", "rb") as f:
    # https://docs.python.org/3/library/struct.html#format-strings
    magic, major, minor, _, _, snl, ll_header_type = struct.unpack('<IHHIIII', f.read(24))

    assert ll_header_type == 1 # 1 = ethernet
    print("snapshot len", snl) # 1 = ethernet
    assert magic == 0xa1b2c3d4 # check little endian
    print(f"pcap proto version {major}.{minor}") # expect 2.4

    n_packets = 0
    parts = {}
    while len(packet_header := f.read(16)) != 0:
        _, _, packet_len, nontruncated_len = struct.unpack('<IIII', packet_header)
        assert packet_len == nontruncated_len
        n_packets += 1

        packet = f.read(packet_len)
        assert packet[12:14] == b'\x08\x00' # is ipv4 packet
        assert packet[23] == 6 # protocol is TCP
        
        # ipv4 headers
        ihl = packet[14] & 0b1111
        segment = packet[14+ihl*4:]
        
        # tcp headers
        src, dst, seq_no, _, flags = struct.unpack('!HHIIH', segment[:14])
        syn = flags & 0b10
        if src == 80 and not syn:
            data_offset = flags >> 12
            parts[seq_no] = segment[data_offset*4:]
        
    body = b''.join(v for _, v in sorted(parts.items())).split(b'\r\n\r\n', 1)[1]
    
    with open('test.jpg', 'wb') as f:
        f.write(body)