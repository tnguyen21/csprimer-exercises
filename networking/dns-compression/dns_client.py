import sys
import socket
import struct
import random

GOOGLE_PUBLIC_DNS = ('8.8.8.8', 53)


def skip_name(res, i):
    next_i = None
    labels = []
    while True:
        x = res[i]
        if x & 0b11000000:
            if next_i is None: next_i = i + 2
            i = ((x & 0x3f) << 8) | res[i+1]
        elif x == 0x00:
            if next_i is None: next_i = i + 1
            break
        else:
            labels.append(res[i+1:i+1+x])
            i += x + 1
    return b'.'.join(labels), next_i


if __name__ == '__main__':
    hostname = sys.argv[1]
    rtype = {
        'A': 1,
        'NS': 2,
    }[sys.argv[2]]

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    xid = random.randint(0, 0xffff)
    flags = 0x0100
    query = struct.pack('!HHHHHH', xid, flags, 1, 0, 0, 0)
    qname = b''.join(
        len(p).to_bytes(1, 'big') + p.encode('ascii')
        for p in hostname.split('.')) + b'\x00'
    query += qname
    query += struct.pack('!HH', rtype, 1)
    s.sendto(query, GOOGLE_PUBLIC_DNS)
    # loop until we get the response to our answer
    while True:
        res, sender = s.recvfrom(4096)
        if sender != GOOGLE_PUBLIC_DNS:
            continue
        rxid, rflags, qdcount, ancount, _, _ = \
            struct.unpack('!HHHHHH', res[:12])
        if rxid == xid:
            break

    assert qdcount == 1
    i = 12
    name, i = skip_name(res, i)  # skip name in question
    i += 4  # skip qtype and qclass
    for idx in range(ancount):
        name, i = skip_name(res, i)  # skip name in answer
        print(name)
        rtype, rclass, ttl, rdlength = struct.unpack('!HHIH', res[i:i+10])
        if rtype == 1:
            ip_addr = res[i+10:i+10+rdlength]
            print('.'.join(str(b) for b in ip_addr))
        if rtype == 2:
            name, i = skip_name(res, i+10)
            print(name)
