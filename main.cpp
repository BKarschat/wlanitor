//
// Created by Bastian Karschat on 4/14/20.
//

#include "Controller/WLAN_Controller.cpp"

int main() {
    auto controller = new wlan_controller();
    controller->init();
    controller->noisy_automatic();
}