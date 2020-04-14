#include "Monitor_sessions.hpp"

Monitor_session::Monitor_session() {
    _session_map = std::make_unique < std::map < Tins::Dot11::address_type, router_ssid_psk_ptk_handshake_type >> ();
}

Monitor_session::~Monitor_session() {

}

void Monitor_session::set_session_elements(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp,
                                           std::string ssid_tmp, std::string psk_tmp, bool ptk_tmp,
                                           bool found_handshake) {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    /*while(!_busy)
    {
      _cond.wait(lock);
      if(_stopped)
      {
        throw "set elements stopped!";
      }
    }*/
    //_busy = true;
    if (_session_map->find(device_tmp) != _session_map->end()) {
        auto session_infos = _session_map->at(device_tmp);
        if (std::get<0>(session_infos) != router_tmp) {
            //insert new session!
            router_ssid_psk_ptk_handshake_type tmp_session(router_tmp, ssid_tmp, psk_tmp, ptk_tmp, found_handshake);
            _session_map->emplace(device_tmp, tmp_session);
        }
    } else {
        router_ssid_psk_ptk_handshake_type tmp_session(router_tmp, ssid_tmp, psk_tmp, ptk_tmp, found_handshake);
        _session_map->emplace(device_tmp, tmp_session);
    }
    //_cond.notify_one();
}

void Monitor_session::set_psk_for_ssid(std::string ssid_tmp, std::string psk_tmp) {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    /*while(!_busy)
    {
      _cond.wait(lock);
      if(_stopped)
      {
        throw "set elements stopped!";
      }
    }*/
    //_busy = true;
    for (auto elem : *_session_map.get()) {
        if (std::get<1>(elem.second) == ssid_tmp) {
            router_ssid_psk_ptk_handshake_type tmp_session(std::get<0>(elem.second), ssid_tmp, psk_tmp, false, true);
            elem.second = tmp_session;
        }
    }
//  _cond.notify_one();
}


bool Monitor_session::is_session_stored(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp) {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    /*while(!_busy)
    {
      _cond.wait(lock);
      if(_stopped)
      {
        throw "get_psk stopped!";
      }
    }*/
    bool return_value{false};
    if (_session_map->find(device_tmp) != _session_map->end()) {
        auto session_infos = _session_map->at(device_tmp);
        if (std::get<0>(session_infos) == router_tmp)
            return_value = true;
    }
    //_busy = false;
    //_cond.notify_one();
    return return_value;
}

std::string Monitor_session::get_psk(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp) {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    /*while(!_busy)
    {
      _cond.wait(lock);
      if(_stopped)
      {
        throw "get_psk stopped!";
      }
    }*/
    std::string return_string{""};
    if (_session_map->find(device_tmp) != _session_map->end()) {
        auto session_infos = _session_map->at(device_tmp);
        return_string = std::get<2>(session_infos);
    }
    //_busy = false;
    //_cond.notify_one();
    return return_string;
}

std::string Monitor_session::get_ssid(Tins::Dot11::address_type router_tmp, Tins::Dot11::address_type device_tmp) {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    /*while(!_busy)
    {
      _cond.wait(lock);
      if(_stopped)
      {
        throw "get_ssid stopped!";
      }
    }*/
    std::string return_string{""};
    if (_session_map->find(device_tmp) != _session_map->end()) {
        auto session_infos = _session_map->at(device_tmp);
        return_string = std::get<1>(session_infos);
    }
//  _busy = false;
    //_cond.notify_one();
    return return_string;
}

std::map <Tins::Dot11::address_type, router_ssid_psk_ptk_handshake_type> Monitor_session::get_map_copy() {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    /*while(!_busy)
    {
      _cond.wait(lock);
      if(_stopped)
      {
        throw "get_ssid stopped!";
      }
    }*/

    //std::map<Tins::Dot11::address_type, router_ssid_psk_ptk_handshake_type> return_map(*_session_map.get());
    //_busy = false;
    //_cond.notify_one();
    return *_session_map.get();
}


bool Monitor_session::get_ptk_status(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp) {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
/*  while(!_busy)
  {
    _cond.wait(lock);
    if(_stopped)
    {
      throw "get_ssid stopped!";
    }
  }*/
    bool return_value{false};
    if (_session_map->find(device_tmp) != _session_map->end()) {
        auto session_infos = _session_map->at(device_tmp);
        return_value = std::get<3>(session_infos);
    }
    //_busy = false;
    //_cond.notify_one();
    return return_value;
}

bool Monitor_session::get_handshake_status(Tins::Dot11::address_type device_tmp, Tins::Dot11::address_type router_tmp) {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    /*while(!_busy)
    {
      _cond.wait(lock);
      if(_stopped)
      {
        throw "get_ssid stopped!";
      }
    }*/
    bool return_value{false};
    if (_session_map->find(device_tmp) != _session_map->end()) {
        auto session_infos = _session_map->at(device_tmp);
        return_value = std::get<4>(session_infos);
    }
    //_busy = false;
    //_cond.notify_one();
    return return_value;
}

void Monitor_session::stop_monitor() {
    _stopped = true;
    _cond.notify_all();
}
