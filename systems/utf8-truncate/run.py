from pprint import pprint

def truncate(bs: bytes):
    n_bytes, bs = bs[0], bs[1:]

    if n_bytes >= len(bs): return bs
    
    while n_bytes>0 and (bs[n_bytes] & 0xc0) == 0x80:
        n_bytes -= 1

    return bs[:n_bytes]
    
    
if __name__ == "__main__":
    cases = []
    expected = []
    with open("cases", "rb") as f:
        for line in f:
            cases.append(line[:-1])
    with open("expected", "rb") as f:
        for line in f:
            expected.append(line[:-1])
    
    for c, e in zip(cases, expected):
        # print(truncate(c), e)
        assert truncate(c) == e