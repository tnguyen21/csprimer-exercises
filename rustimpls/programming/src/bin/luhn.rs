#[allow(dead_code)]
fn verify(card_no: &str) -> bool {
    let cleaned: String = card_no.chars().filter(|c| !c.is_whitespace()).collect();

    if cleaned.len() <= 1 {
        return false;
    }

    if !cleaned.chars().all(|c| c.is_ascii_digit()) {
        return false;
    }

    let sum: u32 = cleaned
        .chars()
        .rev()
        .enumerate()
        .map(|(i, c)| {
            let mut d = c.to_digit(10).unwrap();

            if i % 2 == 1 {
                d *= 2;
                if d > 9 {
                    d -= 9;
                }
            }

            d
        })
        .sum();

    sum % 10 == 0
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
