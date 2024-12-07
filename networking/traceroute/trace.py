import socket
import struct
import time

def traceroute(dest_name, max_hops=30, timeout=2.0):
    # Resolve the destination name to an IP
    dest_addr = socket.gethostbyname(dest_name)
    print(f"Traceroute to {dest_name} ({dest_addr}), {max_hops} hops max")

    # Create a raw socket to receive ICMP
    # This socket is only for listening to ICMP responses
    icmp = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)
    icmp.settimeout(timeout)

    # Create a UDP socket for sending the packets
    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

    ttl = 1
    port = 33434  # Default starting port used by traceroute

    while ttl <= max_hops:
        # Set the TTL on the send socket
        udp.setsockopt(socket.SOL_IP, socket.IP_TTL, ttl)

        # Record the time when we send the packet
        start_time = time.time()

        # Send a single UDP packet
        udp.sendto(b"", (dest_addr, port))

        curr_addr = None
        curr_name = None

        # Wait for a reply
        try:
            # Receive a response
            data, addr = icmp.recvfrom(512)
            end_time = time.time()
            elapsed_ms = (end_time - start_time) * 1000

            # The addr[0] is the router address that responded
            curr_addr = addr[0]

            # Try to resolve the hostname
            try:
                curr_name = socket.gethostbyaddr(curr_addr)[0]
            except socket.herror:
                curr_name = curr_addr

        except socket.timeout:
            # No response received
            pass

        if curr_addr is not None:
            # Print hop information
            print(f"{ttl}\t{curr_name} ({curr_addr})  {elapsed_ms:.3f} ms")
        else:
            # No response
            print(f"{ttl}\t*")

        ttl += 1

        # Check if we've reached the destination
        if curr_addr == dest_addr:
            break

    # Close sockets
    udp.close()
    icmp.close()

# Example usage:
# You might need to run with sudo privileges depending on OS
if __name__ == "__main__":
    traceroute("google.com")
