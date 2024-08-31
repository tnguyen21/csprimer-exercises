# encodes integer into varint64 format
def encode(i: int):
    # create binary repr for int
    bs = bin(i)[2:][::-1]
    # separate into 7-bit nibbles in little-endian order
    chunks = [bs[i:i+7][::-1] for i in range(0, len(bs), 7)]
    payload = bytes()
    for chunk in chunks[:-1]:
        byte_value = int('1'+chunk, 2).to_bytes()
        payload += byte_value
    payload += int('0'+chunks[-1], 2).to_bytes()
    return payload
    
def decode(bs: bytes) -> int:
    payload = str()
    for b in bs:
        b_repr = bin(b)[2:].zfill(8)
        payload = b_repr[1:] + payload
    return int(payload, 2)

if __name__ == "__main__":
    # assert encode(150) == b'\x96\x01'
    # assert decode(b'\x96\x01') == 150
    for i in range(1_000):
        assert i == decode(encode(i))
