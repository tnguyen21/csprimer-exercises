import sys, re

d = dict(zip('0123456789abcdef', range(16)))

def xx2dec(xx):
    return (d[xx[0]] << 4) + d[xx[1]]

def hex2rgb(r):
    # "normalize" to non-abbreviated form
    hx = r.group(1).lower()
    if len(hx) in {3, 4}:
        hx = "".join(x + x for x in hx)
    
    # compute RGB
    dec = [xx2dec(hx[i:i+2]) for i in (0, 2, 4)]
    
    # handle alpha if exists
    alpha = None
    if len(hx) == 8:
        alpha = round(xx2dec(hx[6:]) / 255, 5)
    
    r, g, b = dec[0], dec[1], dec[2]
    
    if alpha:
        return f'rgb({r} {g} {b} / {alpha})'
    
    return f'rgb({r} {g} {b})'

print(re.sub(r'\#([0-9a-fA-F]+)', hex2rgb, sys.stdin.read()))