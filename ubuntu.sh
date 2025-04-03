#!/bin/bash

echo "install software"
#sudo apt-get update && sudo apt-get dist-upgrade
sudo apt update
sudo apt upgrade
sudo apt install gcc g++ make git vim htop cmatrix mc tmux python3-pip
sudo apt install arduino
sudo apt install gcc g++ make python3-pip python3-tk git vim htop cmatrix mc tmux #_del_me_
sudo apt install gcc
sudo apt install g++
sudo apt install make
sudo apt install python3-pip
sudo apt install python3-tk
sudo apt install git
sudo apt install vim
sudo apt install nmap
sudo apt install net-tools
sudo apt install htop
sudo apt install cmatrix
sudo apt install mc
sudo apt install tmux
sudo apt install qbittorrent-nox
sudo apt install dosbox

git clone https://github.com/DIYPLC/LIB_PLC.git
git clone https://github.com/DIYPLC/DIY_PLC.git
git clone https://github.com/DIYPLC/HMI_PLC.git
git clone https://github.com/DIYPLC/Tests.git

echo "ubuntu software"
#dpkg менеджер пакетов
#aptitude менеджер пакетов
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
#wifite

echo "example"
#python3 -m youtube_dl 'https://youtu.be/M379y0op1WM'

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
