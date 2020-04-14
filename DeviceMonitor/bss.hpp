/* TODO License*/

/* class should gather devices wich are connected to a router.
   To get an better overview about what is going on in the bss.*/
class bss_subnet;

#ifndef BSS_HPP
#define BSS_HPP

#include <tins/tins.h>
#include <future>
#include <unistd.h>
#include <sys/types.h>
#include <chrono>
#include <map>
#include <list>


#include "../console/console_app.hpp"
#include "Device_Monitor.hpp"
#include "../Module/DeAuth.hpp"

class bss_subnet {
private:
    std::chrono::time_point <std::chrono::system_clock> _timestamp;
    std::mutex _bss_mutex;
    std::unique_ptr <Send_DeAuth> _deauth_sender;
    std::shared_ptr <Device> _router;
    std::list <std::shared_ptr<bss_subnet>> _same_ssid_bss_subnets;
    std::string _psk{""};
    std::string _ssid{""};
    bool _psk_hack_in_progress = {false};
    bool _wait_for_jamm = {false};
    int _wpa2_hack_status{0};
    std::shared_ptr <Print_devices> _printer;
    Tins::NetworkInterface _iface;
    std::shared_ptr <Device> _host_hack;
    std::vector <std::shared_ptr<Device>> _hacked_devices_vector;
    std::vector <std::shared_ptr<Device>> _all_devices_vector;
    int _device_hack_counter{0};
public:
    std::unique_ptr <Device_monitor> _device_monitor;

    bss_subnet(std::shared_ptr <Print_devices>);

    virtual ~bss_subnet();

    std::shared_ptr <Device> get_device(Tins::HWAddress<6> hw);

    std::shared_ptr <Device> add_device(std::shared_ptr <Device> dev);

    std::shared_ptr <Device> get_router();

    std::shared_ptr <Device> set_router(std::shared_ptr <Device> router_tmp);

    std::string get_ssid();

    void set_ssid(std::string);

    std::string get_psk();

    void set_psk(std::string);

    void set_same_bss(std::shared_ptr <bss_subnet> bss_subnet);

    std::list <std::shared_ptr<bss_subnet>> get_bss_same_ssid_list();

    bool get_psk_hack_status();

    void set_psk_hack_status();

    void start_psk_hack();

    void spread_psk();

    void set_wpa2_hack_status(int status);

    int get_wpa2_hack_status();

    void set_interface(Tins::NetworkInterface iface);

    std::vector <std::shared_ptr<Device>> get_hacked_devices();

    std::vector <std::shared_ptr<Device>> get_devices();
};

#endif
