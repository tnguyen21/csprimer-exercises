"""
given two IP addresses, how do you determined if a 3rd is within the range of the first two?




"""
import struct, socket

asdb = []

with open("ip2asn.tsv", "r") as f:
    while line := f.readline():
        start, end, _, _, desc = line.split("\t")
        start = struct.unpack("!I", socket.inet_aton(start))[0]
        end = struct.unpack("!I", socket.inet_aton(end))[0]
        desc = desc.strip()
        asdb.append((start, end, desc))

print(asdb[:10])
sorted_asdb = sorted(asdb, key = lambda x: x[0])
print(sorted_asdb[:10])
