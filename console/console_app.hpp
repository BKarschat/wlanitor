/* uses ncurses to interact with user
   by Bastian Karschat 5.3.2017
 */
class Print_devices;

#ifndef CONSOLE_APP_HPP
#define CONSOLE_APP_HPP

#include "../DeviceMonitor/Device_Monitor.hpp"
#include "../DeviceMonitor/bss.hpp"
#include <unistd.h>
#include <ncurses.h>                    /* ncurses.h includes stdio.h */
#include <string>
#include <memory>
#include <map>
#include <tins/tins.h>
#include <sstream>

typedef std::map <Tins::Dot11::address_type, std::vector<std::shared_ptr < Device>> >
router_map_type;
typedef std::map <Tins::Dot11::address_type, std::shared_ptr<Device>> device_map_type;


class Print_devices {
private:
    std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> _all_router_map;
    std::mutex _console_mutex;
    WINDOW *cur_window;

    void init();

    int refresh_lines(int printed_line, std::shared_ptr <Device>);

    void line_to_device_mapper();

    std::map<int, std::shared_ptr<Device> > line_map;

    Device &get_device_from_mac(Tins::Dot11::address_type &tmp_mac);

//  std::shared_ptr<router_map_type> router_map;
public:
    Print_devices();

    ~Print_devices();

    void start_printing();

    void print_bss(std::shared_ptr <bss_subnet> subnet);

    void print_string(std::string print);

    void set_up_router_map(std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> router_map_tmp);

    int update_device();
};


#endif
