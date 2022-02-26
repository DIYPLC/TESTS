#!/bin/bash

echo "install software"
#sudo apt-get update && sudo apt-get dist-upgrade
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install gcc
sudo apt-get install g++
sudo apt-get install make
sudo apt-get install python3-pip
sudo apt-get install arduino
sudo apt-get install git
sudo apt-get install vim
sudo apt-get install nmap
sudo apt-get install net-tools
sudo apt-get install htop
sudo apt-get install cmatrix
sudo apt-get install mc
sudo apt-get install tmux
#sudo apt-get install dosbox

echo "ubuntu software"
#tcpdump
#strace
#latex
#OBS Studio
#LibreCAD
#libreoffice
#FreeCAD
#VLC
#gimp
#Wine
#VirtualBox
#Telegram Desktop
#nano
#sublime-text
#chromium
#putty
#geany

echo "example"
#git clone https://github.com/DIYPLC/DIY_PLC_V3.git
#git clone https://github.com/DIYPLC/Test.git
#sudo reboot
#sudo halt
#sudo tcpdump -vv -i eth0 port 502
#netcat 127.0.0.1 -l 5555 #server tcp
#netcat 127.0.0.1 5555 #client tcp or use putty raw mode
#find / -name __main__.py
#df -h #print disk free size
#ifconfig
#iwconfig
