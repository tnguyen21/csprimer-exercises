def verify(card_no: str):
    valid = True
    # drop check digit
    card_no, check_no = card_no[:-1], int(card_no[-1])
    # from right to left, double every 2nd digit
    digits = []
    for i, digit in enumerate(reversed(card_no)):
        if i % 2 == 0:
            digits.append(int(digit) * 2)
        else:
            digits.append(int(digit))
    # sum all digits
    sum = 0
    for digit in digits:
        # if doubling > 9, subtract 9
        if digit > 9:
            sum += (digit - 9)
        else:
            sum += digit
    # calculate (10 - (sum % 10)) % 10
    return (10 - (sum % 10)) % 10 == check_no

def oz_verify(card_no):
    total = 0
    for i, d in enumerate(reversed(card_no)):
        x = int(d) * (1 + i % 2)
        total += x // 10 + x
    return total % 10 == 0

if __name__ == "__main__":
    assert verify("17893729974") == True
    assert verify("3018088033") == True
    assert verify("6031055343") == True

    assert verify("17493729974") == False 
    assert verify("3048088033") == False
    assert verify("6041055343") == False 

    assert oz_verify("17893729974") == True
    assert oz_verify("3018088033") == True
    assert oz_verify("6031055343") == True

    assert oz_verify("17493729974") == False 
    assert oz_verify("3048088033") == False
    assert oz_verify("6041055343") == False 

