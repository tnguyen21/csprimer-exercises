use std::fs;
use std::io::Read;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut f = fs::File::open("synflood.pcap")?;

    let mut pcap_header = [0u8; 24];
    let _ = f.read_exact(&mut pcap_header);
    assert!(pcap_header[..4] == [0xd4, 0xc3, 0xb2, 0xa1]);
    let major = i16::from_le_bytes(pcap_header[4..6].try_into().unwrap());
    let minor = i16::from_le_bytes(pcap_header[6..8].try_into().unwrap());
    println!("pcap proto version {}.{}", major, minor);

    let (mut n_packets, mut n_init, mut n_ack) = (0, 0, 0);
    loop {
        let mut packet_header = [0u8; 16];
        let bytes_read = f.read(&mut packet_header)?;
        if bytes_read == 0 {
            break;
        }

        // sanity check packets
        let packet_len = u32::from_le_bytes(packet_header[8..12].try_into().unwrap());
        let trunc_len = u32::from_le_bytes(packet_header[8..12].try_into().unwrap());
        assert!(packet_len == trunc_len);
        n_packets += 1;

        let mut packet = vec![0u8; packet_len as usize];
        f.read_exact(&mut packet)?;

        // parsing out relevant header info: # https://en.wikipedia.org/wiki/IPv4

        // assume dealing with only ipv4 packets
        let version = u32::from_le_bytes(packet[..4].try_into().unwrap());
        assert!(2 == version);

        let src = u16::from_be_bytes([packet[24], packet[25]]);
        let dst = u16::from_be_bytes([packet[26], packet[27]]);
        let flags = u16::from_be_bytes([packet[36], packet[37]]);

        let syn = flags & 0x02 != 0;
        let ack = flags & 0x10 != 0;

        if dst == 80 && syn {
            n_init += 1;
        }

        if src == 80 && ack {
            n_ack += 1
        }
    }

    println!("{} packets parsed", n_packets);
    println!("{} cnx initiated", n_init);
    println!("{} ack packets", n_ack);
    println!("{} % packets acknowledged", n_ack as f64 / n_init as f64);

    Ok(())
}
