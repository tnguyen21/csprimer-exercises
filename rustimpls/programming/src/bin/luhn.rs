fn verify(card_no: &str) -> bool {
    let mut total = 0;

    for (i, d) in card_no.chars().rev().enumerate() {
        let x = d.to_digit(10).unwrap() * (1 + (i as u32 % 2));
        total += x / 10 + x;
    }

    total % 10 == 0
}

fn main() {}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_luhn_verify() {
        assert_eq!(verify("17893729974"), true);
        assert_eq!(verify("3018088033"), true);
        assert_eq!(verify("6031055343"), true);
        assert_eq!(verify("17493729974"), false);
        assert_eq!(verify("3048088033"), false);
        assert_eq!(verify("6041055343"), false);
    }
}
