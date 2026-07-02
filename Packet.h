#ifndef PACKET_H
#define PACKET_H
#include <bits/stdc++.h>
using namespace std;

typedef enum{CONTROL,VOIP_AUDIO,VIDEO_STREAM,BEST_EFFORT} PacketPriority;

struct Packet{
    uint32_t srcIP;
    uint32_t destIP;
    PacketPriority priority;
    uint16_t payloadSize;
    char payload[256]; //standard size;
    double entry_time;

    Packet()
    {
        srcIP=0;
        destIP=0;
        priority=BEST_EFFORT;
        payloadSize=0;
        entry_time=0.0;
        payload[0]='\0';
    }
    Packet(uint32_t src, uint32_t dest, PacketPriority prio,double time,uint16_t size=64)
    {
        srcIP=src;
        destIP=dest;
        priority=prio;
        payloadSize=size;
        entry_time=time;
        payload[0]='\0';
    }
       

    static string ipToString(uint32_t ip)
    {
        return to_string((ip>>24)& 0xFF)+"."+to_string((ip>>16)& 0xFF)+"."+
                to_string((ip>>8)& 0xFF)+"."+to_string((ip)& 0xFF);
    }

    static uint32_t stringToIP(const string& ipstr)
    {
        uint32_t b1,b2,b3,b4;
        b1=b2=b3=b4=0;
        if(sscanf(ipstr.c_str(),"%u.%u.%u.%u",&b1,&b2,&b3,&b4)==4)
        {
            return (b1<<24)|(b2<<16)|(b3<<8)|b4;
        }
        return 0;
    }
};


#endif 