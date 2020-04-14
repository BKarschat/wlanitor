/* DNS Query spoof
* by Bastian Karschat
* so call me maybe ;)
*/


#ifndef DNSQUERYSPOOF_HPP
#define DNSQUERYSPOOF_HPP

#include <tins/tins.h>

class DNSQuerySpoof {
private:
    int m_time_to_live;
    std::string m_ip_to_spoof;
    Tins::EthernetII m_packet_to_spoof;
    Tins::EthernetII m_spoofed_packet;
public:
    DNSQuerySpoof();

    DNSQuerySpoof(Tins::EthernetII &packet, int ttl, std::string ip_address);

    ~DNSQuerySpoof();

    void do_query_spoof();

    void set_spoofed_ip(std::string ip);

    void set_time_to_live(int ttl);

    void set_Query(Tins::EthernetII &packet);

    Tins::EthernetII get_Answer();
};


#endif
