use std::io::{self, Read, Write};
use termios::{Termios, TCSANOW, ICANON, tcsetattr};

fn main() -> io::Result<()> {
    let stdin = 0;
    
    let mut termios = Termios::from_fd(stdin).unwrap();
    
    // Disable canonical mode (line buffering)
    termios.c_lflag &= !(ICANON);
    tcsetattr(stdin, TCSANOW, &termios).unwrap();
    
    let mut stdout = io::stdout();
    let mut stdin = io::stdin();
    let mut buffer = [0u8; 1];
    
    loop {
        stdin.read_exact(&mut buffer)?;
        let c = buffer[0] as char;
        
        if c >= '1' && c <= '9' {
            let beeps = (c as u8 - b'0') as usize;
            for _ in 0..beeps {
                stdout.write_all(&[0x07])?;
                stdout.flush()?;
                std::thread::sleep(std::time::Duration::from_secs(1));
            }
        }
    }
}