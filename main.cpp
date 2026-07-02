#include "Simulator.h"
#include <cstdlib> 
#include <ctime> 
int main() {
    srand(time(0));

    Simulator engine;

    // 1. SETUP: Load rules into our contiguous memory Trie
    engine.configure_route("192.168.1.0", 24, 0); // Port 0
    engine.configure_route("10.0.0.0",    8,  1); // Port 1
    engine.configure_route("192.168.0.0", 16, 2); // Port 2

    // 2. INJECTION: We create packets with different PAYLOAD SIZES to test DRR
    
    // Packet 1: Huge Video File (1500 bytes). Port 1 will have to save DRR credits to send this.
    Packet p1(Packet::stringToIP("172.16.0.5"), Packet::stringToIP("10.0.0.5"), PacketPriority::VIDEO_STREAM, 100.0, 1500);
    engine.schedule_arrival(100.0, p1);

    // Packet 2: Tiny Chat Message (64 bytes). Port 0 can send this immediately.
    Packet p2(Packet::stringToIP("172.16.0.5"), Packet::stringToIP("192.168.1.5"), PacketPriority::BEST_EFFORT, 102.0, 64);
    engine.schedule_arrival(102.0, p2);

    // Packet 3: Critical Control Data (256 bytes). Protected from RED drops.
    Packet p3(Packet::stringToIP("172.16.0.5"), Packet::stringToIP("192.168.50.5"), PacketPriority::CONTROL, 105.0, 256);
    engine.schedule_arrival(105.0, p3);

    // 3. STRESS TEST: Overwhelm the queue to trigger RED Congestion Control drops
    // We send 850 packets. Our RED threshold starts dropping randomly around 200 packets.
    for (int i = 0; i < 850; ++i) {
        // We set these to BEST_EFFORT so RED is allowed to mathematically drop them
        Packet burst(Packet::stringToIP("8.8.8.8"), Packet::stringToIP("10.1.1.1"), PacketPriority::BEST_EFFORT, 200.0 + (i * 0.1), 500);
        engine.schedule_arrival(200.0 + (i * 0.1), burst);
    }
    // Add this right before engine.run();
    Packet dummy_keepalive;
    engine.schedule_arrival(15000.0, dummy_keepalive); // Keeps the universe alive for 15,000 ns

    // 4. RUN THE HARDWARE
    engine.run();

    return 0;
}