/*
* by Bastian Karschat
* 07.11.2016
*
*/

#include "reverseDNS.hpp"

ReverseDNS::ReverseDNS() {

}


ReverseDNS::ReverseDNS(Tins::HWAddress<6> mac_Router_tmp, Tins::IPv4Address ip_Router_tmp, int out_port_tmp,
                       std::string name_to_resolve_tmp,
                       Tins::NetworkInterface::Info &info, int in_port_tmp) : sender_info(info) {
    in_port = in_port_tmp;
    out_port = out_port_tmp;
    name_to_resolve = name_to_resolve_tmp;
    mac_Router = mac_Router_tmp;
    ip_Router = ip_Router_tmp;
    makePacket();
}

void ReverseDNS::makePacket() {

    auto dns = Tins::DNS();
    //reverse IP
    std::vector <std::string> ip_tmp = split(name_to_resolve, '.');
    std::reverse(ip_tmp.begin(), ip_tmp.end());
    std::string reverse_ip;
    for (auto const &s : ip_tmp) { reverse_ip += s + "."; };
    Tins::DNS::Query que(reverse_ip + "in-addr.arpa", Tins::DNS::PTR, Tins::DNS::IN);
    dns.type(Tins::DNS::QUERY);
    dns.add_query(que);
    dns.recursion_desired(1);
    packet = Tins::EthernetII(mac_Router, sender_info.hw_addr) / Tins::IP(ip_Router, sender_info.ip_addr) /
             Tins::UDP(in_port, out_port) / dns;

}


ReverseDNS::~ReverseDNS() {

}

void ReverseDNS::set_in_port(int port) {
    in_port = port;
}


void ReverseDNS::set_out_port(int port) {
    out_port = port;
}


void ReverseDNS::set_mac_router(Tins::HWAddress<6> mac) {
    mac_Router = mac;
}

void ReverseDNS::set_ip_router(Tins::IPv4Address ip) {
    ip_Router = ip;
}

void ReverseDNS::set_info(Tins::NetworkInterface::Info &info) {
    sender_info = info;
}


void ReverseDNS::set_name_to_resolve(std::string name) {
    name_to_resolve = name;
}


Tins::EthernetII ReverseDNS::get_Packet() {
    return packet;
}


void ReverseDNS::split(const std::string &s, char delim, std::vector <std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector <std::string> ReverseDNS::split(const std::string &s, char delim) {
    std::vector <std::string> elems;
    split(s, delim, elems);
    return elems;
}
