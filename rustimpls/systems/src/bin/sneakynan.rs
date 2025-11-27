fn conceal(s: String) -> f64 {
    let s_bytes = s.as_bytes();
    let mut out = [0u8; 8];

    out[0..2].copy_from_slice(&[0x7f, 0xf8]);

    let len = s_bytes.len().min(6);
    out[2..2 + len].copy_from_slice(&s_bytes[..len]);

    f64::from_le_bytes(out)
}

fn extract(f: f64) -> String {
    let bytes = f.to_le_bytes();
    let end = bytes[2..].iter().position(|&b| b == 0).unwrap_or(6);

    str::from_utf8(&bytes[2..2 + end]).unwrap().to_string()
}

fn main() {
    let msg = "hotdog".to_string();
    assert_eq!(msg, extract(conceal(msg.clone())));

    let msg = "hi".to_string();
    assert_eq!(msg, extract(conceal(msg.clone())));

    // should truncate long messages
    let msg = "hello!!!".to_string();
    assert_eq!("hello!".to_string(), extract(conceal(msg.clone())));
}
