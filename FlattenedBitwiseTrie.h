#ifndef FLATTENED_BITWISE_TRIE_HPP
#define FLATTENED_BITWISE_TRIE_HPP
#include <bits/stdc++.h>
using namespace std;

struct TrieNode{
    int32_t child_0;
    int32_t child_1;
    int16_t port_id;

    TrieNode()
    {
        child_0=-1;
        child_1=-1;
        port_id=-1;
    }
};

class FlattenedBitwiseTrie{
private:
    vector<TrieNode> node_pool;
public:
    FlattenedBitwiseTrie()
    {
        node_pool.reserve(1024);
        node_pool.emplace_back(); //for root node
    }

    void insert(uint32_t ip,uint8_t prefix_length,int16_t port_id)
    {
        int32_t current_node=0;
        for(int i=31;i>31-prefix_length;i--)
        {
            uint32_t bit=((ip>>i)&1);
            if(bit==0)
            {
                if(node_pool[current_node].child_0==-1)
                {
                    node_pool.emplace_back();
                    node_pool[current_node].child_0=node_pool.size()-1;
                }
                current_node=node_pool[current_node].child_0;
            }
            else
            {
                if(node_pool[current_node].child_1==-1)
                {
                    node_pool.emplace_back();
                    node_pool[current_node].child_1=node_pool.size()-1;
                }
                current_node=node_pool[current_node].child_1;
            }
        }
        node_pool[current_node].port_id=port_id;
    }

    int16_t longest_prefix_match(uint32_t dest_ip) const
    {
        int32_t current_node=0;
        int16_t best_port=-1;
        for(int i=31;i>=0;i--)
        {
            if(node_pool[current_node].port_id!=-1)
            {
                best_port=node_pool[current_node].port_id;
            }
            uint32_t bit=((dest_ip>>i)&1);
            int32_t next_node=node_pool[current_node].child_1;
            if(bit==0) next_node=node_pool[current_node].child_0;
            if(next_node==-1) break;
            current_node=next_node;
        }
        if(node_pool[current_node].port_id!=-1)
        {
            best_port=node_pool[current_node].port_id;
        }
        return best_port;
    }

    size_t get_memory_footprint() const{
        size_t size=node_pool.size()*sizeof(TrieNode);
        return size;
    }

};

#endif