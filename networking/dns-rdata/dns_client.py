import argparse
import enum
import socket
import struct
import random

GOOGLE_PUBLIC_DNS = ('8.8.8.8', 53)


class RType(enum.Enum):
    A = 1
    NS = 2
    CNAME = 5
    SOA = 6
    PTR = 12
    MX = 15
    TXT = 16
    AAAA = 28
    RRSIG = 46
    ANY = 255

def parse_name(res, i):
    r"""
    Given response data and starting index, parse and return full domain
    name per RFC 1035 section 4.1.4, and return it along with next index.

    >>> parse_name(b'\x06google\x03com\x00', 0)
    ('google.com.', 12)

    >>> parse_name(b'\x06google\x03com\x00\xc0\x00', 12)
    ('google.com.', 14)

    >>> parse_name(b'\x06google\x03com\x00\x03ns1\xc0\x00', 12)
    ('ns1.google.com.', 18)
    """
    next_i = None  # index at which to continue parsing
    labels = []
    offsets = set()
    while True:
        b = res[i]
        if b & 0b11000000:  # pointer
            if next_i is None:
                next_i = i + 2
            i = ((b & 0b00111111) << 8) | res[i+1]
            if i in offsets:
                break  # TODO better way of communicating infinite loop
            offsets.add(i)
        elif b == 0b00:  # end of labels
            if next_i is None:
                next_i = i + 1
            break
        else:  # one more labelaa
            labels.append(res[i+1:i+1+b])
            i += b + 1
    return (b'.'.join(labels)).decode('utf8')+'.', next_i


def format_rdata(rtype, data, i, rdlength):
    r"""
    >>> format_rdata(RType.A, b'\xff\x80\x01\x00', 0, 4)
    '255.128.1.0'

    """
    if rtype is RType.A:
        return f'{data[i]}.{data[i+1]}.{data[i+2]}.{data[i+3]}'
    elif rtype is RType.AAAA:
        chunks = [data[idx:idx+2] for idx in range(i, i+16, 2)]
        return ':'.join([chunk.hex() for chunk in chunks])
    elif rtype is RType.MX:
        pref = str(data[i] << 8 | data[i+1])
        name, _ = parse_name(data, i+2)
        return f'{pref} {name}'
    elif rtype in {RType.NS, RType.CNAME, RType.PTR}:
        name, i = parse_name(data, i)
        return name
    elif rtype is RType.TXT:
        return data[i:i+rdlength].decode('utf8')
    elif rtype is RType.SOA:
        mname, i = parse_name(data, i)
        rname, i = parse_name(data, i)
        serial, refresh, retry, expire, min = \
            struct.unpack("!IIIII", data[i:i+20])
        return f'{mname} {rname} {serial} {refresh} {retry} {expire} {min}'


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('hostname', nargs='?')
    parser.add_argument('rtype', nargs='?', default='A')
    parser.add_argument('-x', dest='reverse')
    parser.add_argument('--test', action='store_true')
    args = parser.parse_args()

    if args.test:
        import doctest, sys
        doctest.testmod()
        print('OK')
        sys.exit(0)
    if args.reverse:
        rtype = RType.PTR
        hostname = '.'.join(reversed(args.reverse.split('.'))) + '.in-addr.arpa'
    else:
        hostname = args.hostname
        try:
            rtype = RType[args.rtype]
        except KeyError:
            rtype = RType.A
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    xid = random.randint(0, 0xffff)
    flags = 0x0100
    query = struct.pack('!HHHHHH', xid, flags, 1, 0, 0, 1)
    qname = b''.join(
        len(p).to_bytes(1, 'big') + p.encode('ascii')
        for p in hostname.split('.')) + b'\x00'
    query += qname
    query += struct.pack('!HH', rtype.value, 1)
    # add OPT PR to extend UDP -- see RFC 6891
    query += b'\x00' + struct.pack('!HHIH', 41, 4096, 0, 0)
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

    i = 12
    name, i = parse_name(res, i)
    i += 4  # skip qtype and qclass
    for idx in range(ancount):
        name, i = parse_name(res, i)
        rtype, rclass, ttl, rdlength = struct.unpack('!HHIH', res[i:i+10])
        print(format_rdata(RType(rtype), res, i+10, rdlength))
        i += 10 + rdlength
