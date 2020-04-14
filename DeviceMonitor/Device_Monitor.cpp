#include "Device_Monitor.hpp"

std::shared_ptr <Device> Device_monitor::check_router(std::shared_ptr <Device> router_tmp) {
    auto router_mac = router_tmp->get_addresses().hw_addr;
    if (_all_router_map.find(router_mac) != _all_router_map.end()) {
        //router is already in map!
        return _all_router_map.at(router_mac);
    } else {
        //Router is not in map!
        _all_router_map.emplace(router_mac, router_tmp);
        return _all_router_map.at(router_mac);
    }
}

std::shared_ptr <Device>
Device_monitor::check_device(std::shared_ptr <Device> router_tmp, std::shared_ptr <Device> device_tmp) {
    auto device_mac = device_tmp->get_addresses().hw_addr;
    auto devices = router_tmp->get_communication_partners();
    if (devices.find(device_mac) != devices.end()) {
        //Device is already there!
        return devices.at(device_mac);
    } else {
        //Device is not in map!
        if (device_mac == router_tmp->get_addresses().hw_addr)
            return nullptr;
        device_tmp->set_name("");
        router_tmp->set_communication_partner(device_mac, device_tmp);
        _all_devices_vector.push_back(device_tmp);
        std::ofstream file;
        file.open("logs.txt", std::ios::app);
        file << router_tmp->get_addresses().hw_addr.to_string() + " " +
                device_tmp->get_addresses().hw_addr.to_string() + "\n";
        file.close();
        return devices.at(device_mac);
    }
}

Device_monitor::Device_monitor() {

}

Device_monitor::~Device_monitor() {

}

std::shared_ptr <Device>
Device_monitor::set_device(std::shared_ptr <Device> device_tmp, std::shared_ptr <Device> router_tmp) {
    if (router_tmp->get_addresses().hw_addr.is_broadcast() || device_tmp->get_addresses().hw_addr.is_broadcast())
        return nullptr;
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    return check_device(router_tmp, device_tmp);

}

std::shared_ptr <Device> Device_monitor::set_router(std::shared_ptr <Device> router_tmp) {
    if (router_tmp->get_addresses().hw_addr.is_broadcast())
        return nullptr;
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    router_tmp->set_is_router(true);
    return check_router(router_tmp);
}

std::shared_ptr <Device> Device_monitor::get_device(Tins::HWAddress<6> mac_tmp) {
//critical section
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    for (auto router : _all_router_map) {
        auto devices = router.second->get_communication_partners();
        if (devices.find(mac_tmp) != devices.end()) {
            //Device found!
            return devices.at(mac_tmp);
        }
    }
    return nullptr;
}

std::shared_ptr <Device> Device_monitor::get_first_device() {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    for (auto router : _all_router_map) {
        auto devices = router.second->get_communication_partners();
        if (devices.size() > 0) {
            return devices[0];
        } else {
            continue;
        }
    }
    return nullptr;
}


std::shared_ptr <Device> Device_monitor::get_router(Tins::HWAddress<6> mac_tmp) {
    if (_all_router_map.find(mac_tmp) != _all_router_map.end()) {
        //router found!
        return _all_router_map.at(mac_tmp);
    } else
        return nullptr;
}

std::shared_ptr <Device> Device_monitor::get_router(std::string ssid) {
    if (ssid.empty())
        return nullptr;
    for (auto router_iter : _all_router_map) {
        if (router_iter.second->get_name() == ssid)
            return router_iter.second;
        //router found!
    }
    return nullptr;
}


std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> Device_monitor::get_all_router() {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    return _all_router_map;
}

void Device_monitor::update_dos_state(Tins::HWAddress<6> mac_tmp) {
    auto dos_target = get_device(mac_tmp);
    if (dos_target != nullptr) {

        dos_target->set_dos_state();

    }
}

std::vector <std::shared_ptr<Device>> Device_monitor::get_all_devices() {
    return _all_devices_vector;
}
