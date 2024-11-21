import socket, sys, io, select
from enum import Enum, auto

OWN_ADDR = ('0.0.0.0', 8000)
UPSTREAM_ADDR = ('127.0.0.1', 9000)


def log(s): print(s, file=sys.stderr)

class HttpState(Enum):
    START = auto()
    HEADERS = auto()
    BODY = auto()
    END = auto()

class HttpRequest():
    """ only handles parsing GET requests """
    def __init__(self):
        self.headers, self.residual, self.state = {}, b'', HttpState.START
    
    def parse(self, data: bytes):
        bs = io.BytesIO(self.residual + data)
        self.residual = b''

        if self.state == HttpState.START:
            start_line = bs.readline()
            if start_line[-1:] != b'\n': self.residual = start_line; return

            self.method, self.uri, self.version = start_line.rstrip().split(b' ')
            self.state = HttpState.HEADERS

        if self.state is HttpState.HEADERS:
            while True:
                field_line = bs.readline()
                if not field_line: break
                if field_line[-1:] != b'\n': self.residual = field_line; return

                if field_line == b'\r\n' or field_line == b'\n': self.state = HttpState.END; break

                k, v = field_line.rstrip().split(b': ')
                self.headers[k] = v


def should_keepalive(req):
    c = req.headers.get(b'Connection')
    if req.version == b'HTTP/1.0':
        return c and c.lower() == b'keep-alive'
    if req.version == b'HTTP/1.1':
        return not (c and c.lower() == b'close')


server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_sock.bind(OWN_ADDR)
server_sock.listen(10)
log(f'Accepting new connections on {OWN_ADDR}')

# init list for select and managing client/upstream msgs
inbound, outbound, req_for_client, upstream_for_client = [server_sock], [], {}, {}

try:
    while inbound:
        readable, _, _ = select.select(inbound, outbound, inbound)

        for s in readable:
            if s is server_sock:
                client_sock, client_addr = server_sock.accept()
                log(f"Accepting new connection: {client_addr}")
                client_sock.setblocking(False)
                inbound.append(client_sock)
            else:
                try:
                    upstream_sock = upstream_for_client[s]
                except KeyError:
                    upstream_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    upstream_sock.connect(UPSTREAM_ADDR)
                    upstream_for_client[s] = upstream_sock
                
                try:
                    req = req_for_client[s]
                except KeyError:
                    req = HttpRequest()
                    req_for_client[s] = req
                
                data = s.recv(4096)
                if data:
                    req.parse(data)
                    upstream_sock.send(data)
                    log(f'   * -> {len(data)}B')
                else:
                    s.close()
                    inbound.remove(s)
                    del upstream_for_client[s]
                    del req_for_client[s]

                if req.state is HttpState.END:
                    while True:
                        res = upstream_sock.recv(4096)
                        log(f'   * <- {len(res)}B')
                        if not res: break
                        
                        s.send(res)
                        log(f'<- *    {len(res)}B')
                        
                    upstream_sock.close()
                    del upstream_for_client[s]
                    del req_for_client[s]

                    if not should_keepalive(req):
                        s.close()
                        inbound.remove(s)

except KeyboardInterrupt:
    log("Shutting down server")
finally:
    server_sock.close()
