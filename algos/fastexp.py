import random

def exp(a, n):
    if n == 0: return 1
    if n == 1: return a

    i = 1
    total = a
    while (True):
        total *= total 
        i *= 2
        if (2*i > n):
            break
    for _ in range(n - i): total *= a

    return total
    # total = 1
    # for _ in range(n): total *= a
    # return total

for _ in range(100):
    rand_a, rand_n = random.randint(1, 100), random.randint(1, 100)
    try:
        assert exp(rand_a, rand_n) == pow(rand_a, rand_n)
    except AssertionError:
        print(f"({rand_a}, {rand_n})",exp(rand_a, rand_n), "!=", pow(rand_a, rand_n))
