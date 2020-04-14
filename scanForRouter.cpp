
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "tins/tins.h"
#include "Buffer/Packet_Buffer.hpp"

void management_wlan(Tins::Dot11ManagementFrame *mgmnt_frame, std::map <std::string, std::string> &router_map) {
    auto mac_src = mgmnt_frame->addr2();
    auto ssid = mgmnt_frame->ssid();
    auto beacon = mgmnt_frame->find_pdu<Tins::Dot11Beacon>();
    if (beacon != nullptr) {
        if (router_map.find("ssid") == router_map.end()) {
            // std::cout << ssid << " " << mac_src.to_string() << std::endl;
            router_map.insert(std::make_pair(ssid, mac_src.to_string()));
        }
    }
}


void process_packets(std::string iface, std::map <std::string, std::string> &router_map_tmp) {

    auto packet_buffer = std::make_unique<Packet_buffer>(iface);
    packet_buffer->start_sniffing();
    auto start_time = std::chrono::system_clock::now();
    bool stop_loop = false;

    while (!stop_loop) {
        if (packet_buffer->queue_status()) {
            auto packet = packet_buffer->get_packet();

            try {
                auto pdu = packet.pdu();

                auto mgmnt_frame = pdu->find_pdu<Tins::Dot11ManagementFrame>();
                if (mgmnt_frame != nullptr)
                    management_wlan(mgmnt_frame, router_map_tmp);
            }
            catch (...) {
                std::cout << "Error during paket interpretation" << std::endl;
            }
        }
        auto timestamp = std::chrono::system_clock::now();
        if (start_time + std::chrono::seconds(10) < timestamp)
            stop_loop = true;
    }
    return;
}

std::string scan_for_router(std::string network_interface) {
    auto router_map = std::map<std::string, std::string>();
    process_packets(network_interface, router_map);

    std::string return_string{""};
    for (auto const &iter : router_map) {
        return_string += iter.first + ":" + iter.second + ",";
    }
    return return_string;
}