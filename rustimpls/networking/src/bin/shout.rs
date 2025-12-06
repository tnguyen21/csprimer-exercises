use std::net::UdpSocket;

fn main() -> std::io::Result<()> {
    let socket = UdpSocket::bind("127.0.0.1:5050")?;
    println!("listening @ localhost:5050");
    
    loop {
        let mut buf = [0; 1024];
        let (amt, src) = socket.recv_from(&mut buf)?;

        let msg = buf.to_ascii_uppercase();
        socket.send_to(&msg, &src)?;
        println!("Received {} bytes from {}", amt, src);
    }
}