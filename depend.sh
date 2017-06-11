#!/bin/bash


apt-get --assume-yes update
echo "installing NPM and nodeJS, required for reactJS."
apt-get --assume-yes install nodejs 
apt-get --assume-yes install npm
ln -s /usr/bin/nodejs /urs/bin/node
echo "updating nodeJS to latest version using n."
npm install -g n
n stable
echo "updating npm to latest version."
npm update
echo "downloading reactJS."
npm install -g react
