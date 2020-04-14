#!/bin/sh
#connect with wlan1 to hotspot with internet
#wlan0 creates new hotspot with same name
#$1 = ssid 
#$2 = WEP Key
#by k0rn

#works with isc-dhcp-server
#works with hostapd

#connect with wlan1 to hotspot
#get mac
mac=$(iwlist "wlan1" scan | grep -B 7 "$1" -A5 | grep -i Address| sed 's/.*Address: //g')
echo new "$mac"

ifconfig wlan0 down
ifconfig wlan0 hw ether "$mac"
iwconfig wlan0 ap any
ifconfig wlan0 up
ifconfig wlan0 192.168.23.1



#config files 	 	
./makeconfig.sh "$1" "$2"

#start dhcp server for new hosts
sudo /etc/init.d/isc-dhcp-server start
#enable routing
sudo sysctl -w net.ipv4.ip_forward=1
#Forwarding and NAT
sudo iptables -t nat -A POSTROUTING -o wlan1 -j MASQUERADE
sudo iptables -A FORWARD -i wlan1 -s 192.168.23.0/24 -m conntrack --ctstate NEW -j ACCEPT
#sudo iptables -A FORWARD -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT
#Run hostapd
sudo hostapd /etc/hostapd.conf
#Now its running
#Stop it
sudo iptables -F
#todo the other 
#Disable routing
sudo sysctl net.ipv4.ip_forward=0
#Disable dhcp 
sudo /etc/init.d/isc-dhcp-server stop
sudo service hostapd stop
