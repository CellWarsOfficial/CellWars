#!/bin/bash


apt-get update
echo "installing NPM and nodeJS, required for reactJS."
apt-get install nodejs 
apt-get install npm
ln -s /usr/bin/nodejs /urs/bin/node
echo "updating nodeJS to latest version using n."
npm install -g n
n stable
echo "updating npm to latest version."
npm update
echo "downloading reactJS."
npm install -g react
