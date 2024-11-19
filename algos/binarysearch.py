import random
import time

def binarysearch(l: list, v: int):
    lo, hi = 0, len(l)
    
    while hi > lo:
        mid = (lo+hi) // 2
        
        if l[mid] == v:
            return mid
        if l[mid] > v:
            hi = mid
        if l[mid] < v:
            lo = mid+1
    return None

def iterative(l: list, v: int):
    for idx, val in enumerate(l):
        if val == v:
            return idx
    
    return None

if __name__ == "__main__":
    for size in (10, 100, 1000, 100_000, 1_000_000):
        l = [random.randint(1, 100) for _ in range(size)]
        idx = random.randint(0, size)

        s = time.monotonic()
        _ = iterative(l, 1235)
        print(f"took {time.monotonic() - s} for linear search")

        l = sorted(l)
        s = time.monotonic()
        _ = binarysearch(l, 1235)
        print(f"took {time.monotonic() - s} for binary search")
