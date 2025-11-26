use std::io::{Read, Write};
use std::{env, fs};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let path = env::args().nth(1).expect("Please provide a file path");
    let mut file = fs::File::open(path)?;

    let mut bmp_header = [0u8; 14];
    let _ = file.read_exact(&mut bmp_header);
    assert!(bmp_header[..2] == [0x42, 0x4d]);

    // alloc 124 because that's sz of BITMAPV5HEADER; error for other bmp versions
    let mut dib_header = [0u8; 124];
    let _ = file.read_exact(&mut dib_header);
    assert!(dib_header[0] == 124);

    let height = u32::from_le_bytes(dib_header[4..8].try_into().unwrap());
    let width = u32::from_le_bytes(dib_header[8..12].try_into().unwrap());

    let mut src = vec![0u8; (height * width * 3) as usize];
    let _ = file.read_exact(&mut src);

    let mut rotated = Vec::<u8>::new();
    
    for ty in 0..width {
        for tx in 0..width {
            let sy = tx;
            let sx = width - ty - 1;
            let n = 3 * (sy * width + sx) as usize;
            rotated.extend_from_slice(&src[n..n+3]);
        }
    }

    let mut out_file = fs::File::create("out.bmp")?;
    out_file.write_all(&bmp_header)?;
    out_file.write_all(&dib_header)?;
    out_file.write_all(&rotated)?;

    Ok(())
}
