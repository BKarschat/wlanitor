/*TODO BSD License
   by Bastian Karschat
   02.04.2017*/

#ifndef DEVICE_MONITOR_HPP
#define DEVICE_MONITOR_HPP

#include <tins/tins.h>
#include <mutex>
#include <fstream>
#include "DeAuth_States/Dos_State.hpp"
#include "Device.hpp"


class Device_monitor {
private:
    std::mutex _monitor_mutex;
    std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> _all_router_map;

    std::shared_ptr <Device> check_router(std::shared_ptr <Device> router_tmp);

    std::shared_ptr <Device> check_device(std::shared_ptr <Device> router, std::shared_ptr <Device> device_tmp);

    std::vector <std::shared_ptr<Device>> _all_devices_vector;

public:
    Device_monitor();

    virtual ~Device_monitor();

    std::shared_ptr <Device> set_device(std::shared_ptr <Device> device_tmp, std::shared_ptr <Device> router_tmp);

    std::shared_ptr <Device> set_router(std::shared_ptr <Device> tmp_router);

    std::shared_ptr <Device> get_device(Tins::HWAddress<6> mac_tmp);

    std::shared_ptr <Device> get_first_device();

    std::shared_ptr <Device> get_router(Tins::HWAddress<6> mac_tmp);

    std::shared_ptr <Device> get_router(std::string ssid);

    std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> get_all_router();

    std::vector <std::shared_ptr<Device>> get_all_devices();

    void update_dos_state(Tins::HWAddress<6> mac_tmp);
};


#endif
