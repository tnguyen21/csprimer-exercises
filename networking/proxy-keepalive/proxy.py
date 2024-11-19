import socket, sys, io
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


def handle_client_connection(client_sock):
    while True:
        upstream_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        upstream_sock.connect(UPSTREAM_ADDR)
        log(f'Connected to {UPSTREAM_ADDR}')

        req = HttpRequest()
        close = False
        while req.state is not HttpState.END:
            data = client_sock.recv(4096)
            log(f'-> *    {len(data)}B')

            if not data: close = True; break

            req.parse(data)
            upstream_sock.send(data)
            log(f'   * -> {len(data)}B')

        if close: upstream_sock.close(); return

        while True:
            res = upstream_sock.recv(4096)
            log(f'   * <- {len(res)}B')

            if not res: break

            client_sock.send(res)
            log(f'<- *    {len(res)}B')
        
        upstream_sock.close()

        if not should_keepalive(req): return

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(OWN_ADDR)
s.listen(10)
log(f'Accepting new connections on {OWN_ADDR}')

while True:
    try:
        client_sock, client_addr = s.accept()
        log(f'New connection from {client_addr}')
        handle_client_connection(client_sock)
    except ConnectionRefusedError:
        client_sock.send(b'HTTP/1.1 502 Bad Gateway\r\n\r\n')
        log('<- *    BAD GATEWAY')
    except Exception as msg:
        client_sock.send(b'HTTP/1.1 500 Internal Server Error\r\n\r\n')
        log(msg)
    finally:
        client_sock.close()

s.close()
