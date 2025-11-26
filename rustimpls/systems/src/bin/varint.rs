fn encode(mut i: u64) -> Vec<u8> {
    let mut payload = Vec::new();

    while i >= 128 {
        payload.push((i as u8 & 0x7f) | 0x80);
        i >>= 7;
    }
    payload.push(i as u8);

    payload
}

fn decode(bs: Vec<u8>) -> u64 {
    let mut n = 0;

    for b in bs.into_iter().rev() {
        n <<= 7;
        n += (b & 0x7f) as u64;
    }

    n
}

fn main() {
    for i in 0..1_000_000 {
        assert_eq!(decode(encode(i)), i);
    }
}
