"""
1, 2, 3 steps

dynamic programming problem
- think of base cases
  - larger than base cases are just sum of smaller cases

{
    1: 1 # 1 step, only 1 way to do
    2: 2 # 2 steps, 2 ways to do (2, (1, 1))
    3: 4  # 3 steps, ((1, 1, 1), (1, 2), (2, 1), 3)
    4: steps(3) + steps(1),
}
(3, 1), (1, 3), (1, 1, 1, 1), (1, 2, 1), (2, 1, 1), (1, 1, 2), (2, 2)

steps(3)+steps(1) == steps(2) + steps(2)
4 + 1 == 2 + 2

n=1, 1
n=2, 11, 2
n=3, 111 12 21
n=4, 1111 112 211 121 22
n=5, 11111 1112 1121 1211 2111 221 212 122 

n=4, 1->n=3, 2->n=2
n=5, 1->n=4, 2->n=3, 3->n=2

- figure out our base cases (n=1,2, or 3)
- n>base cases, steps(n-1)+steps(n-2)+steps(n-3)
- update `memo`

"""

def steps(n: int):
    memo = {0: 0, 1: 1, 2: 2, 3: 4}
    if n < 4:
        return memo[n]

    for i in range(4, n+1):
        memo[i] = memo[i-1] + memo[i-2] + memo[i-3]
    
    return memo[n]

if __name__ == "__main__":
    for i in range(100):
        print(f"steps {i}", steps(i))
