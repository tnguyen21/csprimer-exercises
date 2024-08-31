def encode(i: int):
    payload = []
    while i > 0:
        part = i % 128
        i >>= 7
        if i > 0:
            part |= 0x80
        else:
            part |= 0x00
        payload.append(part)
    
    return bytes(payload)
    
def decode(bs: bytes):
    num = 0
    
    for b in reversed(bs):
        num <<= 7
        num += (b & 0x7f )
        
    return num

if __name__ == "__main__":
    assert encode(150) == b'\x96\x01'
    assert decode(b'\x96\x01') == 150
    
    for i in range(1_000_000):
        assert decode(encode(i)) == i