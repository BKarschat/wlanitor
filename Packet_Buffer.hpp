/* TODO BSD License
   By Bastian Karschat
   02.04.2017*/

#ifndef PACKET_BUFFER_HPP
#define PACKET_BUFFER_HPP

#include "tins/tins.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <future>

class Packet_buffer {
private:

    std::mutex _monitor_mutex;
    std::condition_variable _waiting;
    std::queue <std::unique_ptr<Tins::Packet>> _packet_que;
    std::thread _sniffing_thread;
    bool stop_sniffing{false};
    std::unique_ptr <Tins::Sniffer> _sniffer;
    Tins::NetworkInterface _interface;

    void add_packet_to_queue(Tins::Packet &packet_tmp);

public:
    Packet_buffer(Tins::NetworkInterface iface);

    virtual ~Packet_buffer();

    void start_sniffing();

    void end_sniffing();

    int queue_status();

    Tins::Packet get_packet();
};

#endif
