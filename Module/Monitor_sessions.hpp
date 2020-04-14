/* Monitor class to store router, device, ssid, psk in thread safe enviroment
by Bastian Karschat
*/

class Monitor_session;

#ifndef MONITOR_SESSIONS_HPP
#define MONITOR_SESSIONS_HPP

#include <tins/tins.h>
#include <condition_variable>
#include <mutex>
#include <thread>

typedef std::tuple<Tins::Dot11::address_type, std::string, std::string, bool, bool> router_ssid_psk_ptk_handshake_type;

class Monitor_session {
private:
    bool _stopped{false};
    bool _busy{false};
    std::unique_ptr <std::map<Tins::Dot11::address_type, router_ssid_psk_ptk_handshake_type>> _session_map;
    std::mutex _monitor_mutex;
    std::condition_variable _cond;
public:
    Monitor_session();

    virtual ~Monitor_session();

    void set_session_elements(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp,
                              std::string ssid_tmp, std::string psk_tmp, bool ptk_tmp, bool found_handshake);

    void set_psk_for_ssid(std::string ssid_tmp, std::string psk_tmp);

    bool is_session_stored(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp);

    std::string get_psk(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp);

    std::string get_ssid(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp);

    bool get_ptk_status(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp);

    bool get_handshake_status(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp);

    std::map <Tins::Dot11::address_type, router_ssid_psk_ptk_handshake_type> get_map_copy();

    void stop_monitor();

};

#endif
