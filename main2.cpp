#include "Simulator2.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

// --- UI / UX HELPER FUNCTIONS ---

void clear_screen() {
    cout << "\033[2J\033[1;1H";
}

void print_welcome() {
    cout << "\033[1;33mWelcome to the Micro-Architectural ASIC Router Shell.\033[0m\n";
    cout << "Type \033[1;32m'help'\033[0m to see available commands.\n\n";
}

void animate_progress_bar(const string& task, int duration_ms) {
    const int width = 50;
    for (int i = 0; i <= width; ++i) {
        cout << "\r\033[1;36m[SYSTEM]\033[0m " << task << " [";
        for(int j = 0; j < width; j++) {
            if(j < i) cout << "\033[1;32m#\033[0m";
            else cout << " ";
        }
        cout << "] " << (i * 100 / width) << "%";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(duration_ms / width));
    }
    cout << "\n";
}

// --- MAIN CLI SHELL ---

int main() {
    srand(time(0));

    // Default Traffic Configuration
    int config_burst_size = 850;
    double config_delay = 0.0;
    int config_payload = 500;

    clear_screen();
    print_welcome();

    string input;
    while (true) {
        cout << "\033[1;32madmin@Router_Sim>\033[0m ";
        getline(cin, input);

        if (input.empty()) continue;

        istringstream iss(input);
        string command;
        iss >> command;

        if (command == "exit" || command == "quit") {
            cout << "Shutting down router interface. Goodbye!\n";
            break;
        }
        else if (command == "clear") {
            clear_screen();
            print_welcome();
        }
        else if (command == "help") {
            cout << "\n\033[1;33m--- Available Commands ---\033[0m\n";
            cout << "  \033[1;36mrun\033[0m                     : Execute the hardware simulation with current config.\n";
            cout << "  \033[1;36mset burst <count>\033[0m       : Set the number of packets per wave (Current: " << config_burst_size << ").\n";
            cout << "  \033[1;36mset delay <ns>\033[0m          : Set the delay between burst packets (Current: " << config_delay << " ns).\n";
            cout << "  \033[1;36mset payload <bytes>\033[0m     : Set the payload size of burst packets (Current: " << config_payload << " bytes).\n";
            cout << "  \033[1;36mclear\033[0m                   : Clear the terminal screen.\n";
            cout << "  \033[1;36mexit\033[0m                    : Close the simulator.\n\n";
        }
        else if (command == "set") {
            string target;
            iss >> target;
            if (target == "burst") {
                iss >> config_burst_size;
                cout << "Burst size updated to " << config_burst_size << " packets.\n";
            } else if (target == "delay") {
                iss >> config_delay;
                cout << "Packet delay updated to " << config_delay << " ns.\n";
            } else if (target == "payload") {
                iss >> config_payload;
                cout << "Payload size updated to " << config_payload << " bytes.\n";
            } else {
                cout << "\033[1;31mError: Unknown set target. Try 'set burst', 'set delay', or 'set payload'.\033[0m\n";
            }
        }
        else if (command == "run") {
            cout << "\n";
            animate_progress_bar("Initializing L1 Cache & Routing Trie...  ", 400);
            animate_progress_bar("Injecting Burst Traffic into Hardware... ", 600);

            Simulator engine;

            // 1. SETUP ROUTES
            engine.configure_route("192.168.1.0", 24, 0);
            engine.configure_route("10.0.0.0",    8,  1);
            engine.configure_route("192.168.0.0", 16, 2);

            // 2. INJECT STANDARD PACKETS
            Packet p1(Packet::stringToIP("172.16.0.5"), Packet::stringToIP("10.0.0.5"),
                PacketPriority::VIDEO_STREAM, 100.0, 1500);
            engine.schedule_arrival(100.0, p1);

            Packet p2(Packet::stringToIP("172.16.0.5"), Packet::stringToIP("192.168.1.5"),
                PacketPriority::BEST_EFFORT, 102.0, 64);
            engine.schedule_arrival(102.0, p2);

            Packet p3(Packet::stringToIP("172.16.0.5"), Packet::stringToIP("192.168.50.5"),
                PacketPriority::CONTROL, 105.0, 256);
            engine.schedule_arrival(105.0, p3);

            // 3. FIRST WAVE — arrives early, fills queue
            for (int i = 0; i < config_burst_size; ++i) {
                Packet burst(
                    Packet::stringToIP("8.8.8.8"),
                    Packet::stringToIP("10.1.1.1"),
                    PacketPriority::BEST_EFFORT,
                    200.0 + (i * config_delay),
                    config_payload
                );
                engine.schedule_arrival(200.0 + (i * config_delay), burst);
            }

            // 4. SECOND WAVE — arrives mid-simulation while DRR is still draining
            //    first wave, guaranteeing drops happen WHILE TX is already printing
            double mid_sim_time = 200.0 + (config_burst_size * config_delay) + 500.0;
            for (int i = 0; i < config_burst_size; ++i) {
                Packet burst2(
                    Packet::stringToIP("8.8.8.8"),
                    Packet::stringToIP("192.168.1.5"),
                    PacketPriority::VIDEO_STREAM,
                    mid_sim_time + (i * config_delay),
                    config_payload
                );
                engine.schedule_arrival(mid_sim_time + (i * config_delay), burst2);
            }

            // 5. KEEPALIVE — gives DRR enough time to fully drain both waves
            Packet dummy_keepalive;
            engine.schedule_arrival(
                mid_sim_time + 500.0 + (config_burst_size * 20.0),
                dummy_keepalive
            );

            // 6. RUN
            engine.run();
            cout << "\n";
        }
        else {
            cout << "\033[1;31mError: Unknown command '" << command << "'. Type 'help' for options.\033[0m\n";
        }
    }

    return 0;
}