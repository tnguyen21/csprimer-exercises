use termios::*;

fn main() {
    let mut termios = Termios::from_fd(fd).unwrap();
}
