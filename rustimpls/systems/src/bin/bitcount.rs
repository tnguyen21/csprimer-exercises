fn bitcount(mut n: u64) -> u8 {
    let mut cnt = 0;

    while n > 0 {
        n = n & (n - 1);
        cnt += 1;
    }

    cnt
}

fn main() {
    assert!(bitcount(0) == 0);
    assert!(bitcount(1) == 1);
    assert!(bitcount(2) == 1);
    assert!(bitcount(3) == 2);
    assert!(bitcount(8) == 1);

    assert!(bitcount(0xffffffff) == 32);
    assert!(bitcount(0b11001101) == 5);
}