#include "Packet_Buffer.hpp"

void Packet_buffer::add_packet_to_queue(Tins::Packet &packet_tmp) {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    _packet_que.push(std::make_unique<Tins::Packet>(packet_tmp));
}


Packet_buffer::Packet_buffer(Tins::NetworkInterface iface) {
    _interface = iface;

    Tins::SnifferConfiguration config;
    config.set_rfmon(true);
    config.set_promisc_mode(true);

    _sniffer = std::make_unique<Tins::Sniffer>(_interface.name(), config);
}

Packet_buffer::~Packet_buffer() {
    _sniffing_thread.join();
}

void Packet_buffer::start_sniffing() {
    _sniffing_thread = std::thread([&]() {
        for (auto &packet : *_sniffer.get()) add_packet_to_queue(packet);
    });
}

void Packet_buffer::end_sniffing() {
    _sniffing_thread.join();
}

int Packet_buffer::queue_status() {
    if (!_packet_que.empty())
        return 1;
    else
        return 0;
}


Tins::Packet Packet_buffer::get_packet() {
    std::unique_lock <std::mutex> lock(_monitor_mutex);
    auto packet = *_packet_que.front().get();
    _packet_que.pop();
    return packet;
}
