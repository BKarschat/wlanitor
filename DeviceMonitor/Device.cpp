#include "Device_Monitor.hpp"

Device::~Device() {
}

Device::Device() {
    _state = std::make_unique<Dos_green_state>();
    _ip_addresses = std::map<std::string, Tins::IPv4Address>();
    _apps = std::map<std::string, Applications>();
    _devices = std::map < Tins::HWAddress < 6 >, std::shared_ptr < Device > > ();
    _radio_information = Radio_Tap_settings();
}

void Device::set_addresses(Tins::NetworkInterface::Info address) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _addresses = address;
}

Tins::NetworkInterface::Info Device::get_addresses() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _addresses;
}

void Device::set_ip_addresses(Tins::IPv4Address ip, std::string str) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _ip_addresses.insert(std::pair<std::string, Tins::IPv4Address>(str, ip));
}

std::map <std::string, Tins::IPv4Address> Device::get_ip_addresses() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _ip_addresses;
}


void Device::set_name(std::string name) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _str_name = name;
}

std::string Device::get_name() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _str_name;
}

void Device::set_is_router(bool router) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _is_router = router;
}

bool Device::get_is_router() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _is_router;
}

void Device::set_is_active(bool active) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _active = active;
}

bool Device::get_is_active() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _active;
}

void Device::set_psk(std::string tmp_psk) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _psk = tmp_psk;
}

std::string Device::get_psk() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _psk;
}

void Device::set_ptk(std::vector<unsigned char> tmp_ptk) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _ptk = tmp_ptk;
}

std::vector<unsigned char> Device::get_ptk() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _ptk;
}

void Device::set_pw_app(std::string name, Applications app) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _apps.insert(std::pair<std::string, Applications>(name, app));
}

Applications Device::get_application(std::string app) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    if (_apps.find(app) != _apps.end())
        return _apps.at(app);
    else {
        auto dummy_app = Applications();
        dummy_app.password = "NONE";
        return dummy_app;
    }
}

void Device::set_communication_partner(Tins::HWAddress<6> hw, std::shared_ptr <Device> device) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _devices.insert(std::pair < Tins::HWAddress < 6 > , std::shared_ptr < Device > > (hw, device));
}

std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> Device::get_communication_partners() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _devices;
}

void Device::set_dos_state() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _state = _state->change_state();
}

std::string Device::get_dos_state() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _state->_status;
}

void Device::set_radiotap_info(Radio_Tap_settings radio_info) {
    std::unique_lock <std::mutex> lock(_device_mutex);
    _radio_information = radio_info;
}

Radio_Tap_settings Device::get_radio_info() {
    std::unique_lock <std::mutex> lock(_device_mutex);
    return _radio_information;
}
