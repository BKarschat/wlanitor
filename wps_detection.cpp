#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "tins/tins.h"
#include "Buffer/Packet_Buffer.hpp"

std::string management_wlan_wps(Tins::Dot11ManagementFrame *mgmnt_frame, std::string mac_router) {
    auto ssid = mgmnt_frame->ssid();
    std::set <Tins::HWAddress<6>> address;
    if (mgmnt_frame->addr2().to_string() != mac_router && mgmnt_frame->addr2().to_string() != mac_router)
        return "";
    auto beacon = mgmnt_frame->find_pdu<Tins::Dot11Beacon>();
    if (beacon != nullptr) {
        if (address.insert(beacon->addr3()).second) {
            for (const auto &opt : beacon->options()) {
                if (opt.option() == Tins::Dot11::VENDOR_SPECIFIC) {
                    if (opt.data_size() >= 4) {
                        Tins::HWAddress<3> addr = opt.data_ptr();
                        const Tins::HWAddress<3> oui("00:50:F2");
                        if (addr == oui && opt.data_ptr()[3] == 0x04) {
                            return ssid + " enable WPS!";

                        } else {
                            return ssid + " disable WPS!";
                        }
                    }
                }
            }
        }
    }
    return "";
}


std::string process_packets_wps(std::string iface, std::string mac_router) {
    auto packet_buffer = std::make_unique<Packet_buffer>(iface);
    packet_buffer->start_sniffing();
    while (1) {
        if (packet_buffer->queue_status()) {
            auto packet = packet_buffer->get_packet();

            try {
                auto pdu = packet.pdu();
                auto mgmnt_frame = pdu->find_pdu<Tins::Dot11ManagementFrame>();
                if (mgmnt_frame != nullptr) {
                    auto return_string = management_wlan_wps(mgmnt_frame, mac_router);
                    std::cout << return_string << std::endl;
                    if (!return_string.empty())
                        return return_string;
                }
            }
            catch (...) {
                std::cout << "Error during paket interpretation" << std::endl;
            }
        }
    }
}

std::string wps_detection(std::string network_interface, std::string mac_router) {
    auto return_string = process_packets_wps(network_interface, mac_router);
    return return_string;
}