#ifndef EGRESS_SCHEDULER_H
#define EGRESS_SCHEDULER_H

#include"Packet.h"

class EgressScheduler{

private:
    vector<Packet> heap_array;
    double tolerance_factor;
    uint32_t deficit_counter;
    uint32_t quantum;

public:
    EgressScheduler(double tf,uint32_t port_quant=500)
    {
        heap_array.reserve(1024);
        tolerance_factor=tf;
        quantum=port_quant;
        deficit_counter=0;
    }

    void insert(const Packet& packet)
    {
        heap_array.push_back(packet);
        bubble_up(heap_array.size()-1);
    }

    void add_drr_credits()
    {
        deficit_counter+=quantum;
    }

    bool extract_next(Packet& out_packet)
    {
        if(heap_array.empty()) 
        {
            return false;
        }
        uint16_t required_bytes=heap_array[0].payloadSize;
        if(deficit_counter<required_bytes)
        {
            return false;
        }

        deficit_counter-=required_bytes;
        
        out_packet=heap_array[0];
        heap_array[0]=heap_array.back();
        heap_array.pop_back();

        if(!heap_array.empty())
        {
            bubble_down(0);
        }
        return true;
    }

    size_t get_size() const
    {
        return heap_array.size();
    }

private:
    double claculate_deadline(const Packet& p) const{
        uint8_t prio_val=static_cast<uint8_t>(p.priority);
        return p.entry_time+(prio_val*tolerance_factor);
    }

    void bubble_up(size_t idx)
    {
        while(idx>0)
        {
            size_t parent_idx=(idx-1)/2;
            double child_deadline=claculate_deadline(heap_array[idx]);
            double parent_deadline=claculate_deadline(heap_array[parent_idx]);
            if(child_deadline<parent_deadline)
            {
                swap(heap_array[idx],heap_array[parent_idx]);
                idx=parent_idx;
            }
            else
            {
                break;
            }
        }
    }

    void bubble_down(size_t idx)
    {
        size_t size=heap_array.size();
        while(true)
        {
            size_t l_child=2*idx+1;
            size_t r_child=2*idx+2;
            size_t smallest=idx;
            double smallest_deadline=claculate_deadline(heap_array[smallest]);
            if(l_child<size)
            {
                double l_deadline=claculate_deadline(heap_array[l_child]);
                if(l_deadline<smallest_deadline)
                {
                    smallest=l_child;
                    smallest_deadline=l_deadline;
                }
            }
            if(r_child<size)
            {
                double r_deadline=claculate_deadline(heap_array[r_child]);
                if(r_deadline<smallest_deadline)
                {
                    smallest=r_child;
                    smallest_deadline=r_deadline;
                }
            }
            if(smallest!=idx)
            {
                swap(heap_array[idx],heap_array[smallest]);
                idx=smallest;
            }
            else
            {
                break;
            }
        }
    }

};

#endif
