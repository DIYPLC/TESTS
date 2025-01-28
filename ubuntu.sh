#!/bin/bash

echo "install software"
#sudo apt-get update && sudo apt-get dist-upgrade
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install gcc
sudo apt-get install g++
sudo apt-get install make
sudo apt-get install python3-pip
sudo apt install python3-tk
sudo apt-get install arduino
sudo apt-get install git
sudo apt-get install vim
sudo apt-get install nmap
sudo apt-get install net-tools
sudo apt-get install htop
sudo apt-get install cmatrix
sudo apt-get install mc
sudo apt-get install tmux
#sudo apt install qbittorrent-nox
#sudo apt-get install dosbox

echo "ubuntu software"
#tcpdump
#Wireshark
#nmap
#strace
#latex
#OBS Studio
#LibreCAD
#libreoffice
#FreeCAD
#Cura
#KiCAD
#VLC
#gimp
#InkScape
#pinta
#Wine
#VirtualBox
#Telegram Desktop
#Jami
#nano
#sublime-text
#emax
#geany
#Eclipse
#chromium
#transmission
#uget
#putty
#Jupyter Notebook
#pycharm communiti edition
#7Zip

echo "example"
#python3 -m youtube_dl 'https://youtu.be/M379y0op1WM'


git clone https://github.com/DIYPLC/LIB_PLC.git
git clone https://github.com/DIYPLC/DIY_PLC.git
git clone https://github.com/DIYPLC/HMI_PLC.git
git clone https://github.com/DIYPLC/Tests.git


#sudo reboot
#sudo halt
#sudo tcpdump -vv -i eth0 port 502
#netcat 127.0.0.1 -l 5555 #server tcp
#netcat 127.0.0.1 5555 #client tcp or use putty raw mode
#find / -name __main__.py
#df -h #print disk free size
#ifconfig
#iwconfig
#pstree #see process tree
