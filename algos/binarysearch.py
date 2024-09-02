def search(l: list, v: int):
    """
    - invariant: v should be in [lo, hi) as long as len(range)>0
    """

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
if __name__ == "__main__":
    l = [-3, 4, 5, 6, 10]
    
    print(search(l, 4))
    print(search(l, -3))
    print(search(l, 10))
    print(search(l, -1))
