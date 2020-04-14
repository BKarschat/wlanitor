/* TODO BSD License
by Bastian Karschat
02.04.2017*/

#ifndef DOS_STATE_HPP
#define DOS_STATE_HPP

#include <chrono>
#include <iostream>
#include <memory>

class Dos_state {
//Abstract class!
protected:
public:
    Dos_state() {}

    virtual ~Dos_state() {}

    std::string _status{"State"};
    std::chrono::time_point <std::chrono::system_clock> timestamp;

    virtual std::unique_ptr <Dos_state> change_state() = 0;
};

class Dos_red_state : public Dos_state {
private:
public:
    Dos_red_state();

    ~Dos_red_state();

    virtual std::unique_ptr <Dos_state> change_state();
};

class Dos_orange_state : public Dos_state {
private:
public:
    Dos_orange_state();

    ~Dos_orange_state();

    virtual std::unique_ptr <Dos_state> change_state();
};

class Dos_green_state : public Dos_state {
private:
public:
    Dos_green_state();

    virtual ~Dos_green_state();

    virtual std::unique_ptr <Dos_state> change_state();
};


#endif
