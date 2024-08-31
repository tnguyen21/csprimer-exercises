import struct

def conceal(s: str) -> float:
    # return nan
    s_bytes = s.encode()
    if len(s_bytes) < 6:
        byte_array = b'\x7f\xf8' + s_bytes + b'\x00'*(6-len(s_bytes))
    else:
        byte_array = b'\x7f\xf8' + s_bytes[:6]
    return struct.unpack('>d', byte_array)[0]
    
def extract(f: float) -> str:
    sneaky_bytes = struct.pack('>d', f)
    print(sneaky_bytes[2:].decode())

if __name__ == '__main__':
    # all should be nan
    print(conceal("hello!"))
    print(conceal("ho!"))
    print(conceal("ho!!!!!!!!!!!"))
    
    extract(conceal("hello!"))
    extract(conceal("ho!"))
    extract(conceal("ho!!!!!!!!!!!"))