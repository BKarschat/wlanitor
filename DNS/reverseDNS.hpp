/*
* by Bastian Karschat
* 07.11.2016
*
*/


#ifndef REVERSEDNS_HPP
#define REVERSEDNS_HPP


#include <tins/tins.h>

class ReverseDNS {
private:
    int in_port, out_port;
    std::string name_to_resolve;
    Tins::HWAddress<6> mac_Router;
    Tins::IPv4Address ip_Router;
    Tins::NetworkInterface::Info sender_info;
    Tins::EthernetII packet;
public:
    ReverseDNS();

    ReverseDNS(Tins::HWAddress<6> mac_Router_tmp, Tins::IPv4Address ip_Router_tmp, int out_port_tmp,
               std::string name_to_resolve_tmp,
               Tins::NetworkInterface::Info &info, int in_port_tmp = 53);

    ~ReverseDNS();

    void set_in_port(int port);

    void set_out_port(int port);

    void set_mac_router(Tins::HWAddress<6> mac);

    void set_ip_router(Tins::IPv4Address mac);

    void set_info(Tins::NetworkInterface::Info &info);

    void set_name_to_resolve(std::string name);

    void makePacket();

    Tins::EthernetII get_Packet();

    void split(const std::string &s, char delim, std::vector <std::string> &elems);

    std::vector <std::string> split(const std::string &s, char delim);
};


#endif
