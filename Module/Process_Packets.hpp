/*TODO BSD License
   by Bastian Karschat
   02.04.2017*/

class Process_packet;

#ifndef PROCESS_PACKET_HPP
#define PROCESS_PACKET_HPP

#include <tins/tins.h>
#include <memory>
#include <vector>
#include <bitset>
#include <future>
#include <functional>
#include <iostream>
#include <fstream>

#include "../DeviceMonitor/Device_Monitor.hpp"
#include "../console/console_app.hpp"
#include "Wpa2_password_hack.hpp"
#include "../Controller/WLAN_Controller.hpp"


class Process_packet {
private:

    std::vector <Tins::Packet> _packet_vector;
    std::shared_ptr <wlan_controller> _controller;
    std::shared_ptr <Device> _src_device;
    std::shared_ptr <Device> _dst_device;
    std::shared_ptr <Device> _router;
    std::shared_ptr <Device> _host;
    std::shared_ptr <Print_devices> _printer;
    std::unique_ptr <Wpa2_pw_hack> _wpa2_hack;
    std::unique_ptr <Tins::Crypto::WPA2Decrypter> _decrypter;
    Radio_Tap_settings radio_info_tmp;
    std::string ssid;
    std::ofstream file;
    Tins::HWAddress<6> _bssid;

public:
    Process_packet();

    virtual ~Process_packet();

    void process(Tins::Packet packet_tmp);

    void management_wlan(Tins::Dot11ManagementFrame *mgmnt_frame);

    void handle_deauth(Tins::Dot11Deauthentication *deauth_frame);

    void handle_data(Tins::Dot11Data *data_frame);

    void handle_beacon(Tins::Dot11Beacon *beacon_frame);

    void handle_radio(Tins::RadioTap *radio_frame);

    void handle_ethernet(Tins::EthernetII &ether_frame);

    void handle_ip(Tins::EthernetII &ether_frame, Tins::IP &ip_frame);

    void handle_handshake(Tins::Dot11Data *data_frame);

    template<class T>
    Tins::HWAddress<6> src_addr(T *dot11_frame);

    template<class T>
    Tins::HWAddress<6> dst_addr(T *dot11_frame);

    void check_device(Tins::HWAddress<6> mac_src, Tins::HWAddress<6> mac_dst);

    void set_printer(std::shared_ptr <Print_devices> printer);

    void set_controller(std::shared_ptr <wlan_controller> controller);

    void set_bssid(Tins::HWAddress<6> bssid);

    void setWordList(std::string fileName);

    Tins::NetworkInterface::Info check_ip_in_device(std::shared_ptr <Device>, Tins::NetworkInterface::Info info);

    Tins::EthernetII make_ethernet_packet(Tins::Dot11Data *);
};

#endif
