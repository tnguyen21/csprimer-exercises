from pprint import pprint
x = [
    1, 2, 3,
    [1, 2, 3],
    ['a', 'b', 'c', [1, 2, 3]]
]

def my_pprint(xs, indent=0):
    for x in xs:
        if type(x) == list:
            my_pprint(x, indent+2)
        else:
            print(" "*indent + str(x))

def fmt(lst, depth=1):
    parts = []
    for x in lst:
        if type(x) is list:
            parts.append(fmt(x, depth+1))
        else:
            parts.append(repr(x))
    return '[' + (',\n' + ' '*depth).join(parts) + ']'


my_pprint(x)
print('---')
print(fmt(x))
print('---')
pprint(x, width=20)

