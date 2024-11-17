"""
1. receive requests on some port
2. parse and confirm its HTTP req
3. forward to known IP:PORT
4. get response back from server
5. some logging
6. send back to client
"""

import socket
import json


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind(("0.0.0.0", 8080))
    s.listen()

    while True:
        cs, addr = s.accept()
        forward_to = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        forward_to.connect(("127.0.0.1", 9000))

        print(f"new connection: {addr}")

        msg = cs.recv(4096)
        print(f"-> *   {len(msg)} bytes")

        forward_to.send(msg)
        print(f"   * ->{len(msg)} bytes")

        while True:
            proxy_resp = (forward_to.recv(4096))
            print(f"   * <-{len(proxy_resp)} bytes")

            if not proxy_resp: break

            cs.send(proxy_resp)
            print(f"<- *   {len(proxy_resp)} bytes")
        
        cs.close()
        forward_to.close()

