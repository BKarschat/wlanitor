#include "Process_Packets.hpp"

Process_packet::Process_packet() {
    _wpa2_hack = std::make_unique<Wpa2_pw_hack>();
    _decrypter = std::make_unique<Tins::Crypto::WPA2Decrypter>();
}

Process_packet::~Process_packet() {}

void Process_packet::set_controller(std::shared_ptr <wlan_controller> controller) {
    _controller = controller;
}

void Process_packet::set_bssid(Tins::HWAddress<6> bssid) {
    _bssid = bssid;
}

void Process_packet::setWordList(std::string fileName) {
    _wpa2_hack.get()->setWordList(fileName);
}


void Process_packet::process(Tins::Packet packet_tmp) {
    try {
        auto pdu = packet_tmp.pdu();
        auto radio = pdu->find_pdu<Tins::RadioTap>();
        auto ether = pdu->find_pdu<Tins::EthernetII>();

        if (radio != nullptr) {
            handle_radio(radio);
            auto mgmnt_frame = pdu->find_pdu<Tins::Dot11ManagementFrame>();
            auto data_frame = pdu->find_pdu<Tins::Dot11Data>();
            auto deauth = mgmnt_frame->find_pdu<Tins::Dot11Deauthentication>();


            if (mgmnt_frame != nullptr)
                management_wlan(mgmnt_frame);
            else if (data_frame != nullptr)
                handle_data(data_frame);
            else if (deauth != nullptr)
                handle_deauth(deauth);
        } else if (ether != nullptr)
            handle_ethernet(*ether);
    }
    catch (...) {
        //TODO
        _router = nullptr;

        _src_device = nullptr;
        _dst_device = nullptr;
        _host = nullptr;
        ssid.clear();
        return;
    }



    /*for(auto iter : map)
       {
       auto a = iter.second;
       std::cout << "Router: " << a->addresses.hw_addr << " " << a->str_name ;
       for(auto i : a->devices)
       {
        auto b = i.second;
        std::cout << "Device: " << b->addresses.hw_addr << " " << b->str_name ;
       }
       }*/

    _router = nullptr;
    _host = nullptr;
    _src_device = nullptr;
    _dst_device = nullptr;
    ssid.clear();
}

Tins::EthernetII Process_packet::make_ethernet_packet(Tins::Dot11Data *dot11) {
    if (dot11->from_ds() && !dot11->to_ds()) {
        return Tins::EthernetII(dot11->addr1(), dot11->addr3());
    } else if (!dot11->from_ds() && dot11->to_ds()) {
        return Tins::EthernetII(dot11->addr3(), dot11->addr2());
    } else {
        return Tins::EthernetII(dot11->addr1(), dot11->addr2());
    }
}

template<class T>
Tins::HWAddress<6> Process_packet::src_addr(T *dot11_frame) {
    if (!dot11_frame->from_ds() && !dot11_frame->to_ds()) {
        return dot11_frame->addr2();
    }
    if (!dot11_frame->from_ds() && dot11_frame->to_ds()) {
        return dot11_frame->addr2();
    }
    if (dot11_frame->from_ds() && dot11_frame->to_ds())
        return dot11_frame->addr4();
    return dot11_frame->addr3();
}

template<class T>
Tins::HWAddress<6> Process_packet::dst_addr(T *dot11_frame) {
    if (!dot11_frame->from_ds() && !dot11_frame->to_ds()) {
        return dot11_frame->addr1();
    }
    if (!dot11_frame->from_ds() && dot11_frame->to_ds()) {
        return dot11_frame->addr3();
    }
    if (dot11_frame->from_ds() && dot11_frame->to_ds())
        return dot11_frame->addr3();
    return dot11_frame->addr1();
}

void Process_packet::check_device(Tins::HWAddress<6> mac_src, Tins::HWAddress<6> mac_dst) {
    //std::ofstream file;
    //file.open("logs.txt");
    //file.close();
    /*First check if router is known*/
    auto bss = _controller->get_bss_from_mac(mac_src);
    if (bss == nullptr) {

        //router is not known! But could be also Device! so check mac_dst
        bss = _controller->get_bss_from_mac(mac_dst);
        if (bss == nullptr) {
            //check for ssid!
            /*_router = _device_monitor->get_router(ssid);
               if(_router != nullptr)
               {
               //_src is router!
               if(_router->addresses.hw_addr == _src_device->addresses.hw_addr)
               {

                if(_router->str_name.empty())
                  _router->str_name = ssid;
                _src_device = _router;
                _host = _device_monitor->set_device(_dst_device, _src_device);
                _dst_device = _host;
               }else if(_router->addresses.hw_addr == _dst_device->addresses.hw_addr)
               {
                //Dst is Router so set Device
                if(_router->str_name.empty())
                  _router->str_name = ssid;
                _dst_device = _router;
                _host = _device_monitor->set_device(_src_device, _dst_device);
                _src_device = _host;
               }else
                _printer->print_string("New Devices detected!");
               //Sender and Dst are completly new! No Chance to detect Router now
               }*/

        } else {
            _router = bss->get_router();
            //file.open("logs.txt");
            //file << "check device";
            //file << "router: "<< _router->get_addresses().hw_addr.to_string() << " _dst_device is router";
            //file.close();
            //Dst is Router so set Device
            if (_router->get_name().empty())
                _router->set_name(ssid);
            _dst_device = _router;
            _host = bss->add_device(_src_device);
            _src_device = _host;
            //file.open("logs.txt");
            //file << "host: " << _host->get_addresses().hw_addr.to_string() << " _src_device is host";
            //file.close();
        }
    } else {

        //Router is known! Check Device!
        _router = bss->get_router();
        //file.open("logs.txt");
        //file << "check device";
        //file << "router: "<< _router->get_addresses().hw_addr.to_string() << " _src_device is router";
        //file.close();
        if (_router->get_name().empty())
            _router->set_name(ssid);
        _src_device = _router;
        _host = bss->add_device(_dst_device);

        _dst_device = _host;
        //file.open("logs.txt");
        //file << "host: " << _host->get_addresses().hw_addr.to_string() << " _dst_device is host";
        //file.close();

    }
    //file.close();
}

/*Handle Management Frames and call if possible the right functions*/
void Process_packet::management_wlan(Tins::Dot11ManagementFrame *mgmnt_frame) {
//        _printer->print_string("MGMNET");
    _src_device = std::make_shared<Device>();
    _dst_device = std::make_shared<Device>();
    auto mac_src = mgmnt_frame->addr2();
    auto mac_dst = mgmnt_frame->addr1();

    if (mac_src == _bssid || mac_dst == _bssid) {
        //std::cout << "Frame BSSID detected" << std::endl;
    } else {
        return;
    }


    auto _src_device_info = _src_device->get_addresses();
    _src_device_info.hw_addr = mac_src;
    _src_device->set_addresses(_src_device_info);
    auto _dst_device_info = _dst_device->get_addresses();
    _dst_device_info.hw_addr = mac_dst;
    _dst_device->set_addresses(_dst_device_info);
    ssid = mgmnt_frame->ssid();
    _src_device->set_name(ssid);
    _dst_device->set_name(ssid);

    //check for known router and set device!
    if (_src_device != nullptr) {
        _src_device->set_radiotap_info(radio_info_tmp);
        if (_src_device->get_is_router() && !_src_device->get_psk().empty()) {
            //Router and psk is there! get ptk for device!

        }
    }
    if (_dst_device != nullptr) {
        _dst_device->set_radiotap_info(radio_info_tmp);
        if (_dst_device->get_is_router() && !_dst_device->get_psk().empty()) {
            //Get ptk for device!

        }
    }


//FIXME bssid address data packages?
//  std::cout << "Test mgmnt" ;


    auto beacon = mgmnt_frame->find_pdu<Tins::Dot11Beacon>();
    auto deauth = mgmnt_frame->find_pdu<Tins::Dot11Deauthentication>();

    if (beacon != nullptr)
        handle_beacon(beacon);
    else if (deauth != nullptr) {
        check_device(mac_src, mac_dst);
        handle_deauth(deauth);
    } else {
        //get some infos
        //TODO
    }

}

void Process_packet::handle_deauth(Tins::Dot11Deauthentication *deauth_frame) {

    //      _printer->print_string("deauth_frame");

    //FIXME Devices wich are not know as Router / Device are not supported!
    auto bss = _controller->get_bss_from_mac(_router->get_addresses().hw_addr);
    if (_router != nullptr) {
        bss->_device_monitor->update_dos_state(_router->get_addresses().hw_addr);
    }
    if (_host != nullptr) {
        bss->_device_monitor->update_dos_state(_host->get_addresses().hw_addr);
    }
}

void Process_packet::handle_data(Tins::Dot11Data *data_frame) {
    //std::ofstream file;
    //file.open("logs.txt");
    //file << " data ";
    //file.close();

    //    _printer->print_string("Data");

    //TODO send Request to handle with SSID

    //std::cout << "DATA!" << std::endl;
    //std::cout << "src: "<<data_frame->src_addr() << " dst: " << data_frame->dst_addr() << " bssid " << data_frame->bssid_addr() << std::endl;
    _src_device = std::make_shared<Device>();
    _dst_device = std::make_shared<Device>();
    auto mac_src = src_addr(data_frame);
    auto mac_dst = dst_addr(data_frame);
    auto mac_bssid = data_frame->bssid_addr();


    if (mac_src == _bssid || mac_dst == _bssid) {
        std::cout << "DATA BSSID detected" << ":" << mac_bssid.to_string() << std::endl;
    } else {
        return;
    }

    auto _src_device_info = _src_device->get_addresses();
    _src_device_info.hw_addr = mac_src;
    _src_device->set_addresses(_src_device_info);
    auto _dst_device_info = _dst_device->get_addresses();
    _dst_device_info.hw_addr = mac_dst;
    _dst_device->set_addresses(_dst_device_info);
    ssid = "";
    //check for known router and set device!
    if (mac_src.is_broadcast()) {
        //std::cout << mac_src << std::endl;
        check_device(mac_bssid, mac_dst);
        if (_router == nullptr) {
            if (mac_bssid != mac_dst)
                check_device(mac_bssid, mac_dst);
        }
    } else if (mac_dst.is_broadcast()) {
        //std::cout << mac_dst << std::endl;
        check_device(mac_src, mac_bssid);
        if (_router == nullptr) {
            if (mac_bssid != mac_src)
                check_device(mac_src, mac_bssid);
        }
    } else
        check_device(mac_src, mac_dst);
    /* no router? ok check bssid!*/
    if (_router == nullptr && !mac_dst.is_broadcast() && !mac_src.is_broadcast()) {
        if (mac_bssid != mac_src)
            check_device(mac_src, mac_bssid);
        if (mac_bssid != mac_dst)
            check_device(mac_bssid, mac_dst);
    }

    auto eapol = data_frame->find_pdu<Tins::RSNEAPOL>();
    if (eapol != nullptr) {
        handle_handshake(data_frame);
    } else {
        std::cout << "EAPOL NOT Detected" << std::endl;
    }
    //auto tmp_router_mac = data_frame->bssid_addr();

    //printer->print_string(tmp_router_mac.to_string() + " + " + tmp_mac_dest.to_string() +  " + " + tmp_mac_src.to_string());
    /* TODO!if(decrypt_succes == 1){
       printer->print_string("NO SUCCES!");
            if(!monitor_session->is_session_stored(tmp_device_mac, tmp_router_mac))
            {
              printer->print_string(ssid);
              monitor_session->set_session_elements(tmp_device_mac, tmp_router_mac, ssid, "", false, false);
            }

            }else if(decrypt_succes == 0)
            {
              //succes!

              printer->print_string("SUCCES!!");
              if(!monitor_session->get_ptk_status(tmp_device_mac, tmp_router_mac))
              {
                auto psk = monitor_session->get_psk(tmp_device_mac, tmp_router_mac);
                printer->print_string("SUCCES!!1");
                monitor_session->set_session_elements(tmp_device_mac, tmp_router_mac, ssid, psk, true, true);
              }
            }
     */
    //  _printer->print_string("Tedst decrypt");
    if (_decrypter->decrypt(*data_frame)) {
        //file.open("logs.txt");
        //file << " decrypt ";
        //file.close();
        _printer->print_string("Decrypt!");
        auto &snap = data_frame->rfind_pdu<Tins::SNAP>();
        auto ether_frame = make_ethernet_packet(data_frame);
        ether_frame.inner_pdu(snap.release_inner_pdu());

        handle_ethernet(ether_frame);
    }
}

void Process_packet::handle_beacon(Tins::Dot11Beacon *beacon_frame) {
    //  _printer->print_string("beacon_frame");
    if (_router == nullptr) {
        //std::ofstream file;
        //file.open("logs.txt");
        //file << " beacon_frame ";
        //file.close();

        //new Router found!
        //std::cout <<_src_device->str_name ;
        auto bss = _controller->get_bss_from_mac(_src_device->get_addresses().hw_addr);
        if (bss == nullptr) {
            bss = _controller->set_bss_from_mac(_src_device->get_addresses().hw_addr);
        }

        _router = bss->set_router(_src_device);
        //file.open("logs.txt");
        //file << " router: " << _router->get_addresses().hw_addr.to_string() << " is _src_device ";
        //file.close();
        if (_router == nullptr)
            return;
        //else
        //  _printer->print_string("New router found via beacon: " + _router->addresses.hw_addr.to_string() + " SSID: " + ssid );
    }


}

void Process_packet::handle_radio(Tins::RadioTap *radio_frame) {
    //_printer->print_string("radio_frame");

    int channel = 0, wep = 0, channel_type = 0;
    //check first wich bi it set!
    std::bitset<20> present_flags(radio_frame->present());
    if (present_flags.test(1)) {
        //FLAGS set!
        auto encryption = radio_frame->flags();
        if (encryption == 4) {
            //WEP encypted!
            wep = 1;
        }
    }
    if (present_flags.test(3)) {
        //CHANNEL 5Ghz or 2,4 is set!
        channel_type = radio_frame->channel_type();

        channel = Tins::Utils::mhz_to_channel(radio_frame->channel_freq());
    }
    radio_info_tmp.encryption = wep;
    radio_info_tmp.channel_type = channel_type;
    radio_info_tmp.channel_freq = channel;

    return;
}


void Process_packet::handle_ethernet(Tins::EthernetII &ether_frame) {
    //TODO
    //_printer->print_string("Handle ETHERNET!!");
    auto ip_frame = ether_frame.rfind_pdu<Tins::IP>();
    handle_ip(ether_frame, ip_frame);
}

void Process_packet::handle_ip(Tins::EthernetII &ether_frame, Tins::IP &ip_frame) {
    if (_dst_device != nullptr && _src_device != nullptr) {
        auto addresses = _src_device->get_addresses();
        addresses.ip_addr = ip_frame.src_addr();
        addresses.hw_addr = ether_frame.src_addr();
        addresses = check_ip_in_device(_src_device, addresses);
        _src_device->set_addresses(addresses);

        addresses.ip_addr = ip_frame.dst_addr();
        addresses.hw_addr = ether_frame.dst_addr();
        addresses = check_ip_in_device(_dst_device, addresses);
        _dst_device->set_addresses(addresses);
    }
    _printer->print_string(ip_frame.src_addr().to_string() + ip_frame.dst_addr().to_string());
}


void Process_packet::set_printer(std::shared_ptr <Print_devices> printer) {
    _printer = printer;
    _wpa2_hack->set_printer(_printer);
}


void Process_packet::handle_handshake(Tins::Dot11Data *data_frame) {
    //std::ofstream file;
    //_printer->print_string("handle_handshake");
    //file.open("logs.txt");
    //file << " handle_handshake ";
    //file.close();

    std::cout << "Handle Handshake" << std::endl;

    if (_router != nullptr && _host != nullptr) {
        if (!_host->get_ptk().empty())
            return;
        auto bss = _controller->get_bss_from_mac(_router->get_addresses().hw_addr);
        //auto future = std::async(std::launch::async,std::bind(&Wpa2_pw_hack::start_pw_hack, _wpa2_hack.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,std::placeholders::_4), std::ref(eapol_frame), _router->get_name(), _router, _host);
        //int result = future.get();
        //file.open("logs.txt");
        //file << " bss: " << _router->get_addresses().hw_addr.to_string() << " host: " << _host->get_addresses().hw_addr.to_string();
        //file.close();

        _printer->print_string("HACK FUCKING PSK!");
        int result = _wpa2_hack->start_pw_hack(data_frame, _router->get_name(), _router, _host);
        switch (result) {
            case 1: {
                bss->set_psk(_wpa2_hack->get_psk());
                bss->set_wpa2_hack_status(1);
                //                      void add_decryption_keys(const addr_pair &addresses,
                //
                typedef std::pair <Tins::HWAddress<6>, Tins::HWAddress<6>> addr_pair;
                addr_pair connection_pair(_router->get_addresses().hw_addr, _host->get_addresses().hw_addr);
                _decrypter->add_ap_data(_wpa2_hack->get_psk(), _router->get_name());
                _decrypter->add_decryption_keys(connection_pair, _wpa2_hack->get_session_keys());
                _host->set_ptk(_wpa2_hack->get_ptk());
                //got psk!
                std::cout << "wpa2_hack_status 1" << std::endl;
            }
                break;
            case 2:
                bss->set_wpa2_hack_status(2);
                //need whole handshake!
                std::cout << "wpa2_hack_status 2" << std::endl;
                break;
            case 3:
                bss->set_wpa2_hack_status(3);
                //Wrong Wordlist
                std::cout << "wpa2_hack_status 3" << std::endl;
                break;
            case 4:
                bss->set_wpa2_hack_status(4);
                //something went wrong
                std::cout << "wpa2_hack_status 4" << std::endl;
                break;
        }

        std::cout << "Handle Handshake finish" << std::endl;
    } else {
        // nothing to do ...
    }
//           TODO check COntroller!*/

    //std::async(std::launch::async,std::bind(&Wpa2_pw_hack::start_pw_hack, wpa2_pw_hack.get(), std::placeholders::_1, std::placeholders::_2), std::ref(pack), ssid);
    //_running = true;
    //}
    //file.close();
}

Tins::NetworkInterface::Info
Process_packet::check_ip_in_device(std::shared_ptr <Device> dev, Tins::NetworkInterface::Info info2) {
    //check if ip 0.0.0.0
    auto info = dev->get_addresses();
    Tins::IPv4Address tmp_null("0.0.0.0");
    if (info.ip_addr == tmp_null) {
        // address is null... update!
        info.ip_addr = info2.ip_addr;
        return info;
    } else if (info2.ip_addr == tmp_null) {
        //dont update!
        return info;
    }

    // Both ips are not 0.0.0.0
    if (info.ip_addr == info2.ip_addr) {
        //found same IP addr no problem!
        return info;
    } else {
        //found differnet ip addr!
        /*if(info2.ip_addr.is_private())
           {
                //ip is local
                //add dev ip to vector and local ip from dev2 to ip_addr
                if(info.ip_addr.is_private())
                {
                        //Both private !*/
        auto addresses = dev->get_ip_addresses();
        for (auto string_ip : addresses) {
            if (string_ip.second == info2.ip_addr)
                return info;
        }
        if (info.ip_addr < info2.ip_addr) {

            dev->set_ip_addresses(info2.ip_addr, " ");
        } else {
            auto old_ip = info.ip_addr;
            info.ip_addr = info2.ip_addr;
            dev->set_ip_addresses(old_ip, " ");
        }
/*                        }
                        else
                        {
                                //info1 is not private!
                                auto old_ip = info.ip_addr;
                                info.ip_addr = info2.ip_addr;
                                dev->set_ip_addresses(old_ip, " ");
                        }
                }
                else
                {
                        if(info.ip_addr.is_private())
                        {
                                dev->set_ip_addresses(info2.ip_addr, " ");
                        }
                        else
                        {
                                if(info.ip_addr < info2.ip_addr)
                                {
                                        dev->set_ip_addresses(info2.ip_addr, " ");
                                }
                                else
                                {
                                        auto old_ip = info.ip_addr;
                                        info.ip_addr = info2.ip_addr;
                                        dev->set_ip_addresses(old_ip, " ");
                                }
                        }
                }*/
    }
    return info;
}
