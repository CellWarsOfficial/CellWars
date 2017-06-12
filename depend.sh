#!/bin/bash


apt-get --assume-yes update
echo "installing NPM and nodeJS, required for reactJS."
apt-get --assume-yes install nodejs 
apt-get --assume-yes install npm
ln -s /usr/bin/nodejs /urs/bin/node
echo "updating nodeJS to latest version using n."
cd react/my-app/
npm install n -g
npm install npm@latest -g
n stable
echo "updating npm to latest version."
npm update
npm cache clean --force
echo "downloading reactJS."
npm install -g react
npm install -g react-dom
npm install -g create-react-app
npm install -g react-scripts
