/* uses ncurses to interact with user
   by Bastian Karschat 5.3.2017
 */


#include "console_app.hpp"


Print_devices::Print_devices() {
    cur_window = initscr();

    raw();
    //damit der fucking getch nicht blockt
    nodelay(stdscr, TRUE);
    noecho();
    wprintw(cur_window, "Start Sniffing");
    wrefresh(cur_window);
}

Print_devices::~Print_devices() {
    endwin();
}

void Print_devices::start_printing() {
    if (cur_window == nullptr)
        return;
    wprintw(cur_window, "Start Sniffing");
    wrefresh(cur_window);
    int ch;
    //while(1)
    {
        ch = getch();
        if (ch == 113) {
            endwin();
            cur_window = nullptr;
            //std::cout << "End program with Ctrl-c!" << std::endl;
            return;
        }
        if (cur_window == nullptr)
            return;
        wrefresh(cur_window);
        update_device();
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Print_devices::print_string(std::string print) {
    std::unique_lock <std::mutex> lock(_console_mutex);
    if (cur_window == nullptr) {
        endwin();
        return;
    }
    //move(25, 0);    // move to begining of line
    //clrtoeol();    // clear line
    //mvwprintw(cur_window, 25, 5,"%s", print.c_str());
    //wrefresh(cur_window);
    std::cout << print << std::endl;
}


int Print_devices::update_device() {
    if (getch() == 113) {
        endwin();
        cur_window = nullptr;
        return 1;
    }
    if (cur_window == nullptr)
        return 1;
/*  if(device_connected_map == nullptr)
   {
    wprintw(cur_window,"Connected Device Map is uninit!");
    //wrefresh(cur_window);
    return 0;
   }
   if(all_devices_map == nullptr)
   {
   //  wprintw(cur_window,"All Devices Map is uninit!");
    wrefresh(cur_window);
    return 0;
   }
   if(_all_router_map == nullptr)
   {
   //    wprintw(cur_window,"Router Map is uninit!");
    wrefresh(cur_window);
    return 0;
   }
 */

    //werase(cur_window);
    //map devices to line
    line_to_device_mapper();

    //reference variables (which get optimised away if unused)
    for (auto iter : line_map) {
        refresh_lines(iter.first, iter.second);
        wrefresh(cur_window);
    }
    return 0;
}

void Print_devices::line_to_device_mapper() {
    int counter = 0;

    /*for(auto elem : *all_devices_map.get())
       {
       line_map[counter] = elem.second;
       counter++;
       }
     */



    std::vector <Tins::HWAddress<6>> used_mac;
    for (auto iter : _all_router_map) {
        auto router = iter.second;
        if (std::find(used_mac.begin(), used_mac.end(), iter.first) == used_mac.end()) {
            //mac not used before!
            used_mac.push_back(iter.first);
            line_map[counter] = router;
            counter++;
            /*for(auto device : router->get_communication_partners())
               {
                    used_mac.push_back(device.first);
                    line_map[counter] = device.second;
                    counter++;
               }*/
        }

    }

/*
   for(auto device : *all_devices_map.get() )
   {
    auto device_mac_tmp = device.first;
    if(std::find(used_mac.begin(), used_mac.end(), device_mac_tmp) == used_mac.end())
    {
      used_mac.push_back(device_mac_tmp);
      if(router_map->find(device_mac_tmp) != router_map->end())
      {
        line_map[counter] = device.second;
        counter++;
        for(auto iter : router_map->at(device_mac_tmp))
        {
      //  auto tmp_device = get_device_from_mac(iter->addresses.hw_addr);
        //if(tmp_device != nullptr)
        //{
          line_map[counter] = iter;
          counter++;
        }
      }else{
        line_map[counter] = device.second;
        counter++;
      }
    }
   }
 */
/*for(auto router_tmp : *router_map.get())
   {
   auto router_key = router_tmp.first;
   auto device_map = router_tmp.second;
   auto tmp_router = get_device_from_mac(router_key);
   //if(tmp_router != nullptr)
   //{
    line_map[counter] = tmp_router;
    counter++;
    //int device_counter = 0;
    for(auto iter : device_map)
    {
    //  auto tmp_device = get_device_from_mac(iter->addresses.hw_addr);
      //if(tmp_device != nullptr)
      //{
        line_map[counter] = iter;
        counter++;
    }
   }
    //}
   //}
   }*/
}


/*std::shared_ptr<Device> Print_devices::get_unique_device(std::shared_ptr<Device> tmp_dev)
   {
   for( auto iter : line_map)
   {
    if(iter.second == tmp_dev)
      return nullptr;
   }
   return tmp_dev;
   }
 */

//Device & Print_devices::get_device_from_mac(Tins::Dot11::address_type &tmp_mac)
//{
//  if(all_devices_map->find(tmp_mac) != all_devices_map->end())
//    return all_devices_map->at(tmp_mac);
//}



int Print_devices::refresh_lines(int printed_line, std::shared_ptr <Device> dev) {
    std::unique_lock <std::mutex> lock(_console_mutex);
    if (cur_window == nullptr)
        return 1;
    //iterate over infos and print!
    std::string ip{dev->get_addresses().ip_addr.to_string()}, mac{dev->get_addresses().hw_addr.to_string()};
    std::string channel{" "};
    if (!(std::to_string(dev->get_radio_info().channel_freq)).empty())
        channel = std::to_string(dev->get_radio_info().channel_freq);
    std::string ips{" "};
    std::string status{" "};
    status = dev->get_dos_state() + dev->get_psk();

    auto tmp = dev->get_ip_addresses();
    for (auto elem : tmp) {
        ips += std::get<0>(elem) + " " + std::get<1>(elem).to_string();
    }
    if (dev->get_is_router()) {
        //  mvwprintw(cur_window, 0, printed_line, "TEST %s", ip);
//    std::cout << ip << std::endl;
        move(printed_line, 0); // move to begining of line
        clrtoeol(); // clear line
        //mvwprintw(cur_window, printed_line, 0, "Router %i %s %s %s %s %s %s\n", printed_line, mac.c_str(), ip.c_str(), channel.c_str(), ips.c_str(), dev->get_name().c_str(), status.c_str());
    }
    return 0;
}


void Print_devices::print_bss(std::shared_ptr <bss_subnet> subnet) {
    std::string print{""};
    std::string print2{""};
    auto router = subnet->get_router();
    print += " " + router->get_addresses().hw_addr.to_string();
    print_string(std::to_string(subnet->get_devices().size()) + "          " +
                 std::to_string(subnet->get_hacked_devices().size()));

    for (auto host : subnet->get_hacked_devices()) {
        print2 += " " + host->get_addresses().hw_addr.to_string();
    }
    for (auto host : subnet->get_devices()) {
        print += " " + host->get_addresses().hw_addr.to_string();
    }
    std::unique_lock <std::mutex> lock(_console_mutex);
    if (cur_window == nullptr) {
        endwin();
        return;
    }
    move(27, 0); // move to begining of line
    clrtoeol(); // clear line
    //mvwprintw(cur_window, 27, 5,"%s", print.c_str());
    wrefresh(cur_window);
    move(30, 0); // move to begining of line
    clrtoeol(); // clear line
    //mvwprintw(cur_window, 30, 5,"%s", print2.c_str());
    wrefresh(cur_window);

}


void Print_devices::set_up_router_map(std::map <Tins::HWAddress<6>, std::shared_ptr<Device>> router_map_tmp) {
    _all_router_map = router_map_tmp;
}
