#ifndef STATIC_QUEUE_H
#define STATIC_QUEUE_H
#include <cstddef>
#include <cstdlib>
#include "Packet.h"
template<typename T,size_t CAPACITY>
class StaticQueue{
private:
    T storage[CAPACITY];
    size_t head_idx;
    size_t tail_idx;
    size_t element_count;
    size_t min_threshold;
    size_t max_threshold;
    double max_drop_probability;

public:
    StaticQueue()
    {
        head_idx=0;
        tail_idx=0;
        element_count=0;
        min_threshold=CAPACITY*0.20;
        max_threshold=CAPACITY*0.80;
        max_drop_probability=0.15;
    } 
    bool enqueue(const T& item)
    {
        if(element_count==CAPACITY) return false;
        if(element_count>min_threshold)
        {
            if(item.priority==BEST_EFFORT || item.priority==VIDEO_STREAM)
            {
                double fill_ratio=(double)(element_count-min_threshold)/(max_threshold-min_threshold);
                double current_prob=max_drop_probability*fill_ratio;
                double random_prob=(double)(rand())/RAND_MAX;
                if(random_prob<current_prob)
                {
                    return false;
                }
            }
        }
        storage[tail_idx]=item;
        tail_idx=(tail_idx+1)%CAPACITY;
        element_count++;
        return true;
    }
    bool dequeue(T& out_item)
    {
        if(element_count==0) return false;
        out_item=storage[head_idx];
        head_idx=(head_idx+1)%CAPACITY;
        element_count--;
        return true;
    }
    size_t get_size() 
    {
        return element_count;
    }
    bool is_empty()
    {
        return element_count==0;
    }
    constexpr size_t get_capacity() const
    {
        return CAPACITY;
    }
};

#endif