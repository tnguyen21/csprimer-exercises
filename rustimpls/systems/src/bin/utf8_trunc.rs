use std::io::{BufRead, BufReader, Write};
use std::{env, fs};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let path = env::args().nth(1).expect("Please provide a file path");
    let f_in = fs::File::open(path)?;
    let mut reader = BufReader::new(f_in);
    let mut f_out = fs::File::create("out")?;

    let mut buf = Vec::new();

    loop {
        buf.clear();

        let bytes_read = reader.read_until(b'\n', &mut buf)?;

        if bytes_read == 0 {
            break;
        }

        // immediately rm trailing '\n' to make keep_len math easier
        // TODO error handling/edge case handling
        buf.pop();

        let limit = buf[0] as usize;
        let data = &buf[1..];

        let mut keep_len = std::cmp::min(limit, data.len());

        // more data than our limit, so need to truncate
        if data.len() > keep_len {
            while keep_len > 0 && (data[keep_len] & 0xc0) == 0x80 {
                keep_len -= 1;
            }
        }

        f_out.write_all(&data[..keep_len])?;
        f_out.write_all(b"\r\n")?;
    }

    Ok(())
}
