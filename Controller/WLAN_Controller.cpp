#include "WLAN_Controller.hpp"


wlan_controller::wlan_controller() {
    read_config();
}

wlan_controller::~wlan_controller() {

}

void wlan_controller::init(Tins::NetworkInterface iface, Tins::HWAddress<6> bssid, std::string wordList) {
    _iface = iface;
    _printer = std::make_shared<Print_devices>();
    _packet_buffer = std::make_unique<Packet_buffer>(_iface);
    _process_packets = std::make_shared<Process_packet>();
    _process_packets->set_controller(shared_from_this());
    _process_packets->set_printer(_printer);
    _process_packets->set_bssid(bssid);
    _process_packets->setWordList(wordList);
    _bss_subnets = std::map < Tins::HWAddress < 6 >, std::shared_ptr < bss_subnet > > ();
}

void wlan_controller::detect_evil_twin(Tins::HWAddress<6> hw_router) {
    //TODO
}

void wlan_controller::setFilter(Tins::HWAddress<6> bssid) {
    _bssid = bssid;
}

void wlan_controller::noisy_automatic() {
    _packet_buffer->start_sniffing();

    bool phrase_detected = false;
    while (!phrase_detected) {

        process_packets();

        for (auto bss : _bss_subnets) {
            //_printer->print_string("Attack against: " + bss.second->get_router()->get_addresses().hw_addr.to_string());
            bss.second->start_psk_hack();

            if (bss.second->get_wpa2_hack_status() == 1) {
                phrase_detected = true;
                _psk = bss.second->get_psk();
                break;
            }

        }
        //print_router();
    }
}

std::string wlan_controller::get_psk() {
    return _psk;
}

void wlan_controller::print_router() {
    std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> map;

    for (auto bss : _bss_subnets) {
        map.insert(std::pair < Tins::HWAddress < 6 > ,
                   std::shared_ptr < Device > > (bss.first, bss.second->get_router()));
    }
    _printer->set_up_router_map(map);
    _printer->update_device();
    if (_bss_subnets.find(Tins::HWAddress<6>("5c:49:79:31:01:b8")) != _bss_subnets.end()) {
        auto subnet = _bss_subnets.at(Tins::HWAddress<6>("5c:49:79:31:01:b8"));
        _printer->print_bss(subnet);
    }
}


void wlan_controller::silent_automatic() {

}

void wlan_controller::set_printer(std::shared_ptr <Print_devices> printer) {
    _printer = printer;
}


void wlan_controller::watch_ssid(std::string ssid) {

}

void wlan_controller::watch_router(Tins::HWAddress<6> hw) {

}

void wlan_controller::process_packets() {
    if (_packet_buffer->queue_status()) {
        auto packet = _packet_buffer->get_packet();
        _process_packets->process(packet);
    }
}

void wlan_controller::wpa2_automatic() {

}

void wlan_controller::read_config() {
    // _iface = Tins::NetworkInterface("wlx00c0ca829323");
}

std::shared_ptr <bss_subnet> wlan_controller::get_bss_from_mac(Tins::HWAddress<6> mac) {
    std::unique_lock <std::mutex> lock(_controller_mutex);
    if (_bss_subnets.find(mac) != _bss_subnets.end()) {
        return _bss_subnets.at(mac);
    } else {
        return nullptr;
    }
}

std::shared_ptr <bss_subnet> wlan_controller::set_bss_from_mac(Tins::HWAddress<6> mac) {
    std::unique_lock <std::mutex> lock(_controller_mutex);
    if (_bss_subnets.find(mac) != _bss_subnets.end()) {
        return _bss_subnets.at(mac);
    } else {
        _bss_subnets.insert(std::pair < Tins::HWAddress < 6 > ,
                            std::shared_ptr < bss_subnet > > (mac, std::make_shared<bss_subnet>(_printer)));
        _bss_subnets.at(mac)->set_interface(_iface);
        return _bss_subnets.at(mac);
    }
}
