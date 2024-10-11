import socket

s = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
s.bind(('localhost', 1337))

while True:
    msg, sender = s.recvfrom(1024)
    print(f'{sender}: {msg}')
    s.sendto(msg.decode().upper().encode(), sender)
