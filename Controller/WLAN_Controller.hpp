/*BSD License TODO!
   by Bastian Karschat
   02.04.2017
 */
class wlan_controller;

#ifndef WLAN_CONTROLLER_HPP
#define WLAN_CONTROLLER_HPP

#include <tins/tins.h>

#include "../console/console_app.hpp"
#include "../DeviceMonitor/Device_Monitor.hpp"
#include "../DeviceMonitor/bss.hpp"
#include "../Buffer/Packet_Buffer.hpp"
#include "../Module/Process_Packets.hpp"

class wlan_controller : public std::enable_shared_from_this<wlan_controller> {
private:
    std::mutex _controller_mutex;
    std::map <Tins::HWAddress<6>, std::shared_ptr<bss_subnet>> _bss_subnets;
    std::shared_ptr <Print_devices> _printer;
    std::shared_ptr <Process_packet> _process_packets;
    std::unique_ptr <Packet_buffer> _packet_buffer;
    Tins::NetworkInterface _iface;
    Tins::HWAddress<6> _bssid;
    std::string _psk;
public:
    wlan_controller();

    virtual ~wlan_controller();

    void init(Tins::NetworkInterface iface, Tins::HWAddress<6> bssid, std::string wordlist);

    void detect_evil_twin(Tins::HWAddress<6> hw_router);

    void noisy_automatic();

    void silent_automatic();

    void watch_ssid(std::string ssid);

    void watch_router(Tins::HWAddress<6> hw);

    void process_packets();

    void wpa2_automatic();

    void set_printer(std::shared_ptr <Print_devices> printer);

    void read_config();

    void setInterface(std::string ifaceName);

    void setFilter(Tins::HWAddress<6> bssid);

    std::shared_ptr <bss_subnet> get_bss_from_mac(Tins::HWAddress<6> mac);

    std::shared_ptr <bss_subnet> set_bss_from_mac(Tins::HWAddress<6> mac);

    void print_router();

    std::string get_psk();

};

#endif
