/* class to send automatically DeAuthentification packtes
 * by Bastian Karschat
 * 16.03.2017
 */

class Send_DeAuth;

#ifndef DEAUTH_HPP
#define DEAUTH_HPP

#include <tins/tins.h>
#include <fstream>
#include "../console/console_app.hpp"


//typedef std::pair<Tins::Dot11::address_type, Tins::Dot11::address_type> addr_pair;
//typedef std::map<addr_pair, Tins::Crypto::WPA2::SessionKeys> keys_map;

class Send_DeAuth {
private:

    Tins::NetworkInterface _iface;
    std::shared_ptr <Print_devices> _printer;
    bool _stopped{false};
    std::fstream fs;
    std::unique_ptr <Tins::PacketSender> _sender;

public:
    Send_DeAuth();

    virtual ~Send_DeAuth();

    void set_interface_to_send(Tins::NetworkInterface iface_tmp);

    void
    jamm_device(int count, Tins::Dot11::address_type destination, Tins::Dot11::address_type router, std::string ssid,
                Tins::NetworkInterface iface);

    void set_sender(std::shared_ptr <Tins::PacketSender> sender_tmp);

    void end_jamming(bool);

    void set_printer(std::shared_ptr <Print_devices> print_tmp);
};


#endif
