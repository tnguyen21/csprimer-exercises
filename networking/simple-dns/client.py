import socket
import sys
"""
port 53 when using UDP

https://www.ietf.org/rfc/rfc1035.txt
refer to section 4 for message format

12 byte header
\x00\x00\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00
question
\x06google
question
\x03com
end byte for data
\x00
query type (A record)
\x00\x01
query class (IN)
\x00\x01
"""

if not sys.argv[1]:
    print("usage: python client.py <domain>")
domain, label = sys.argv[1].split(".") 
print(domain, len(domain), label, len(label))
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect(("8.8.8.8", 53))  # 8.8.8.8 google DNS, port 53 for UDP

header = b'\x00\x00\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00'
# assume domain and label len can fit in 1 byte 
question = int.to_bytes(len(domain), 1, "big") + domain.encode() + \
    int.to_bytes(len(label), 1, "big") + label.encode() + b'\x00'
q_type_q_class = b'\x00\x01\x00\x01'
s.send(header + question + q_type_q_class)

resp = s.recv(4096)
ip_bytes = resp[-4:]
print(f"IP Address: {'.'.join(f'{num}' for num in ip_bytes)}")
