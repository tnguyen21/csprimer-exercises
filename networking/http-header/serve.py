import socket
import json

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind(("0.0.0.0", 1337))
    s.listen()
    while True:
        cs, addr = s.accept()

        with cs:
           msg = cs.recv(4096)

           headers, body = msg.split(b'\r\n\r\n')
           resp = {}

           for header in headers.split(b'\r\n')[1:]:
               k, v = header.split(b': ')
               resp[k.decode('ascii')] = v.decode('ascii')

           cs.send(b'HTTP/1.1 200 OK\r\n\r\n')
           cs.send(json.dumps(resp, indent=2).encode('ascii'))

