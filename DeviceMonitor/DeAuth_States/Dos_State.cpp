#include "Dos_State.hpp"

Dos_red_state::Dos_red_state() {
    _status = "RED STATE!";
    timestamp = std::chrono::system_clock::now();
    auto bla = std::chrono::system_clock::to_time_t(timestamp);
//  std::cout << std::ctime(&bla) << " Router is under attack! RED!" << std::endl;
}

Dos_red_state::~Dos_red_state() {

}

std::unique_ptr <Dos_state> Dos_red_state::change_state() {
    auto tmp = std::make_unique<Dos_orange_state>();
    return tmp;
}

Dos_orange_state::Dos_orange_state() {
    _status = "ORANGE STATE!";
    timestamp = std::chrono::system_clock::now();
    auto bla = std::chrono::system_clock::to_time_t(timestamp);
    //std::cout << std::ctime(&bla) << " Router is under attack! Orange!" << std::endl;
}

Dos_orange_state::~Dos_orange_state() {

}

std::unique_ptr <Dos_state> Dos_orange_state::change_state() {
    auto tmp = std::make_unique<Dos_red_state>();
    return tmp;
}


Dos_green_state::Dos_green_state() {
    _status = "GREEN STATE!";
    timestamp = std::chrono::system_clock::now();
    auto bla = std::chrono::system_clock::to_time_t(timestamp);
    //std::cout << std::ctime(&bla) << " Router is under attack!" << std::endl;
}

Dos_green_state::~Dos_green_state() {

}

std::unique_ptr <Dos_state> Dos_green_state::change_state() {
    std::chrono::time_point <std::chrono::system_clock> tim = std::chrono::system_clock::now();
    auto duration = tim - timestamp;

    //if((duration)> std::chrono::seconds(5))
    return std::make_unique<Dos_orange_state>();
    //else
    //  return std::make_unique<Dos_green_state>();;
}
