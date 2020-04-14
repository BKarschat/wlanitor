/*try to hack wpa2 pw
   by Bastian Karschat
 */
class Wpa2_pw_hack;

#ifndef WPA2_PASSWORD_HACK_HPP
#define WPA2_PASSWORD_HACK_HPP

#include <tins/tins.h>
#include <fstream>
#include <future>
#include "../console/console_app.hpp"
#include "../DeviceMonitor/Device_Monitor.hpp"
#include "DeAuth.hpp"


class Wpa2_pw_hack {
private:
    std::shared_ptr <Print_devices> _printer;
    std::string wordlist;
//std::unique_ptr<Tins::RSNHandshakeCapturer> rsnhand;
    Tins::RSNHandshakeCapturer rsnhand;
    std::string _psk{""};
    std::vector<unsigned char> _ptk;
    Tins::Crypto::WPA2::SessionKeys _session_keys;
    std::map <Tins::HWAddress<6>, std::string> test_map;

//std::thread hacking;
//bool _running{false};


public:
    Wpa2_pw_hack();

    virtual ~Wpa2_pw_hack();

    int start_pw_hack(Tins::Dot11Data *data_frame, std::string ssid, std::shared_ptr <Device> router,
                      std::shared_ptr <Device> host);

    void stop_thread();

    void set_printer(std::shared_ptr <Print_devices> printer);

    std::string get_psk();

    std::vector<unsigned char> get_ptk();

    Tins::Crypto::WPA2::SessionKeys get_session_keys();

    void setWordList(std::string filename);

};

#endif
