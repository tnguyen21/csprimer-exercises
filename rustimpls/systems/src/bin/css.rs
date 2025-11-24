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
    // Compile Regex once (efficient if function is called multiple times)
    // We verify length inside the regex to avoid matching invalid hex lengths like #12345
    static RE: OnceLock<Regex> = OnceLock::new();
    let re = RE.get_or_init(|| {
        Regex::new(r"#([0-9a-fA-F]{3,4}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})\b").unwrap()
    });

    re.replace_all(input, |caps: &Captures| {
        // caps[0] is the full match (e.g. "#fff"), caps[1] is the hex part ("fff")
        match convert_hex_to_rgb(&caps[1]) {
            Some(rgb) => rgb,
            None => caps[0].to_string(),
        }
    })
    .to_string()
}

fn convert_hex_to_rgb(hex: &str) -> Option<String> {
    // We iterate over bytes to avoid String allocations
    let mut bytes = hex.bytes();

    fn decode_hex_char(c: u8) -> u8 {
        match c {
            b'0'..=b'9' => c - b'0',
            b'a'..=b'f' => c - b'a' + 10,
            b'A'..=b'F' => c - b'A' + 10,
            _ => 0, // Should not happen due to Regex validation
        }
    }

    // Helper to combine two hex chars into one byte (e.g. 'F' and 'F' -> 255)
    let mut next_component = || {
        let c1 = bytes.next()?;
        let c2 = if hex.len() == 3 || hex.len() == 4 {
            c1 // Handle shorthand (e.g., "f" becomes "ff")
        } else {
            bytes.next()?
        };

        Some((decode_hex_char(c1) << 4) | decode_hex_char(c2))
    };

    let r = next_component()?;
    let g = next_component()?;
    let b = next_component()?;

    match next_component() {
        // Check if there is an Alpha channel
        Some(a) => {
            let alpha_str = format_alpha(a);
            Some(format!("rgba({} {} {} {})", r, g, b, alpha_str))
        }
        None => Some(format!("rgb({} {} {})", r, g, b)),
    }
}

fn format_alpha(value: u8) -> String {
    let normalized = (value as f64) / 255.0;
    let s = format!("{:.5}", normalized);
    s.trim_end_matches('0').trim_end_matches('.').to_string()
}
