/*TODO BSD License
   by Bastian Karschat
   02.04.2017*/

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <tins/tins.h>
#include <mutex>
#include "DeAuth_States/Dos_State.hpp"

struct Applications {
    std::string password;
    int srcPort, dstPort;
    bool isTcp;
    Tins::IPv4Address serverAddresses;
};

struct Radio_Tap_settings {
    //TODO RadioTap mcs field
    int datarate;
    //encryption 0 = no, 1 = wep, 2 = wpa2
    int encryption;
    //0 = 2,4, 1 = 5 GHz is used!
    int channel_type;
    //WLAN channel 1...14
    int channel_freq;
};

//stores the important Data of an WLAN device
class Device {
public:
    Device();

    virtual ~Device();

    void set_addresses(Tins::NetworkInterface::Info addresses);

    Tins::NetworkInterface::Info get_addresses();

    void set_ip_addresses(Tins::IPv4Address ip, std::string str);

    std::map <std::string, Tins::IPv4Address> get_ip_addresses();

    void set_name(std::string name);

    std::string get_name();

    void set_is_router(bool router);

    bool get_is_router();

    void set_is_active(bool active);

    bool get_is_active();

    void set_psk(std::string);

    std::string get_psk();

    void set_ptk(std::vector<unsigned char>);

    std::vector<unsigned char> get_ptk();

    void set_pw_app(std::string name, Applications app);

    Applications get_application(std::string name);

    void set_communication_partner(Tins::HWAddress<6>, std::shared_ptr <Device>);

    std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> get_communication_partners();

    void set_dos_state();

    std::string get_dos_state();

    void set_radiotap_info(Radio_Tap_settings radio_info);

    Radio_Tap_settings get_radio_info();

private:
    std::mutex _device_mutex;
//TODO GETTER AND SETTER ! Monitor object!
/*ip Addressand hw Address
   Tins::Dot11::address_type hw_Address;
   Tins::IPv4Address ip4_Address;
   Tins::IPv6Address  ip6_Address;
   are stored in info! */
    Tins::NetworkInterface::Info _addresses;
//more
    std::map <std::string, Tins::IPv4Address> _ip_addresses;
//name
    std::string _str_name{};
//router
    bool _is_router = false;
//active
    bool _active = false;
// store psk
    std::string _psk{};
//store ptk
    std::vector<unsigned char> _ptk;
//passwords the application and the password have the same id
    std::map <std::string, Applications> _apps;
//store communcication Partners!
    std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> _devices;

//watcher
//  std::shared_ptr<Device_Watcher_State> watcher;
    std::unique_ptr <Dos_state> _state;
//radio Information
    Radio_Tap_settings _radio_information;
};


#endif
