use std::collections::HashMap;
use std::net::UdpSocket;

fn main() -> std::io::Result<()> {
    let socket = UdpSocket::bind("127.0.0.1:8080")?;
    let mut store: HashMap<String, String> = HashMap::new();

    let mut buf = [0; 4096];
    loop {
        let (amt, src) = socket.recv_from(&mut buf)?;
        let msg = String::from_utf8_lossy(&buf[..amt]).trim().to_string();
        let parts: Vec<&str> = msg.splitn(2, ' ').collect();

        let resp = if parts.is_empty() {
            "Error: malformatted message".to_string()
        } else {
            let op = parts[0];
            if op == "SET" && parts.len() == 2 {
                let kv: Vec<&str> = parts[1].splitn(2, '=').collect();
                if kv.len() == 2 {
                    store.insert(kv[0].to_string(), kv[1].to_string());
                    "OK!".to_string()
                } else {
                    "Error: bad set".to_string()
                }
            } else if op == "GET" && parts.len() == 2 {
                match store.get(parts[1]) {
                    Some(v) => v.to_string(),
                    None => "Error: key not found".to_string(),
                }
            } else {
                "Error: unknown op".to_string()
            }
        };

        socket.send_to(resp.as_bytes(), src)?;
    }
}
