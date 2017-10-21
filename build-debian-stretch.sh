#!/bin/bash

wget -qO - http://packages.gameap.ru/debian/gameap-rep.gpg.key | apt-key add -
echo "deb http://packages.gameap.ru/debian stretch main" >> /etc/apt/sources.list

apt-get -qq update

apt-get -y install build-essential cmake

apt-get -y install libboost-dev libboost-system-dev libboost-filesystem-dev libboost-iostreams-dev libboost-thread-dev
apt-get -y --allow-unauthenticated install libboost-process-dev libbinn-dev
apt-get -y install libjsoncpp-dev libssl-dev libmysqlclient-dev

cmake .
make