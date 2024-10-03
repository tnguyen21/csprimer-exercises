import re
import sys

"""
- read line by line
    - if line has pattern match `#xxxxxx`
      then convert to `rgb(x x x)` format
        - find `#`
        - read 2 chars at a time
        - convert to decimal
    - else: continue   

"""

pattern = r"#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})"

with open(sys.argv[1]) as f:
    for line in f:
        matches = re.findall(pattern, line)

        if matches:
            idx = line.find("#")
            print(line[:idx], end="")
            
            print("rgb(", end="")
            print(int(line[idx+1:idx+3], 16), end=" ") # r
            print(int(line[idx+3:idx+5], 16), end=" ") # g
            print(int(line[idx+5:idx+6], 16), end="") # b
            print(");")
        else:
            print(line, end="")
