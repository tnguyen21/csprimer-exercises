import sys, socket, struct, time

SEGMENT_SZ = 4

def pack_data(seq, ack, payload):
    return struct.pack('!HH', seq, ack) + payload

def unpack_data(b: bytes):
    seq, ack = struct.unpack('!HH', b[:4])
    return seq, ack, b[4:] 

class ReliableConnection():
    def __init__(self, to_addr=None):
        self.to_addr = to_addr
        self.seq, self.ack, self.their_ack = 0, 0, 0
        self.pending_payloads = []
        self._sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._sock.settimeout(1)

    def send(self, msg: bytes):
        if not self.to_addr:
            print("No `to_addr`; Aborting!")
            raise Exception

        for i in range(0, len(msg), SEGMENT_SZ):
            segment = pack_data(self.seq, self.ack, msg[i:i+SEGMENT_SZ])
            while self.their_ack <= self.seq:
                self._sock.sendto(segment, (self.to_addr))
                self._wait_for_other()
            self.seq += 1

    def recv(self):
        while not self.pending_payloads:
            self._wait_for_other()
        return self.pending_payloads.pop(0)
    
    def _wait_for_other(self):
        while True:
            try:
                data, sender = self._sock.recvfrom(4096)
            except socket.timeout:
                break

            if not self.to_addr: self.to_addr = sender
            
            # if self.to_addr != sender: continue
            
            seq, ack, msg = unpack_data(data)
            # print(f'recvd (seq, ack): {seq}, {ack}')
            self.their_ack = max(self.their_ack, ack)

            if msg and seq <= self.ack:
                if msg and seq == self.ack:
                    self.pending_payloads.append(msg)
                    self.ack += 1
                
                segment = pack_data(self.seq, self.ack, b'')
                self._sock.sendto(segment, self.to_addr)
            break

    def bind(self, own_addr): self._sock.bind(own_addr)
    def close(self): self._sock.close()

if __name__ == "__main__":
    if len(sys.argv) > 1:
        # client
        rd = ReliableConnection(('localhost', int(sys.argv[1])))
        rd.send(b"0123456789" * 10)
        # print(rd.recv())
        # print(rd.recv())
    else:
        # act as server, listen forever
        rd = ReliableConnection()
        rd.bind(('0.0.0.0', 8000))
        try:
            while True:
                msg = rd.recv()
                print(msg)
        except KeyboardInterrupt:
            print("Closing connection")
        except Exception as msg:
            print(msg, file=sys.stderr)
        # print(rd.recv())
        # print(rd.recv())
        # print(rd.recv())
        # print(rd.to_addr)
        # rd.send(b'abcde')

    rd.close()