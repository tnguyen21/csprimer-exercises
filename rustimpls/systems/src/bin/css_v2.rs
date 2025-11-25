use regex::{Captures, Regex};
use std::env;
use std::fs;
use std::sync::OnceLock;

fn main() {
    let path = env::args().nth(1).expect("Please provide a file path");
    let content = fs::read_to_string(&path).expect("Could not read file");

    let converted = convert_hex_colors(&content);
    print!("{converted}");
}

fn convert_hex_colors(input: &str) -> String {
    // Regex to match #rgb, #rgba, #rrggbb, #rrggbbaa
    static RE: OnceLock<Regex> = OnceLock::new();
    let re = RE.get_or_init(|| {
        Regex::new(r"#([0-9a-fA-F]{3,4}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})\b").unwrap()
    });

    re.replace_all(input, |caps: &Captures| {
        convert_hex_to_rgb(&caps[1]).unwrap_or_else(|| caps[0].to_string())
    })
    .to_string()
}

fn convert_hex_to_rgb(hex: &str) -> Option<String> {
    // We map the hex char to its index in this string.
    const HEX_DIGITS: &[u8] = b"0123456789abcdef";

    // A fast helper to convert a hex byte (e.g., b'a') to decimal (10)
    let to_dec = |b: u8| -> u8 {
        HEX_DIGITS
            .iter()
            .position(|&d| d == b.to_ascii_lowercase())
            .unwrap_or(0) as u8
    };

    // Normalize input into a vector of color channel values (0-255)
    let values: Vec<u8> = if hex.len() == 3 || hex.len() == 4 {
        // Shorthand Case (#abc): Each char represents a byte repeated
        hex.bytes()
            .map(|b| {
                let v = to_dec(b);
                (v << 4) | v
            })
            .collect()
    } else {
        // Standard Case (#aabbcc): Two chars represent one byte
        hex.as_bytes()
            .chunks(2)
            .map(|chunk| {
                let v1 = to_dec(chunk[0]);
                // chunk[1] is guaranteed by Regex, but safe indexing is good practice
                let v2 = to_dec(*chunk.get(1).unwrap_or(&b'0'));
                (v1 << 4) | v2
            })
            .collect()
    };

    match values.as_slice() {
        [r, g, b] => Some(format!("rgb({}, {}, {})", r, g, b)),
        [r, g, b, a] => Some(format!("rgba({}, {}, {}, {})", r, g, b, format_alpha(*a))),
        _ => None, // Should not be reached if Regex is correct
    }
}

fn format_alpha(value: u8) -> String {
    let normalized = (value as f64) / 255.0;
    let s = format!("{:.5}", normalized);
    s.trim_end_matches('0').trim_end_matches('.').to_string()
}
