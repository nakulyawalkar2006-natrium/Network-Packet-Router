#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "EgressScheduler.h"
#include "Packet.h"
#include "StaticQueue.h"
#include "FlattenedBitwiseTrie.h"
#include <thread>
#include <chrono>
#include <sstream>

typedef enum{PACKET_ARRIVAL,PIPELINE_PARSE,PIPELINE_LOOKUP,PIPELINE_ENQUEUE,ARBITER_TICK} EventType;

struct SimulationEvent{
    double timestamp;
    EventType type;
    Packet pkt;

    bool operator>(const SimulationEvent& other) const
    {
        return timestamp>other.timestamp;
    }
};

class Simulator{
private:
    double global_time;
    priority_queue<SimulationEvent,vector<SimulationEvent>,greater<SimulationEvent>> timeline;
    FlattenedBitwiseTrie routing_table;
    StaticQueue<Packet,1024> ingress_queue;
    vector<EgressScheduler> egress_ports;

    uint64_t metric_total_arrived=0;
    uint64_t metric_total_routed=0;
    uint64_t metric_total_dropped=0;
    uint64_t metric_total_port_dispatched[4]={0,0,0,0};

    string tx_line  = "TX:   --";
    string drop_line = "DROP: --";

    // Redraws both status lines in place — always called after any update
    void redraw()
    {
        // Line 1: TX  (green)
        cout << "\r\033[K\033[1;32m" << tx_line << "\033[0m"
        // Move to next line
             << "\n"
        // Line 2: DROP (red background)
             << "\r\033[K\033[1;41;97m" << drop_line << "\033[0m"
        // Jump back up to line 1 so next redraw overwrites from the top
             << "\033[1A\r"
             << flush;
    }

public:
    Simulator()
    {
        global_time=0.0;
        for(int i=0;i<4;i++)
        {
            egress_ports.emplace_back(EgressScheduler(500.0,1000));
        }
        timeline.push({0.0,ARBITER_TICK,Packet()});
    }

    void configure_route(const string& ip_str,uint8_t prefix,int16_t port)
    {
        routing_table.insert(Packet::stringToIP(ip_str),prefix,port);
    }

    void schedule_arrival(double arrival_time,const Packet& pkt)
    {
        timeline.push({arrival_time,PACKET_ARRIVAL,pkt});
        metric_total_arrived++;
    }

    void run()
    {
        cout << "\n[SYSTEM] Booting Micro-Architectural ASIC Simulator...\n";
        cout << "------------------------------------------------------\n";

        // Reserve the two status lines upfront
        cout << "\n";
        // Draw initial blank state of both lines so cursor is positioned correctly
        redraw();
        while(timeline.size()>1)
        {
            SimulationEvent currEvent=timeline.top();
            timeline.pop();
            global_time=currEvent.timestamp;

            if(currEvent.type==PACKET_ARRIVAL)
            {
                if(ingress_queue.enqueue(currEvent.pkt))
                {
                    timeline.push({global_time+2.0,PIPELINE_PARSE,currEvent.pkt});
                }
                else
                {
                    metric_total_dropped++;
                    ostringstream oss;
                    oss << "DROP @ " << fixed << setprecision(2) << global_time
                        << " ns | RED: Packet dropped due to congestion!"
                        << " [Total Drops: " << metric_total_dropped << "]";
                    drop_line = oss.str();
                    redraw();
                }
            }
            else if(currEvent.type==PIPELINE_PARSE)
            {
                Packet dummy;
                ingress_queue.dequeue(dummy);
                timeline.push({global_time+2.0,PIPELINE_LOOKUP,currEvent.pkt});
            }
            else if(currEvent.type==PIPELINE_LOOKUP)
            {
                int16_t target_port=routing_table.longest_prefix_match(currEvent.pkt.destIP);
                if(target_port!=-1 && target_port<4)
                {
                    currEvent.pkt.destIP=target_port;
                    timeline.push({global_time+1.0,PIPELINE_ENQUEUE,currEvent.pkt});
                }
                else
                {
                    metric_total_dropped++;
                    ostringstream oss;
                    oss << "DROP @ " << fixed << setprecision(2) << global_time
                        << " ns | NO ROUTE: Packet dropped (unroutable dest)!"
                        << " [Total Drops: " << metric_total_dropped << "]";
                    drop_line = oss.str();
                    redraw();   
                }
            }
            else if(currEvent.type==PIPELINE_ENQUEUE)
            {
                int16_t port=currEvent.pkt.destIP;
                egress_ports[port].insert(currEvent.pkt);
            }
            else if(currEvent.type==ARBITER_TICK)
            {
                for(int i=0;i<4;i++)
                {
                    egress_ports[i].add_drr_credits();

                    Packet outgoing_pkt;
                    while(egress_ports[i].extract_next(outgoing_pkt))
                    {
                        metric_total_routed++;
                        metric_total_port_dispatched[i]++;

                        ostringstream oss;
                        oss << "TIME: " << fixed << setprecision(2) << global_time
                            << " ns | TX: Port " << i
                            << " transmitted " << outgoing_pkt.payloadSize
                            << " bytes | Priority: " << static_cast<int>(outgoing_pkt.priority)
                            << " | Total Tx: " << metric_total_routed;
                        tx_line = oss.str();
                        redraw();

                        this_thread::sleep_for(chrono::milliseconds(10));
                    }
                }
                timeline.push({global_time+10.0,ARBITER_TICK,Packet()});
            }
        }

        // Move cursor past the two status lines before printing report
        cout << "\n\n\n";
        print_hardware_report();
    }

private:
    void print_hardware_report() {
        cout << "\n======================================================================\n";
        cout << "               HARDWARE-LEVEL SIMULATION REPORT\n";
        cout << "======================================================================\n";
        cout << "Total Execution Runtime    : " << fixed << setprecision(2) << global_time << " nanoseconds\n";
        cout << "Total Incoming Traffic     : " << metric_total_arrived << " packets\n";
        cout << "Packets Successfully Tx    : " << metric_total_routed << " packets\n";

        double drop_rate = metric_total_arrived > 0 ? (double)metric_total_dropped / metric_total_arrived * 100.0 : 0.0;
        cout << "Packets Dropped (RED/Tail) : " << metric_total_dropped << " (Drop Rate: "
             << setprecision(2) << drop_rate << "%)\n\n";

        cout << "------------------ PORT DISTRIBUTION METRICS -------------------------\n";
        cout << "  - Port 0 [Local Traffic]   : " << metric_total_port_dispatched[0] << " packets\n";
        cout << "  - Port 1 [Cloud Gateway]   : " << metric_total_port_dispatched[1] << " packets\n";
        cout << "  - Port 2 [Broad Fallback]  : " << metric_total_port_dispatched[2] << " packets\n";
        cout << "  - Port 3 [Unused/Idle]     : " << metric_total_port_dispatched[3] << " packets\n\n";

        cout << "------------------ ARCHITECTURE CONFIGURATION ------------------------\n";
        cout << "  [Routing Trie]  Memory   : " << routing_table.get_memory_footprint() << " bytes (L1 Cache Contiguous)\n";
        cout << "  [Congestion]    Policy   : Random Early Detection (RED) enabled\n";
        cout << "  [Port Arbiter]  Protocol : Deficit Round-Robin (DRR) Bandwidth Balancing\n";
        cout << "======================================================================\n";
    }
};

#endif