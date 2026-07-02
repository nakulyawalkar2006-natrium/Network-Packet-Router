# 🚀 Micro-Architectural ASIC Router Simulator

A high-performance software simulation of a modern ASIC-based network router built entirely in **C++**. The project models the internal packet-processing pipeline of hardware routers, including routing lookup, congestion control, scheduling, and event-driven simulation.

---

## 📌 Features

- ⚡ Event-driven router simulation
- 🌐 Longest Prefix Match (LPM) routing using a Flattened Bitwise Trie
- 📦 Packet parsing and forwarding pipeline
- 🚦 Random Early Detection (RED) congestion control
- ⚖️ Deficit Round Robin (DRR) packet scheduling
- 📊 Hardware-level statistics and performance report
- 🖥️ Interactive CLI simulator with configurable traffic generation
- 💾 Cache-friendly contiguous memory data structures

---

## 🏗️ Architecture

```
                Incoming Packets
                       │
                       ▼
              Static Ingress Queue
               (RED Congestion Control)
                       │
                       ▼
                Pipeline Parser
                       │
                       ▼
             Flattened Bitwise Trie
          (Longest Prefix Match Lookup)
                       │
                       ▼
              Egress Port Scheduler
         (Defliest Round Robin Scheduler)
                       │
                       ▼
               Packet Transmission
```

---

## 📂 Project Structure

```
.
├── Packet.h                  # Packet structure and IP utilities
├── StaticQueue.h             # Fixed-size ingress queue with RED
├── FlattenedBitwiseTrie.h    # Cache-friendly routing table
├── EgressScheduler.h         # DRR scheduler
├── Simulator.h               # Core simulation engine
├── Simulator2.h              # Interactive simulator
├── main.cpp                  # Standard simulation
├── main2.cpp                 # CLI-based simulation
└── README.md
```

---

## 🔧 Technologies Used

- C++17
- STL
- Priority Queue
- Min Heap
- Bitwise Operations
- Event-Driven Simulation
- Trie Data Structure
- Scheduling Algorithms

---

## 📖 Components

### 1. Packet

Represents a network packet containing:

- Source IP
- Destination IP
- Priority
- Payload Size
- Arrival Timestamp

Supported priorities:

- CONTROL
- VOIP_AUDIO
- VIDEO_STREAM
- BEST_EFFORT

---

### 2. Flattened Bitwise Trie

Implements **Longest Prefix Match (LPM)** routing.

Features:

- Contiguous memory allocation
- Low memory overhead
- Cache-friendly traversal
- O(32) lookup time

---

### 3. Static Queue

Implements the router ingress queue.

Features:

- Fixed-size circular buffer
- RED (Random Early Detection)
- Tail Drop when full

RED begins probabilistic packet dropping before the queue reaches capacity, reducing congestion.

---

### 4. Egress Scheduler

Implements **Deficit Round Robin (DRR)** scheduling.

Features:

- Fair bandwidth allocation
- Variable packet size support
- Priority-aware scheduling
- Heap-based packet ordering

---

### 5. Event Driven Simulator

The router operates using timestamped simulation events.

Pipeline stages:

1. Packet Arrival
2. Parse
3. Route Lookup
4. Enqueue
5. Arbiter Tick
6. Packet Transmission

---

## 📊 Metrics Generated

After simulation, the following statistics are displayed:

- Total packets received
- Successfully transmitted packets
- Packet drop count
- Drop percentage
- Per-port traffic distribution
- Routing table memory usage
- Congestion policy
- Scheduling policy
- Total simulation runtime

---

## 💻 Interactive CLI

The project also includes an interactive shell.

Supported commands:

```
help
run
set burst <count>
set delay <nanoseconds>
set payload <bytes>
clear
exit
```

Example:

```
admin@Router_Sim> set burst 1000
admin@Router_Sim> set payload 1500
admin@Router_Sim> run
```

---

## ▶️ Compilation

Compile the standard simulator:

```bash
g++ -std=c++17 main.cpp -o router_sim
```

Compile the interactive simulator:

```bash
g++ -std=c++17 main2.cpp -o router_shell
```

Run:

```bash
./router_sim
```

or

```bash
./router_shell
```

---

## 🧪 Example Simulation

The simulator creates multiple traffic classes including:

- Video streams
- Control packets
- Best effort traffic
- Large burst traffic

This demonstrates:

- RED packet drops
- DRR scheduling
- Route lookup
- Packet forwarding
- Queue congestion

---

## 🚀 Future Improvements

- IPv6 support
- ECMP routing
- QoS classification
- Multi-threaded packet processing
- Virtual Output Queues (VOQ)
- Weighted Fair Queueing (WFQ)
- Hardware pipeline visualization
- PCAP file input support

---

## 🎯 Learning Outcomes

This project demonstrates practical implementation of:

- Computer Networks
- Data Structures
- Operating Systems concepts
- Scheduling Algorithms
- Event Driven Simulation
- Cache-aware programming
- High-performance systems programming
- ASIC router architecture

---

## 📜 License

This project is intended for educational and research purposes.

---

## 👨‍💻 Author

**Nakul Yawalkar**

If you found this project useful, consider giving it a ⭐ on GitHub.
