#!/bin/sh
#make config files for hotspot.sh
#$1 = ssid
#$2 = wep key
#by k0rn

if [ -f /etc/hostapd.conf ]
	then
	echo hostapd.conf already exists!
	else
	touch "/etc/hostapd.conf"
fi

#Define iface 
	echo -n > /etc/hostapd.conf

	echo "interface=wlan0" >> /etc/hostapd.conf
	echo "driver=nl80211" >> /etc/hostapd.conf
	echo "ssid=$1" >> /etc/hostapd.conf
	echo "hw_mode=g" >> /etc/hostapd.conf
	echo "channel=6" >> /etc/hostapd.conf
	
	if [ "$2" != '' ]
		then
		echo "wep_default_key=0" >> /etc/hostapd.conf
		echo "wep_key0=$2" >> /etc/hostapd.conf
	fi

	echo -n >  /etc/dhcp/dhcpd.conf



	echo "default-lease-time 600;" >> /etc/dhcp/dhcpd.conf
	echo "max-lease-time 7200;">> /etc/dhcp/dhcpd.conf
	echo "authoritative;">> /etc/dhcp/dhcpd.conf
	echo "subnet 192.168.23.0 netmask 255.255.255.0 {">> /etc/dhcp/dhcpd.conf
  	echo "range 192.168.23.10 192.168.23.254;" >> /etc/dhcp/dhcpd.conf
  	echo "option routers 192.168.23.1;">> /etc/dhcp/dhcpd.conf
  	echo "option ip-forwarding on;">> /etc/dhcp/dhcpd.conf
  	echo "option subnet-mask 255.255.255.0;" >> /etc/dhcp/dhcpd.conf
  	echo "option broadcast-address 192.168.23.255;" >> /etc/dhcp/dhcpd.conf
 	echo "interface wlan0;}" >> /etc/dhcp/dhcpd.conf

