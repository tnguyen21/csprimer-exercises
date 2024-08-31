def roman(n: int) -> str:
    """
    - starting from most sig digit
      - mod 1000, 500, etc
      - check if "subtractive" case (e.g. 4, 9) -- append special string instead
      - else append expected string
    """
    parts = (
        (1000, 'M'),
        (900, 'CM'),
        (500, 'D'),
        (400, 'CD'),
        (100, 'C'),
        (90, 'XC'),
        (50, 'L'),
        (40, 'XL'),
        (10, 'X'),
        (9, 'IX'),
        (5, 'V'),
        (4, 'IV'),
        (1, 'I')
    )

    # recursive
    # for d, c in parts:
    #     if d <= n:
    #         return c + roman(n-d)
    

    # iterative
    out = []
    while n > 0:
        for d, c in parts:
            if d <= n:
                out.append(c)
                n = n-d
                break
    return "".join(out)
    

if __name__ == "__main__":
    tests = (
        (39, "XXXIX"),
        (246, "CCXLVI"),
        (789, "DCCLXXXIX"),
        (2421, "MMCDXXI")
    )

    for n, repr in tests:
        print(n, repr)
        assert roman(n) == repr

    print("ok")
